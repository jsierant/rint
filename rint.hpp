
#include <limits>
#include <cstdint>
#include <stdexcept>
#include <tuple>

namespace rint {
namespace detail {

struct none {};

template<auto min, auto max> struct range_signed {
  template<typename T> constexpr static bool match() {
    return std::numeric_limits<T>::min() <= min
      && std::numeric_limits<T>::max() >= max;
  }
};

template<auto min, auto max> struct range_unsigned {
  template<typename T> constexpr static bool match() {
    return std::numeric_limits<T>::max() >= max;
  }
};

template<typename pred, bool, typename matching, typename... candidates>
struct find_type_impl { };

template<typename pred, typename matching, typename head, typename... tail>
struct find_type_impl<pred, false, matching, head, tail...>
  : find_type_impl<pred, pred::template match<head>(), head, tail...> {};

template<typename pred, typename matching>
struct find_type_impl<pred, false, matching> { using type = none; };

template<typename pred, typename matching, typename... candidates>
struct find_type_impl<pred, true, matching, candidates...> {
  using type = matching;
};

template<typename pred, typename head, typename... tail> struct find_type
  : find_type_impl<pred, pred::template match<head>(), head, tail...> { };

template<auto min, auto max, bool is_signed,
         typename sign_candidates, typename unsigned_candidates>
struct value_type_impl {};

template<auto min, auto max, typename unsigned_cont, typename... candidates>
struct value_type_impl<min, max, true, std::tuple<candidates...>, unsigned_cont>
  : find_type<range_signed<min, max>, candidates...>{
};

template<auto min, auto max, typename signed_cont, typename... candidates>
struct value_type_impl<min, max, false, signed_cont, std::tuple<candidates...>>
  : find_type<range_unsigned<min, max>, candidates...>{
};

template<auto min, auto max,
         typename sign_candidates, typename unsigned_candidates>
struct value_type
  : value_type_impl<min, max, (min<0), sign_candidates, unsigned_candidates> {};

template<typename R, typename Val>
static void verify_range(Val val, char const* msg) {
  if(std::numeric_limits<R>::min() > val
    || std::numeric_limits<R>::max() < val) {
    throw std::out_of_range(msg);
  }
}

template<typename T1, typename T2>
constexpr bool same_sign() {
  return std::is_signed<T1>::value == std::is_signed<T2>::value;
}

}

template<auto min_v, auto max_v>
class ranged_int {
public:
  using value_type =
    typename detail::value_type<
      min_v, max_v,
      std::tuple<std::int8_t, std::int16_t, std::int32_t>,
      std::tuple<std::uint8_t, std::uint16_t, std::uint32_t>
    >::type;
  static_assert(!std::is_same<value_type, detail::none>::value,
    "Unable to detect the value type for given range!");

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
    static_assert(
      detail::same_sign<value_type,
                        typename ranged_int<min, max>::value_type>(),
      "Comparision of values with the same sign is alowed");
    return value < rhs.value;
  }
  template<auto min, auto max>
  inline bool operator>(ranged_int<min, max> const& rhs) const {
    static_assert(
      detail::same_sign<value_type,
                        typename ranged_int<min, max>::value_type>(),
      "Comparision of values with the same sign is alowed");
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
    static_assert(
      detail::same_sign<value_type,
                        typename ranged_int<min, max>::value_type>(),
      "Comparision of values with the same sign is alowed");
    return value == *rhs;
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
