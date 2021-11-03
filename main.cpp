#define VERSION "0.1.0"
#include "src/brainfucknt.hpp"
#include "src/str2bfnt.hpp"
#include "src/transpile.hpp"
#include <fstream>

using namespace bfnt;

void brainfucknt_repl(size_t tape_len = 32) {
  // * is broken in the repl
  std::cout << "Brainfuckn't repl version " << VERSION << " with precjmp"
            << std::endl
            << "Created by 4gboframram" << std::endl
            << "Type 'exit' to exit or type 'clear' to reset the tapes."
            << std::endl
            << "-------------------------------------------------------"
            << std::endl
            << std::endl;
clear:
  size_t size = 1;
  auto t1 = new Tape(size, tape_len);
  auto t2 = new Tape(size, tape_len);
  std::string code;

start_repl:
  std::cout << "bfnt>> ";
  std::cin >> code;
  if (code == "exit")
    return;
  else if (code == "clear") {
    delete t1;
    delete t2;
    size = 1;
    goto clear;
  }

  auto pos = code.begin();
  const auto beg = code.begin();
  const auto end = code.end();
  auto open_count = 0;
  auto close_count = 0;
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
    goto start_repl;
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
  code = "";
  std::cout << *t1 << std::endl << *t2 << std::endl;
  goto start_repl; // The goto looks nicer than having a double nested while
                   // loop
  return;
}

[[noreturn]] void help(std::string command = "") {
  if (command == "") {
    std::cout << "     Commands" << std::endl
              << "--------------------" << std::endl
              << std::endl;

    std::cout << "No commands ->       Starts the brainfuckn't repl"
              << std::endl
              << std::endl;

    std::cout << "[filename] ->        Interprets the file with the default "
                 "interpreter (precjmp)"
              << std::endl
              << std::endl;

    std::cout << "-h, --help ->        Shows this message" << std::endl
              << std::endl;

    std::cout << "-i, --interpreter [precjmp | instcnt] ->    Interprets the "
                 "file with that interpreter. "
              << "Use -h interpreter for more information" << std::endl
              << std::endl;

    std::cout
        << "-len, --tapelen [number] ->    Changes the length of the tape used."
        << std::endl
        << std::endl;

    std::cout << "-str, --string ->    Convert a string to brainfuckn't. "
                 "WARNING: The output code is very unoptimized"
              << std::endl
              << std::endl;

    std::cout << "-t, --transpile [file] ->    Transpiles the code to c++ that "
                 "can be compiled with your favorite compiler. "
              << std::endl
              << std::endl;

    std::cout << "-o, --output ->    Output file for the transpiler. Should be "
                 "placed before the transpile command."
              << std::endl
              << std::endl;
  } else if (command == "interpreter") {
    std::cout << "     Interpreters" << std::endl
              << "--------------------" << std::endl
              << std::endl;
    std::cout
        << "precjmp ->    (default) Calculates the jump positions of all loops "
           "before running. Increases execution speed with a little "
           "overhead from the additional stage of finding all loops."
        << std::endl
        << std::endl;
    std::cout << "instcnt ->    Outputs the number of instructions in the code "
                 "after execution."
              << std::endl
              << std::endl;
  } else {
    std::cout << "Unknown command '" << command << "'" << std::endl;
  }
  exit(0);
}

#define FILE_NOT_FOUND                                                         \
  std::cerr << "Error: Could not find input file '" << filename << "'"         \
            << std::endl;                                                      \
  return;

void parse_args(int argc, const char *argv[]) {
  auto i = 1;
  std::string code;
  std::string arg;
  std::fstream file;
  std::string filename;
  std::ostringstream ss;
  std::string out = "out.cpp";
  size_t tape_len = 30000;
  while (i < argc) {
    arg = argv[i];
    if (arg == "-h" || arg == "--help") {
      if (i + 1 < argc) {
        help(argv[i + 1]);
      } else {
        help("");
      }
    } else if (arg == "-i" || arg == "--interpreter") {
      if (++i >= argc) {
        std::cerr << "No file provided. Exiting" << std::endl;
        return;
      }
      auto inter = std::string(argv[i]);

      if (inter == "precjmp") {
        if (file) {
          ss << file.rdbuf();
          brainfucknt(ss.str(), tape_len);
          ss.str() = "";
          file.close();
        } else {
          FILE_NOT_FOUND
        }
      } else if (inter == "instcnt") {
        file.open(filename = argv[++i]);
        if (file) {
          ss << file.rdbuf();

          std::cout << std::endl
                    << brainfucknt_inst_count(ss.str(), tape_len)
                    << " instructions" << std::endl;
          ss.str() = "";
          file.close();
        } else {
          FILE_NOT_FOUND
        }
      } else {
        std::cout << "Unknown interpreter '" << inter << "'" << std::endl;
        return;
      }
      i++;

    } else if (arg == "-len" || arg == "--tapelen") {
      try {
        tape_len = std::stoi(argv[++i]);
      } catch (std::logic_error) {
        std::cerr << "The argument for -len must be a valid integer";
      }
    } else if (arg == "-str" || arg == "--string") {
      try {
        std::cout << string2bfnt(argv[++i]) << std::endl;
      } catch (std::logic_error) {
        std::cout << "The argument for -str must exist" << std::endl;
      }
      return;
    } else if (arg == "-t" || arg == "--transpile") {
      file.open(filename = argv[++i]);
      if (file) {
        ss << file.rdbuf();
        transpile(ss.str(), out, tape_len);
        ss.str() = "";
        file.close();
      } else {
        FILE_NOT_FOUND
      }
    } else if (arg == "-o" || arg == "--output") {
      out = argv[++i];
    } else {
      file.open(filename = argv[i]);
      if (file) {
        ss << file.rdbuf();
        brainfucknt(ss.str(), tape_len);
        ss.str() = "";
        file.close();
      } else {
        FILE_NOT_FOUND
      }
    }
    i++;
  }
}

int main(int argc, const char *argv[]) {
  if (argc <= 1) {
    brainfucknt_repl();
  } else {
    parse_args(argc, argv);
  }
}