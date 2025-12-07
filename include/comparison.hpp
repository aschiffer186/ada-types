#ifndef COMPARISON_HPP
#define COMPARISON_HPP

#include <compare>
#include <concepts>

namespace ada_types {
template <typename Derived> struct comparison_operators {
  friend constexpr auto operator==(const Derived& a_lhs, const Derived& a_rhs)
      -> bool
    requires std::equality_comparable<typename Derived::underlying_type>
  {
    return a_lhs.get() == a_rhs.get();
  }

  friend constexpr auto operator<=>(const Derived& a_lhs, const Derived& a_rhs)
    requires std::three_way_comparable<typename Derived::underlying_type>
  {
    return a_lhs.get() <=> a_rhs.get();
  }
};
} // namespace ada_types

#endif