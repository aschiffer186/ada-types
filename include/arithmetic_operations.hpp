#ifndef ARITHMETIC_OPERATIONS_HPP
#define ARITHMETIC_OPERATIONS_HPP

#include "concepts.hpp"
namespace ada_types {
template <class Derived> class addition {
  template <strong_type_like U>
  friend constexpr auto operator+=(Derived& a_lhs, const U& a_rhs) -> Derived&
    requires add_assignable_with<typename Derived::underlying_type,
                                 typename U::underlying_type>
  {
    a_lhs.get() += a_rhs.get();
    return a_lhs;
  }

  template <strong_type_like U>
  friend constexpr auto operator+(const Derived& a_lhs, const U& a_rhs)
      -> Derived&
    requires add_with<typename Derived::underlying_type,
                      typename U::underlying_type>
  {
    return Derived(a_lhs.get() + a_rhs.get());
  }
};

template <class Derived> class subtraction {
  template <strong_type_like U>
  friend constexpr auto operator-=(Derived& a_lhs, const U& a_rhs) -> Derived&
    requires subtract_assignable_with<typename Derived::underlying_type,
                                      typename U::underlying_type>
  {
    a_lhs.get() -= a_rhs.get();
    return a_lhs;
  }

  template <strong_type_like U>
  friend constexpr auto operator-(const Derived& a_lhs, const U& a_rhs)
      -> Derived&
    requires subtract_with<typename Derived::underlying_type,
                           typename U::underlying_type>
  {
    return Derived(a_lhs.get() - a_rhs.get());
  }
};

template <class Derived> class multiplication {
  template <strong_type_like U>
  friend constexpr auto operator*=(Derived& a_lhs, const U& a_rhs) -> Derived&
    requires multiply_assignable_with<typename Derived::underlying_type,
                                      typename U::underlying_type>
  {
    a_lhs.get() *= a_rhs.get();
    return a_lhs;
  }

  template <strong_type_like U>
  friend constexpr auto operator*(const Derived& a_lhs, const U& a_rhs)
      -> Derived&
    requires multiply_with<typename Derived::underlying_type,
                           typename U::underlying_type>
  {
    return Derived(a_lhs.get() * a_rhs.get());
  }
};

template <class Derived> class division {
  template <strong_type_like U>
  friend constexpr auto operator/=(Derived& a_lhs, const U& a_rhs) -> Derived&
    requires divide_assignable_with<typename Derived::underlying_type,
                                    typename U::underlying_type>
  {
    a_lhs.get() /= a_rhs.get();
    return a_lhs;
  }

  template <strong_type_like U>
  friend constexpr auto operator/(const Derived& a_lhs, const U& a_rhs)
      -> Derived&
    requires divide_with<typename Derived::underlying_type,
                         typename U::underlying_type>
  {
    return Derived(a_lhs.get() / a_rhs.get());
  }
};

template <class Derived> class modulo {
  template <strong_type_like U>
  friend constexpr auto operator%=(Derived& a_lhs, const U& a_rhs) -> Derived&
    requires modulo_assignable_with<typename Derived::underlying_type,
                                    typename U::underlying_type>
  {
    a_lhs.get() %= a_rhs.get();
    return a_lhs;
  }

  template <strong_type_like U>
  friend constexpr auto operator%(const Derived& a_lhs, const U& a_rhs)
      -> Derived&
    requires modulo_with<typename Derived::underlying_type,
                         typename U::underlying_type>
  {
    return Derived(a_lhs.get() % a_rhs.get());
  }
};

template <class Derived> class increment {
  friend constexpr auto operator++(Derived& a_value) -> Derived& {
    ++a_value.get();
    return a_value;
  }

  friend constexpr auto operator++(Derived& a_value, int) -> Derived {
    Derived temp{a_value};
    ++a_value;
    return temp;
  }
};

template <class Derived> class decrement {
  friend constexpr auto operator--(Derived& a_value) -> Derived& {
    --a_value.get();
    return a_value;
  }

  friend constexpr auto operator--(Derived& a_value, int) -> Derived {
    Derived temp{a_value};
    --a_value;
    return temp;
  }
};
} // namespace ada_types

#endif