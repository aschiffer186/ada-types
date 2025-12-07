#ifndef CHECKED_ARITHMETIC_HPP
#define CHECKED_ARITHMETIC_HPP

#include <cassert>
#include <format>

#include "concepts.hpp"
#include "exceptions.hpp"
namespace ada_types {
template <typename Expression> class arithmetic_expression {
public:
  [[nodiscard]] constexpr auto get() const noexcept {
    return static_cast<const Expression&>(*this).get();
  }
};

template <typename LHS, typename RHS>
class addition_expression
    : public arithmetic_expression<addition_expression<LHS, RHS>> {
public:
  addition_expression(const LHS& a_lhs, const RHS& a_rhs) noexcept
      : m_lhs(a_lhs), m_rhs(a_rhs) {}

  [[nodiscard]] constexpr std::intmax_t get() const noexcept {
    return static_cast<std::intmax_t>(m_lhs.get()) +
           static_cast<std::intmax_t>(m_rhs.get());
  }

private:
  const LHS& m_lhs;
  const RHS& m_rhs;
};

template <typename E1, typename E2>
constexpr auto operator+(const arithmetic_expression<E1>& a_lhs,
                         const arithmetic_expression<E2>& a_rhs) noexcept {
  return addition_expression<arithmetic_expression<E1>,
                             arithmetic_expression<E2>>(a_lhs, a_rhs);
}

template <typename LHS, typename RHS>
class subtraction_expression
    : public arithmetic_expression<subtraction_expression<LHS, RHS>> {
public:
  subtraction_expression(const LHS& a_lhs, const RHS& a_rhs) noexcept
      : m_lhs(a_lhs), m_rhs(a_rhs) {}

  [[nodiscard]] constexpr std::intmax_t get() const noexcept {
    return static_cast<std::intmax_t>(m_lhs.get()) -
           static_cast<std::intmax_t>(m_rhs.get());
  }

private:
  const LHS& m_lhs;
  const RHS& m_rhs;
};

template <typename E1, typename E2>
constexpr auto operator-(const arithmetic_expression<E1>& a_lhs,
                         const arithmetic_expression<E2>& a_rhs) noexcept {
  return subtraction_expression<arithmetic_expression<E1>,
                                arithmetic_expression<E2>>(a_lhs, a_rhs);
}

template <typename LHS, typename RHS>
class multiplication_expression
    : public arithmetic_expression<multiplication_expression<LHS, RHS>> {
public:
  multiplication_expression(const LHS& a_lhs, const RHS& a_rhs) noexcept
      : m_lhs(a_lhs), m_rhs(a_rhs) {}

  [[nodiscard]] constexpr std::intmax_t get() const noexcept {
    return static_cast<std::intmax_t>(m_lhs.get()) *
           static_cast<std::intmax_t>(m_rhs.get());
  }

private:
  const LHS& m_lhs;
  const RHS& m_rhs;
};

template <typename E1, typename E2>
constexpr auto operator*(const arithmetic_expression<E1>& a_lhs,
                         const arithmetic_expression<E2>& a_rhs) noexcept {
  return multiplication_expression<arithmetic_expression<E1>,
                                   arithmetic_expression<E2>>(a_lhs, a_rhs);
}

template <typename LHS, typename RHS>
class division_expression
    : public arithmetic_expression<division_expression<LHS, RHS>> {
public:
  division_expression(const LHS& a_lhs, const RHS& a_rhs) noexcept
      : m_lhs(a_lhs), m_rhs(a_rhs) {}

  [[nodiscard]] constexpr std::intmax_t get() const noexcept {
    assert(m_rhs.get() != 0 && "unexpected division by 0");
    return static_cast<std::intmax_t>(m_lhs.get()) /
           static_cast<std::intmax_t>(m_rhs.get());
  }

private:
  const LHS& m_lhs;
  const RHS& m_rhs;
};

template <typename E1, typename E2>
constexpr auto operator/(const arithmetic_expression<E1>& a_lhs,
                         const arithmetic_expression<E2>& a_rhs) {
  if (a_rhs.get() == 0) [[unlikely]] {
    throw domain_error("Division by 0");
  }
  return division_expression<arithmetic_expression<E1>,
                             arithmetic_expression<E2>>(a_lhs, a_rhs);
}

template <signed_integer_like Derived> class checked_addition {
  template <typename E>
  friend constexpr auto operator+=(Derived& a_lhs,
                                   const arithmetic_expression<E>& a_rhs)
      -> Derived& {
    if (a_lhs.get() < Derived::min - a_rhs.get()) [[unlikely]] {
      std::string message =
          std::format("Invalid value encountered during addition assigment. "
                      "Minimum allowable value is {}, actual value is {}",
                      Derived::min, a_rhs.get());
      throw input_out_of_range{message};
    }
    if (a_lhs.get() > Derived::max - a_rhs.get()) [[unlikely]] {
      std::string message =
          std::format("Invalid value encountered during addition assigment. "
                      "Maximum allowable value is {}, actual value is {}",
                      Derived::max, a_rhs.get());
      throw input_out_of_range{message};
    }
    a_lhs.get() += a_rhs.get();
    return a_lhs;
  }

  template <signed_integer_like U>
  friend constexpr auto operator+=(Derived& a_lhs, U a_rhs) -> Derived& {
    if (a_lhs.get() < Derived::min - a_rhs.get()) [[unlikely]] {
      std::string message =
          std::format("Invalid value encountered during addition assigment. "
                      "Minimum allowable value is {}, actual value is {}",
                      Derived::min, a_rhs.get());
      throw input_out_of_range{message};
    }
    if (a_lhs.get() > Derived::max - a_rhs.get()) [[unlikely]] {
      std::string message =
          std::format("Invalid value encountered during addition assigment. "
                      "Maximum allowable value is {}, actual value is {}",
                      Derived::max, a_rhs.get());
      throw input_out_of_range{message};
    }
    a_lhs.get() += a_rhs.get();
    return a_lhs;
  }

  template <typename E>
  friend constexpr auto
  operator+(Derived a_lhs, const arithmetic_expression<E>& a_rhs) noexcept {
    return addition_expression<Derived, arithmetic_expression<E>>(a_lhs, a_rhs);
  }

  template <typename E>
  friend constexpr auto operator+(const arithmetic_expression<E>& a_lhs,
                                  Derived a_rhs) noexcept {
    return addition_expression<arithmetic_expression<E>, Derived>(a_lhs, a_rhs);
  }

  template <signed_integer_like U>
  friend constexpr auto operator+(Derived a_lhs, U a_rhs) noexcept {
    return addition_expression<Derived, U>(a_lhs, a_rhs);
  }
};

template <signed_integer_like Derived> class checked_subtraction {
  template <typename E>
  friend constexpr auto operator-=(Derived& a_lhs,
                                   const arithmetic_expression<E>& a_rhs)
      -> Derived& {
    if (a_lhs.get() > Derived::max + a_rhs.get()) [[unlikely]] {
      std::string message =
          std::format("Invalid value encountered during subtraction assigment. "
                      "Maximum allowable value is {}, actual value is {}",
                      Derived::max, a_rhs.get());
      throw input_out_of_range{message};
    }
    if (a_lhs.get() < Derived::min + a_rhs.get()) [[unlikely]] {
      std::string message =
          std::format("Invalid value encountered during subtraction assigment. "
                      "Minimum allowable value is {}, actual value is {}",
                      Derived::min, a_rhs.get());
      throw input_out_of_range{message};
    }
    a_lhs.get() -= a_rhs.get();
    return a_lhs;
  }

  template <typename E>
  friend constexpr auto
  operator-(Derived a_lhs, const arithmetic_expression<E>& a_rhs) noexcept {
    return subtraction_expression<Derived, arithmetic_expression<E>>(a_lhs,
                                                                     a_rhs);
  }

  template <typename E>
  friend constexpr auto operator-(const arithmetic_expression<E>& a_lhs,
                                  Derived a_rhs) noexcept {
    return subtraction_expression<arithmetic_expression<E>, Derived>(a_lhs,
                                                                     a_rhs);
  }

  template <signed_integer_like U>
  friend constexpr auto operator-(Derived a_lhs, U a_rhs) noexcept {
    return subtraction_expression<Derived, U>(a_lhs, a_rhs);
  }
};

template <signed_integer_like Derived> class checked_multiplication {
  template <typename E>
  friend constexpr auto
  operator*(Derived a_lhs, const arithmetic_expression<E>& a_rhs) noexcept {
    return multiplication_expression<Derived, arithmetic_expression<E>>(a_lhs,
                                                                        a_rhs);
  }

  template <signed_integer_like U>
  friend constexpr auto operator*(Derived a_lhs, U a_rhs) noexcept {
    return multiplication_expression<Derived, U>(a_lhs, a_rhs);
  }
};

template <signed_integer_like Derived> class checked_division {
  template <typename E>
  friend constexpr auto operator/=(Derived& a_lhs,
                                   const arithmetic_expression<E>& a_rhs)
      -> Derived& {
    if (a_rhs.get() == 0) [[unlikely]] {
      throw domain_error("Division by 0");
    }
    a_lhs.get() /= a_rhs.get();
    if (a_lhs.get() < Derived::min) [[unlikely]] {
      std::string message =
          std::format("Invalid value encountered during division assigment. "
                      "Minimum allowable value is {}, actual value is {}",
                      Derived::min, a_lhs.get());
      throw input_out_of_range{message};
    }
    if (a_lhs.get() > Derived::max) [[unlikely]] {
      std::string message =
          std::format("Invalid value encountered during division assigment. "
                      "Maximum allowable value is {}, actual value is {}",
                      Derived::max, a_lhs.get());
      throw input_out_of_range{message};
    }
    return a_lhs;
  }

  template <signed_integer_like U>
  friend constexpr auto operator/=(Derived& a_lhs, U a_rhs) -> Derived& {
    a_lhs.get() /= a_rhs.get();
    if (a_rhs.get() == 0) [[unlikely]] {
      throw domain_error("Division by 0");
    }
    if (a_lhs.get() < Derived::min) [[unlikely]] {
      std::string message =
          std::format("Invalid value encountered during division assigment. "
                      "Minimum allowable value is {}, actual value is {}",
                      Derived::min, a_lhs.get());
      throw input_out_of_range{message};
    }
    if (a_lhs.get() > Derived::max) [[unlikely]] {
      std::string message =
          std::format("Invalid value encountered during division assigment. "
                      "Maximum allowable value is {}, actual value is {}",
                      Derived::max, a_lhs.get());
      throw input_out_of_range{message};
    }
    return a_lhs;
  }

  template <typename E>
  friend constexpr auto operator/(Derived a_lhs,
                                  const arithmetic_expression<E>& a_rhs) {
    if (a_rhs.get() == 0) [[unlikely]] {
      throw domain_error("Division by 0");
    }
    return division_expression<Derived, arithmetic_expression<E>>(a_lhs, a_rhs);
  }

  template <typename E>
  friend constexpr auto operator/(const arithmetic_expression<E> a_lhs,
                                  const Derived& a_rhs) {
    if (a_rhs.get() == 0) [[unlikely]] {
      throw domain_error("Division by 0");
    }
    return division_expression<arithmetic_expression<E>, Derived>(a_lhs, a_rhs);
  }

  template <signed_integer_like U>
  friend constexpr auto operator/(Derived a_lhs, U a_rhs) {
    if (a_rhs.get() == 0) [[unlikely]] {
      throw domain_error("Division by 0");
    }
    return division_expression<Derived, U>(a_lhs, a_rhs);
  }
};

template <signed_integer_like Derived> class checked_modulo {};

template <signed_integer_like Derived> class checked_increment {
  friend constexpr auto operator++(Derived& a_value) -> Derived& {
    if (a_value.get() == Derived::max) [[unlikely]] {
      throw input_out_of_range(
          "Attempting to increment signed integer at maximum value");
    }
    ++a_value.get();
    return a_value;
  }

  friend constexpr auto operator++(Derived& a_value, int) -> Derived {
    Derived temp{a_value};
    ++a_value;
    return temp;
  }
};

template <signed_integer_like Derived> class checked_decrement {
  friend constexpr auto operator--(Derived& a_value) -> Derived& {
    if (a_value.get() == Derived::min) [[unlikely]] {
      throw input_out_of_range(
          "Attempting to decrement signed integer at minimum value");
    }
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