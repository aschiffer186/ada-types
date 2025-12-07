#ifndef ADA_TYPES_FWD_HPP
#define ADA_TYPES_FWD_HPP

#include <algorithm>
#include <cstddef>

namespace ada_types {
template <std::size_t N> struct string_literal {
  char m_data[N];

  constexpr string_literal(const char (&a_data)[N]) {
    std::ranges::copy(a_data, m_data);
  }
};

template <std::size_t N> string_literal(const char (&)[N]) -> string_literal<N>;

template <std::size_t N1, std::size_t N2>
constexpr auto operator==(const string_literal<N1> a_lhs,
                          const string_literal<N2> a_rhs) -> bool {
  if (N1 != N2) {
    return false;
  }
  return std::ranges::equal(a_lhs.m_data, a_rhs.m_data);
}

template <typename T, string_literal Tag> class strong_type;

template <std::signed_integral T, string_literal Tag, std::intmax_t Min,
          std::intmax_t Max>
class signed_integer;
} // namespace ada_types

#endif