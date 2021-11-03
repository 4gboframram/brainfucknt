#ifndef STR2BFNT
#define STR2BFNT
#include <bitset>
#include <iostream>
#include <sstream>
typedef std::bitset<8> char_bits;
namespace bfnt {
std::string string2bfnt(std::string input) {
  std::string output;
  std::stringstream bin;
  for (auto c : input) {
    char_bits bits(c);
    bin << bits;
    for (auto i : bin.str()) {
      output += ">";
      if (i == '1') {
        output += "~";
      }
    }
    output += "+++++++<<<<<<<.&-------";
  }
  std::cout << output << std::endl;
  return output;
}
} // namespace bfnt
#endif