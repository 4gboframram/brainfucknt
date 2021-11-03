#ifndef BRAINFUCKNT
#define BRAINFUCKNT
#include <boost/dynamic_bitset.hpp>
#include <iostream>
#include <sstream>
#include <tuple>
#include <unordered_map>

#define R2(n) n, n + (2 << 6), n + (1 << 6), n + (3 << 6)
#define R4(n) R2(n), R2(n + (2 << 4)), R2(n + (1 << 4)), R2(n + (3 << 4))
#define R6(n) R4(n), R4(n + (2 << 2)), R4(n + (1 << 2)), R4(n + (3 << 2))

namespace bfnt {
const unsigned char lookuptable[256] = {R6(0), R6(2), R6(1), R6(3)};
typedef boost::dynamic_bitset<> Bits;

typedef struct Pointer {
  Bits &data;
  size_t pos;
  size_t &size;

  inline void set_data(Bits);
  inline Bits get_data();
  inline Pointer(size_t &size_ref, Bits &d) : size(size_ref), data(d) {
    size = 1;
    pos = 0;
  }
} Pointer;

inline void Pointer::set_data(Bits b) {
  for (auto i = 0; i < size; i++) {
    data[pos + i] = b[i];
  }
}
inline Bits Pointer::get_data() {
  Bits b(size);
  for (auto i = 0; i < size; i++) {
    b[size - i - 1] = data[pos + i];
  }
  return b;
}
typedef struct Tape {
  Pointer pointer;
  Bits data;
  inline long to_long() { return pointer.get_data().to_ulong(); }

  inline Tape(size_t &size_ref, size_t len)
      : data(len), pointer(size_ref, data) {}

} Tape;

inline std::ostream &operator<<(std::ostream &o, Tape t) {
  std::stringstream temp("");
  temp << t.data;
  std::string out = temp.str();
  std::reverse(out.begin(), out.end());
  o << out << std::endl;
  for (auto i = 0; i < t.pointer.pos + t.pointer.size; i++) {
    if (i >= t.pointer.pos) {
      o << '^';
    } else {
      o << ' ';
    }
  }
  o << " (size: " << t.pointer.size << ", pos: " << t.pointer.pos << ')'
    << std::endl;
  return o;
}
// clang-format off

// <> move first tape's pointer left / right
// {} move second tape's pointer l/r
// ^|& bitwise ops between contents of the pointers
// +- increase / decrease the pointer size. Size is unsigned. Pointers expand to the right.
// [inst] loop while the pointer is not full of zeroes or a zero size 
// ~ bitwise not of first pointer
// * get an ascii input and put the numeric value into the first pointer. If the number is too large for the pointer, then truncate the bytes so that the lowest value bytes are put into the pointer.
// . write the ascii character of the first pointer's contents to standard output
// , write the numeric value of the current position to standard output
// _ print out both tapes with the highest bits to the left and an indication of pointer size and position. used for debug
// @ swap first and second tapes
// The language is turing complete because it can simulate brainfuck with binary cells by keeping the pointer size to 1 and ignoring the second pointer.

// clang-format on
#define BOP(OP)                                                                \
  inline void operator OP(Tape &t1, Tape &t2) {                                \
    for (auto i = 0; i < t1.pointer.size; i++) {                               \
      t1.data[t1.pointer.pos + i] OP## = t2.data[t2.pointer.pos + i];          \
    }                                                                          \
  }
BOP(^)
BOP(&)
BOP(|)
inline void operator~(Tape &t) {
  for (auto i = 0; i < t.pointer.size; i++) {
    t.data[t.pointer.pos + i] = ~t.data[t.pointer.pos + i];
  }
}
inline void operator<<(Tape &t, unsigned long i) {
  Bits temp = Bits(t.pointer.size, i);
  t.pointer.set_data(temp);
}

inline void find_matching(std::string::iterator &it) {
  char step;
  if (*it == '[')
    step = 1;
  else if (*it == ']')
    step = -1;
  else
    return;

  size_t depth = step;
  while (depth) {
    it += step;
    if (*it == '[')
      depth++;
    else if (*it == ']')
      depth--;
  }
  return;
}

inline auto find_matching_jc(std::string::iterator it,
                             std::string::iterator beg,
                             std::string::iterator end) {
  char step;
  if (*it == '[')
    step = 1;
  else if (*it == ']')
    step = -1;
  else
    return it;
  size_t depth = step;
  while (depth) {
    it += step;
    if (it < beg || it > end)
      return it;
    if (*it == '[')
      depth++;
    else if (*it == ']')
      depth--;
  }
  return it;
}
auto brainfucknt(std::string code, size_t tape_len,
                 std::ostream &out = std::cout,
                 std::istream &input = std::cin) {
  // yeah i ain't typing the long-ass name of the return type
  size_t size = 1;
  auto t1 = new Tape(size, tape_len);
  auto t2 = new Tape(size, tape_len);
  auto pos = code.begin();
  auto open_count = 0;
  auto close_count = 0;
  const auto beg = code.begin();
  const auto end = code.end();
  std::unordered_map<int, int> jumps;
  for (auto i = code.begin(); i < code.end(); i++) {
    if (*i == '[' || *i == ']') {
      jumps[i - beg] = find_matching_jc(i, beg, end) - i;
      if (*i == '[')
        open_count++;
      else if (*i == ']')
        close_count++;
    }
  }
  if (open_count != close_count) {
    std::cerr << "SyntaxError: Unmatched loops somewhere." << std::endl
              << "Like literally, this is the only possible syntax error that "
                 "you could mess up. Good job."
              << std::endl;
    return std::make_tuple(*t1, *t2, size);
  }
  while (pos < end) {
    switch (*pos) {
    case '~':
      ~*t1;
      break;
    case '<':
      t1->pointer.pos--;
      break;
    case '>':
      t1->pointer.pos++;
      break;
    case '{':
      t2->pointer.pos--;
      break;
    case '}':
      t2->pointer.pos++;
      break;
    case '^':
      *t1 ^ *t2;
      break;
    case '|':
      *t1 | *t2;
      break;
    case '&':
      *t1 &*t2;
      break;
    case '@':
      std::swap(t1, t2);
      break;
    case '+':
      size++;
      break;
    case '-':
      size--;
      break;
    case '[': {
      if (t1->pointer.get_data().none() || !size) {
        pos += jumps[pos - beg];
      }
      break;
    }
    case ']': {
      if (t1->pointer.get_data().any() && size) {
        pos += jumps[pos - beg];
      }
      break;
    }
    case '.':
      std::cout << (char)t1->to_long();
      break;
    case ',':
      std::cout << t1->to_long();
      break;
    case '_':
      std::cout << *t1 << *t2;
      break;
    case '*':
      *t1 << (unsigned char)lookuptable[std::cin.get()];
      break; // reverse the order of the bits
    }
    pos++;
  }
  return std::make_tuple(*t1, *t2, size);
}

auto brainfucknt_inst_count(std::string code, size_t tape_len,
                            std::ostream &out = std::cout,
                            std::istream &input = std::cin) {
  size_t size = 1;
  auto t1 = new Tape(size, tape_len);
  auto t2 = new Tape(size, tape_len);
  auto pos = code.begin();
  auto open_count = 0;
  auto close_count = 0;
  const auto beg = code.begin();
  const auto end = code.end();
  auto insts = 0;
  std::unordered_map<int, int> jumps;
  for (auto i = code.begin(); i < code.end(); i++) {
    if (*i == '[' || *i == ']') {
      jumps[i - beg] = find_matching_jc(i, beg, end) - i;
      if (*i == '[')
        open_count++;
      else if (*i == ']')
        close_count++;
    }
  }
  if (open_count != close_count) {
    std::cerr << "SyntaxError: Unmatched loops somewhere." << std::endl
              << "Like literally, this is the only possible syntax error that "
                 "you could mess up. Good job."
              << std::endl;
    return insts;
  }
  while (pos < end) {
    switch (*pos) {
    case '~':
      ~*t1;
      break;
    case '<':
      t1->pointer.pos--;
      break;
    case '>':
      t1->pointer.pos++;
      break;
    case '{':
      t2->pointer.pos--;
      break;
    case '}':
      t2->pointer.pos++;
      break;
    case '^':
      *t1 ^ *t2;
      break;
    case '|':
      *t1 | *t2;
      break;
    case '&':
      *t1 &*t2;
      break;
    case '@':
      std::swap(t1, t2);
      break;
    case '+':
      size++;
      break;
    case '-':
      size--;
      break;
    case '[': {
      if (t1->pointer.get_data().none() || !size) {
        pos += jumps[pos - beg];
      }
      break;
    }
    case ']': {
      if (t1->pointer.get_data().any() && size) {
        pos += jumps[pos - beg];
      }
      break;
    }
    case '.':
      std::cout << (char)t1->to_long();
      break;
    case ',':
      std::cout << t1->to_long();
      break;
    case '_':
      std::cout << *t1 << *t2;
      break;
    case '*':
      *t1 << (unsigned char)lookuptable[std::cin.get()];
      break; // reverse the order of the bits
    }
    pos++;
    insts++;
  }
  return insts;
}
} // namespace bfnt
#endif
