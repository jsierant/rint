#include <bandit/bandit.h>
#include <rint.hpp>
#include <iostream>

using namespace rint;
using namespace snowhouse;
using namespace bandit;

template<auto min, auto max, typename E>
void expect_value_type() {
  bool const is_same = std::is_same<
    typename ranged_int<min, max>::value_type, E>::value;
  AssertThat(is_same, IsTrue());
}

template<typename E>
void expect_value_type_for_limits() {
  expect_value_type<std::numeric_limits<E>::min(),
    std::numeric_limits<E>::max(), E>();
}
template<typename T>
auto make_ranged_int_with_min() {
  return ranged_int<std::numeric_limits<T>::min(), 0>
    {std::numeric_limits<T>::min()};
}

go_bandit([]{
  describe("selecting type", []{

    it("shall be uint8", []{
      expect_value_type_for_limits<std::uint8_t>();
    });

    it("shall be int8", []{
      expect_value_type_for_limits<std::int8_t>();
    });

    it("shall be uint16", []{
      expect_value_type_for_limits<std::uint16_t>();
      expect_value_type<std::numeric_limits<std::uint16_t>::min(),
        std::numeric_limits<std::uint8_t>::max()+1, std::uint16_t>();
    });

    it("shall be int16", []{
      expect_value_type_for_limits<std::int16_t>();
      expect_value_type<std::numeric_limits<std::int8_t>::min()-1,
        std::numeric_limits<std::uint8_t>::max()+1, std::int16_t>();
    });

    it("shall be uint32", []{
      expect_value_type_for_limits<std::uint32_t>();
      expect_value_type<std::numeric_limits<std::uint32_t>::min(),
        std::numeric_limits<std::uint16_t>::max()+1, std::uint32_t>();
    });

    it("shall be int32", []{
      expect_value_type_for_limits<std::int32_t>();
      expect_value_type<std::numeric_limits<std::int16_t>::min()-1,
        std::numeric_limits<std::int16_t>::max()+1, std::int32_t>();
    });
  });

  describe("min max", []() {
    it("min shall be smaller then max", []{
      ranged_int<29, 30>{};
    });
  });

  describe("equality comparision", []() {
    it("two rint shall be equal", []{
      AssertThat((ranged_int<0, 30>{1}), Equals((ranged_int<0, 30>{1})));
      AssertThat((ranged_int<0, 30>{1}), Equals((ranged_int<0, 40>{1})));
    });
    it("two rint shall not be equal", []{
      AssertThat((ranged_int<0, 30>{1}),
                 Is().Not().EqualTo((ranged_int<0, 30>{2})));
      AssertThat((ranged_int<0, 30>{1}),
                 Is().Not().EqualTo((ranged_int<0, 40>{2})));
    });
  });

  describe("comparision operator", []() {
    it("greater", []{
      AssertThat((ranged_int<0, 30>{2}),
                 IsGreaterThan((ranged_int<0, 30>{1})));
      AssertThat((ranged_int<0, 30>{2}),
                 IsGreaterThan((ranged_int<0, 40>{1})));
    });
    it("less", []{
      AssertThat((ranged_int<0, 30>{1}),
                 IsLessThan((ranged_int<0, 30>{2})));
      AssertThat((ranged_int<0, 30>{1}),
                 IsLessThan((ranged_int<0, 40>{2})));
    });
  });

  describe("cast function", []() {
    it("shall throw when converting negative val to unsigned", []{
      AssertThrows(std::out_of_range,
        (to_integral<std::uint8_t>(ranged_int<-10, 30>{-2})));
      AssertThrows(std::out_of_range,
        (to_integral<std::uint16_t>(ranged_int<-10, 30>{-2})));
      AssertThrows(std::out_of_range,
        (to_integral<std::uint32_t>(ranged_int<-10, 30>{-2})));
    });
    it("shall throw when converting out of range value", []{
      AssertThrows(std::out_of_range,
        (to_integral<std::int8_t>(make_ranged_int_with_min<std::int32_t>())));
      AssertThrows(std::out_of_range,
        (to_integral<std::int16_t>(make_ranged_int_with_min<std::int32_t>())));
      AssertThrows(std::out_of_range,
        (to_integral<std::int16_t>(
          ranged_int<0, std::numeric_limits<std::uint16_t>::max() >{
            std::numeric_limits<std::uint16_t>::max()})));
    });
  });
});
