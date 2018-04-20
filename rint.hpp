
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

template<typename R, typename Val>
static void verify_range(Val val, char const* msg) {
  if(std::numeric_limits<R>::min() > val
    || std::numeric_limits<R>::max() < val) {
    throw std::out_of_range(msg);
  }
}

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
      detail::verify_range<value_type>(val, "rint: failed to initialize");
  }

  ranged_int() : value(0) { }
  ~ranged_int() = default;

  ranged_int(ranged_int const&) = default;
  ranged_int& operator=(ranged_int const&) = default;

  ranged_int(ranged_int&&) = default;
  ranged_int& operator=(ranged_int&&) = default;


  value_type operator*() const { return value; }

  template<auto min, auto max>
  inline bool operator<(ranged_int<min, max> const& rhs) const {
    return value < rhs.value;
  }
  template<auto min, auto max>
  inline bool operator>(ranged_int<min, max> const& rhs) const {
    return value > rhs.value;
  }
  template<auto min, auto max>
  inline bool operator<=(ranged_int<min, max> const& rhs) const {
    return !(*this > rhs);
  }
  template<auto min, auto max>
  inline bool operator>=(ranged_int<min, max> const& rhs) const {
    return !(*this < rhs);
  }
  template<auto min, auto max>
  inline bool operator==(ranged_int<min, max> const& rhs) const {
    return value == rhs.value;
  }
  template<auto min, auto max>
  inline bool operator!=(ranged_int<min, max> const& rhs) const {
    return !(*this == rhs);
  }

private:
  value_type value;
};

template<typename t, auto min, auto max>
t to_integral(ranged_int<min, max> val) {
  detail::verify_range<t>(*val, "rint: unable to cast");
  return static_cast<t>(*val);
}

}
