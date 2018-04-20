
#include <limits>
#include <cstdint>
#include <stdexcept>

namespace rint {
namespace detail {

template<auto min, auto max, typename T>
constexpr bool in_range() {
  return std::numeric_limits<T>::min() <= min
    && std::numeric_limits<T>::max() >= max;
}

template<typename T, bool = false> struct fwd { using type = void; };
template<typename T> struct fwd<T, true> { using type = T; };

template<auto min, auto max, typename t, typename... types>
struct value_type { using type = t; };

template<auto min, auto max, typename head, typename... candidates>
struct value_type<min, max, void, head, candidates...> {
  using type = typename value_type<min, max,
    typename fwd<head, in_range<min, max, head>()>::type,
    candidates...>::type;
};

template<auto min, auto max>
struct value_type<min, max, void> { };

}

template<auto min_v, auto max_v>
class ranged_int {
public:
  using value_type =
    typename detail::value_type<
      min_v, max_v, void,
      std::uint8_t, std::int8_t,
      std::uint16_t, std::int16_t,
      std::uint32_t, std::int32_t
    >::type;

  static constexpr value_type min() { return min_v; }
  static constexpr value_type max() { return max_v; }

  template<typename Val>
  explicit ranged_int(Val val)
    : value(static_cast<value_type>(val)) {
    verify(val);
  }

  value_type operator*() const { return value; }

private:
  template<typename Val>
  static void verify(Val val) {
    if(std::numeric_limits<value_type>::min() > val
      || std::numeric_limits<value_type>::max() < val) {
      throw std::out_of_range("rint: failed to initilize");
    }
  }
  value_type value;
};

template<typename t, auto min, auto max>
t to_integral(ranged_int<min, max> val) {
  if(!detail::in_range<min, max, t>()) {
    throw std::out_of_range("rint: unable to cast");
  }
  return static_cast<t>(*val);
}

}
