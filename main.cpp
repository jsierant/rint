#include "rint.hpp"
#include <type_traits>
#include <cassert>
#include <iostream>

using tu8 = rint::ranged_int<0, 100>;
using ti8 = rint::ranged_int<-1, 100>;
using tu16 = rint::ranged_int<0, 256>;
using ti16 = rint::ranged_int<-1, 256>;
int main(int argc, char *argv[]) {
  assert((std::is_same<tu8::value_type, std::uint8_t>::value));
  assert((std::is_same<ti8::value_type, std::int8_t>::value));
  assert((std::is_same<tu16::value_type, std::uint16_t>::value));
  assert((std::is_same<ti16::value_type, std::int16_t>::value));
  ti8 v1(-1);
  assert((rint::to_integral<std::int8_t>(v1)));
//   assert((rint::to_integral<std::uint8_t>(v1)));
  tu8 v2(1);

  return 0;
}
