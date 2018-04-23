#include <bandit/bandit.h>
#include <rint.hpp>

using namespace rint;
using namespace snowhouse;
using namespace bandit;

template<auto min, auto max, typename E>
void expect_value_type() {
  auto const is_same = std::is_same<
    typename ranged_int<min, max>::value_type, E>::value;
  AssertThat(is_same, IsTrue());
}

go_bandit([]{
  describe("selecting type", []{

    it("shall select uint8", []{
      expect_value_type<std::numeric_limits<std::uint8_t>::min(),
        std::numeric_limits<std::uint8_t>::max(), std::uint8_t>();
      expect_value_type<0, 255, std::uint8_t>();
      expect_value_type<1, 254, std::uint8_t>();
      expect_value_type<10, 30, std::uint8_t>();
    });

    it("shall select int8", []{
      expect_value_type<std::numeric_limits<std::int8_t>::min(),
        std::numeric_limits<std::int8_t>::max(), std::int8_t>();
    });

    it("shall select uint16", []{
      expect_value_type<std::numeric_limits<std::uint16_t>::min(),
        std::numeric_limits<std::uint16_t>::max(), std::uint16_t>();
    });

    it("shall select int16", []{
      expect_value_type<std::numeric_limits<std::int16_t>::min(),
        std::numeric_limits<std::int16_t>::max(), std::int16_t>();
    });

    it("shall select uint32", []{
      expect_value_type<std::numeric_limits<std::uint32_t>::min(),
        std::numeric_limits<std::uint32_t>::max(), std::uint32_t>();
    });

    it("shall select int32", []{
      expect_value_type<std::numeric_limits<std::int32_t>::min(),
        std::numeric_limits<std::int32_t>::max(), std::int32_t>();
    });
  });

  describe("min max", []() {
    it("min shall be smaller then max", []{
      ranged_int<29, 30>{};
    });
  });
});
