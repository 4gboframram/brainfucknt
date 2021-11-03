#ifndef TRANSPILE_BFNT
#define TRANSPILE_BFNT
#include "brainfucknt.hpp"
#include <fstream>
#include <vector>
namespace bfnt {
#define START_LP "while (t1->pointer.get_data().any()){"
#define END_LP "}"

// #define XOR "*t1 ^ *t2;" // count will be filtered off
#define OR "*t1 | *t2;"
#define AND "*t1 &*t2;"

#define OUT_CHR "std::cout << (char)t1->to_long();"
#define OUT_NUM "std::cout << t1->to_long();"
#define PRINT_TAPES "std::cout << *t1 << *t2;"
#define GET_CHR "*t1 << (unsigned char)lookuptable[std::cin.get()];"

#define COUNT_MAC(NAME, VAL)                                                   \
  std::string NAME(int count) { return VAL + std::to_string(count) + ";"; }
COUNT_MAC(T1L, "t1->pointer.pos-=")
COUNT_MAC(T1R, "t1->pointer.pos+=")
COUNT_MAC(T2L, "t2->pointer.pos-=")
COUNT_MAC(T2R, "t2->pointer.pos+=")
COUNT_MAC(INCSIZE, "size+=")
COUNT_MAC(DECSIZE, "size-=")
std::string XOR(int count) {
  if (count % 2) {
    return "*t1 ^ *t2;";
  } else
    return "";
}
std::string SWAP(int count) {
  if (count % 2) {
    return "std::swap(t1, t2);";
  } else
    return "";
}
std::string NOT(int count) {
  if (count % 2)
    return "~*t1;";
  else
    return "";
}

typedef struct macro {
  char inst;
  int count;

  std::string to_str();
  friend std::ostream &operator<<(std::ostream &o, struct macro &m) {
    o << m.inst << " " << m.count << std::endl;
    return o;
  }
  bool operator!=(macro m) { return (m.inst != inst) || (m.count != count); }
  macro(char i, int c) : inst(i), count(c) {}

} macro;

std::string macro::to_str() {
  // preliminary optimization based on number of instructions alone
  std::unordered_map<char, std::string> mac_map = {
      {'~', NOT(count)},  {'[', START_LP},       {']', END_LP},
      {'<', T1L(count)},  {'>', T1R(count)},     {'{', T2L(count)},
      {'}', T2R(count)},  {'^', XOR(count)},     {'|', OR},
      {'&', AND},         {'+', INCSIZE(count)}, {'-', DECSIZE(count)},
      {'@', SWAP(count)}, {'.', OUT_CHR},        {',', OUT_NUM},
      {'*', GET_CHR},     {'_', PRINT_TAPES}};

  return mac_map[inst];
};

std::ostream &operator<<(std::ostream &o, std::vector<macro> v) {
  for (auto i : v) {
    o << i;
  }
  return o;
}

std::vector<macro> create_macros(std::string input) {
  char current_char = 0;
  input += '\x01'; // add redundant byte to make poggers
  int current_counter = 0;
  std::stringstream s;
  std::vector<macro> ret;
  for (auto i : input) {
    if (i == current_char &&
        (i == '<' || i == '>' || i == '{' || i == '}' || i == '+' || i == '-' ||
         i == '~' || i == '^' ||
         i == '@')) // all of the operations we can optimize by counting them
    {
      current_counter++;
    } else {

      s << current_char << " " << current_counter << std::endl;
      if (current_char) {
        ret.push_back(macro(current_char, current_counter));
      }
      current_char = i;
      current_counter = 1;
    }
  }
  ret.erase(std::remove_if(ret.begin(), ret.end(),
                           [](macro m) { return m.to_str() == ""; }),
            ret.end());
  return ret;
}

std::string macros_to_code(std::vector<macro> v) {
  std::string ret("");
  for (auto i : v) {
    ret += i.to_str();
    ret += '\n';
  }
  return ret;
}
std::vector<macro> simplify_inverses(std::vector<macro> v) {
  auto i = 0;
  char c;
  while (i++ < v.size()) {
    c = v[i].inst;
    // std::cout << c << std::endl;
    if ((c == '^') || (c == '@') || (c == '~')) {
      if (v[i + 1].inst == c && i < v.size()) {
        v.erase(v.begin() + i, v.begin() + i + 2);
      }
    }
  }
  return v;
}
std::vector<macro> optimize_others(std::vector<macro> v) {
  std::vector<macro> ret;
  auto i = 0;
  auto mov = 0;
  auto mov2 = 0;
  auto s = 0;
  while (i < v.size()) {
    mov = 0;
    mov2 = 0;
    s = 0;

    while ((v[i].inst == '>') || (v[i].inst == '<') || v[i].inst == '+' ||
           v[i].inst == '-' || (v[i].inst == '}') || (v[i].inst == '{')) {
      if (v[i].inst == '>')
        mov += v[i].count;
      else if (v[i].inst == '<')
        mov -= v[i].count;
      else if (v[i].inst == '}')
        mov2 += v[i].count;
      else if (v[i].inst == '{')
        mov2 -= v[i].count;
      else if (v[i].inst == '+')
        s += v[i].count;
      else if (v[i].inst == '-')
        s -= v[i].count;
      i++;
    }
    // std::cout << mov;
    if (s || mov || mov2) {
      i--;
      if (mov != 0)
        ret.push_back(macro('>', mov));
      if (s != 0)
        ret.push_back(macro('+', s));
      if (mov2 != 0)
        ret.push_back(macro('}', mov2));
    } else {
      ret.push_back(v[i]);
    }
    i++;
  }
  return simplify_inverses(ret);
}

std::vector<macro> optimize(std::vector<macro> v) {
  std::vector<macro> prev;
  std::vector<macro> cur;
  cur = v;
  do {
    prev = cur;
    cur = optimize_others(prev);
  } while (cur.size() != prev.size());

  return prev;
}
const std::string header =
    "#include <iostream>\n#include <sstream>\n#include <bitset>\n#include "
    "<algorithm>\n#define R2(n) n, n + (2 << 6), n + (1 << 6), n + (3 << "
    "6)\n#define R4(n) R2(n), R2(n + (2 << 4)), R2(n + (1 << 4)), R2(n + (3 << "
    "4))\n#define R6(n) R4(n), R4(n + (2 << 2)), R4(n + (1 << 2)), R4(n + (3 "
    "<< 2))\n\nconst unsigned char lookuptable[256] = {R6(0), R6(2), R6(1), "
    "R6(3)};\ntypedef std::bitset<16> Bits;\ntypedef struct Pointer {\n  Bits "
    "&data;\n  size_t pos;\n  size_t &size;\n\n  inline void set_data(Bits);\n "
    " inline Bits get_data();\n  inline Pointer(size_t &size_ref, Bits &d) : "
    "size(size_ref), data(d) {\n    size = 1;\n    pos = 0;\n  }\n} "
    "Pointer;\n\ninline void Pointer::set_data(Bits b) {\n  for (auto i = 0; i "
    "< size; i++) {\n    data[pos + i] = b[i];\n  }\n}\ninline Bits "
    "Pointer::get_data() {\n  Bits b;\n  for (auto i = 0; i < size; i++) {\n   "
    " b[size - i - 1] = data[pos + i];\n  }\n  return b;\n}\ntypedef struct "
    "Tape {\n  Pointer pointer;\n  Bits data;\n  inline long to_long() { "
    "return pointer.get_data().to_ulong(); }\n\n  inline Tape(size_t "
    "&size_ref)\n      : data(), pointer(size_ref, data) {}\n\n} "
    "Tape;\n\ninline std::ostream &operator<<(std::ostream &o, Tape t) {\n  "
    "std::stringstream temp(\"\");\n  temp << t.data;\n  std::string out = "
    "temp.str();\n  std::reverse(out.begin(), out.end());\n  o << out << "
    "std::endl;\n  for (auto i = 0; i < t.pointer.pos + t.pointer.size; i++) "
    "{\n    if (i >= t.pointer.pos) {\n      o << \'^\';\n    } else {\n      "
    "o << \' \';\n    }\n  }\n  o << \" (size: \" << t.pointer.size << \", "
    "pos: \" << t.pointer.pos << \')\'\n    << std::endl;\n  return "
    "o;\n}\n\n#define BOP(OP)                                                  "
    "                inline void operator OP(Tape &t1, Tape &t2) {             "
    "                       for (auto i = 0; i < t1.pointer.size; i++) {       "
    "                              t1.data[t1.pointer.pos + i] = "
    "t1.data[t1.pointer.pos + i] OP t2.data[t2.pointer.pos + i];              "
    "}                                                                         "
    "   }\nBOP(^)\nBOP(&)\nBOP(|)\ninline void operator~(Tape &t) {\n  for "
    "(auto i = 0; i < t.pointer.size; i++) {\n    t.data[t.pointer.pos + i] = "
    "~t.data[t.pointer.pos + i];\n  }\n}\ninline void operator<<(Tape &t, "
    "unsigned long i) {\n  Bits temp = Bits(i);\n  "
    "t.pointer.set_data(temp);\n}\nint main(){\n    size_t size = 1;\n    auto "
    "t1 = new Tape(size);\n    auto t2 = new Tape(size);";
void transpile(std::string code, std::string file, size_t tape_len) {
  auto macs = create_macros(code);
  auto out = bfnt::macros_to_code(bfnt::optimize(macs));
  std::fstream f(file, std::ios::app);
  size_t index = 0;
  auto h = header;
  while (true) {
    index = h.find("16", index);
    if (index == std::string::npos)
      break;
    h.replace(index, 2, std::to_string(tape_len));
    index += 2;
  }
  if (f.good()) {
    f << h << out << "}";
  } else {
    throw std::runtime_error("you found secret error lol");
  }
}
} // namespace bfnt
#endif
