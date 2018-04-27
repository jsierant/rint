
#include <limits>
#include <cstdint>
#include <stdexcept>
#include <tuple>

namespace rint {
namespace detail {

struct none {};

template<
  auto min,
  auto max
>
struct range {
  template<typename verifying_t>
  constexpr static bool match() {
    if constexpr (min < 0) {
      return std::numeric_limits<verifying_t>::min() <= min
        && std::numeric_limits<verifying_t>::max() >= max;
    }
    return std::numeric_limits<verifying_t>::max() >= max;
  }
};

template<
  typename predicate_t,
  bool,
  typename verifying_t,
  typename... candidates_t
>
struct find_type_impl { };

template<
  typename predicate_t,
  typename verifying_t,
  typename next_candidate_t,
  typename... remining_candidates_t
>
struct find_type_impl<
  predicate_t,
  false,
  verifying_t,
  next_candidate_t,
  remining_candidates_t...
>
  : find_type_impl<
      predicate_t,
      predicate_t::template match<next_candidate_t>(),
      next_candidate_t,
      remining_candidates_t...
    >
{ };

template<
  typename predicate_t,
  typename verifying_t
>
struct find_type_impl<
  predicate_t,
  false,
  verifying_t
> {
  using type = none;
};

template<
  typename predicate_t,
  typename verifying_t,
  typename... candidates_t
>
struct find_type_impl<
  predicate_t,
  true,
  verifying_t,
  candidates_t...
> {
  using type = verifying_t;
};

template<
  typename pred,
  typename head,
  typename... tail
>
struct find_type
  : find_type_impl<pred, pred::template match<head>(),
                   head, tail...>
{ };

template<
  auto min, auto max, bool is_signed,
  typename sign_candidates,
  typename unsigned_candidates
>
struct value_type_impl { };

template<
  auto min_tv, auto max_tv,
  typename unsigned_candidate_container_t,
  typename... signed_candidates_t
>
struct value_type_impl<
  min_tv, max_tv, true,
  std::tuple<signed_candidates_t...>,
  unsigned_candidate_container_t
>
  : find_type<
      range<min_tv, max_tv>,
      signed_candidates_t...
    >
{ };

template<
  auto min_tv, auto max_tv,
  typename signed_candidate_container_t,
  typename... unsigned_candidates_t
>
struct value_type_impl<
  min_tv, max_tv, false,
  signed_candidate_container_t,
  std::tuple<unsigned_candidates_t...>
>
  : find_type<
      range<min_tv, max_tv>,
      unsigned_candidates_t...
    >
{ };

template<
  auto min_tv, auto max_tv,
  typename signed_candidate_container_t,
  typename unsigned_candidate_container_t>
struct value_type
  : value_type_impl<
      min_tv, max_tv, (min_tv < 0),
      signed_candidate_container_t,
      unsigned_candidate_container_t
    >
{ };

template<
  typename t1_t,
  typename t2_t
>
constexpr bool same_sign() {
  return std::is_signed<t1_t>::value
    == std::is_signed<t2_t>::value;
}

template<
  typename subject_t,
  typename value_t
>
std::enable_if_t<same_sign<subject_t, value_t>()>
verify_range(value_t value, char const* msg) {
  if(std::numeric_limits<subject_t>::min() > value
      || std::numeric_limits<subject_t>::max() < value) {
    throw std::out_of_range(msg);
  }
}

template<
  typename subject_t,
  typename value_t
>
std::enable_if_t<
  std::is_unsigned<subject_t>::value
    && std::is_signed<value_t>::value>
verify_range(value_t value, char const* msg) {
  if(value < 0
     || std::numeric_limits<subject_t>::max()
        < static_cast<std::uint32_t>(value)) {
    throw std::out_of_range(msg);
  }
}

template<
  typename subject_t,
  typename value_t
>
std::enable_if_t<
  std::is_signed<subject_t>::value
  && std::is_unsigned<value_t>::value>
verify_range(value_t value, char const* msg) {
  if(static_cast<std::uint32_t>(
      std::numeric_limits<subject_t>::max()) < value) {
      throw std::out_of_range(msg);
  }
}

}

template<
  auto min_v, auto max_v
>
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

  template<typename value_t>
  explicit ranged_int(value_t val)
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
    return value < *rhs;
  }
  template<auto min, auto max>
  inline bool operator>(ranged_int<min, max> const& rhs) const {
    static_assert(
      detail::same_sign<value_type,
                        typename ranged_int<min, max>::value_type>(),
      "Comparision of values with the same sign is alowed");
    return value > *rhs;
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
