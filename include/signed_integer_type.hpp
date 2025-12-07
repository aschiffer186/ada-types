/// \file signed_integer_type.hpp
/// \brief Definition of strongly typed signed integer type.

#ifndef SIGNED_INTEGER_TYPE
#define SIGNED_INTEGER_TYPE

#include <concepts>
#include <cstdint>
#include <format>
#include <limits>

#include "checked_arithmetic_operations.hpp"
#include "exceptions.hpp"
#include "strong_type_base.hpp"

/// \namespace ada_types Namespace containing public API
namespace ada_types {
template <std::signed_integral T, string_literal Tag, std::intmax_t Min,
          std::intmax_t Max>
class signed_integer : public strong_type<T, Tag>,
                       checked_addition<signed_integer<T, Tag, Min, Max>>,
                       checked_subtraction<signed_integer<T, Tag, Min, Max>>,
                       checked_multiplication<signed_integer<T, Tag, Min, Max>>,
                       checked_division<signed_integer<T, Tag, Min, Max>>,
                       checked_modulo<signed_integer<T, Tag, Min, Max>>,
                       checked_increment<signed_integer<T, Tag, Min, Max>>,
                       checked_decrement<signed_integer<T, Tag, Min, Max>> {
  static_assert(Min < Max);
  using base_type = strong_type<T, Tag>;

  template <typename U> constexpr static bool dependent_false = false;

public:
  /// The minimum allowable value of the type
  constexpr static typename base_type::underlying_type min = Min;
  /// The maximum allowable value of the type
  constexpr static typename base_type::underlying_type max = Max;

  /// \brief Default constructor
  ///
  /// Initializes the underlying value to 0.
  /// If \f$Min \leq 0 \leq Max\f$ is \c false, the program is ill-formed.
  template <std::intmax_t MI = Min, std::intmax_t MA = Max>
  constexpr signed_integer() noexcept : base_type() {
    if constexpr (MI > 0) {
      static_assert(dependent_false<T>,
                    "Attempting to default construct signd integer where 0 is "
                    "not in allowable range");
    } else if constexpr (MA < 0) {
      static_assert(dependent_false<T>,
                    "Attempting to default construct signd integer where 0 is "
                    "not in allowable range");
    }
  }

  /// \brief Constructor
  ///
  /// Initializes \c *this with the specified value.
  /// This constructor is a constexpr constructor if \c a_value is in the range
  /// \f$[Min, Max]\f$.
  ///
  /// \tparam U The type of the value used to initialize \c *this
  /// \param a_value The value used to initialize \c *this.
  /// \throw input_out_of_range Throws \c input_out_of_range if the input value
  /// is outside the type allowable by the range.
  template <std::signed_integral U = std::remove_cv_t<T>>
  constexpr signed_integer(U a_value) : base_type(a_value) {
    if constexpr (Min > std::numeric_limits<U>::min()) {
      if (a_value < Min) {
        std::string message =
            std::format("Input below minimum allowable value, input is {}, "
                        "minimum allowable value is {}",
                        a_value, Min);
        throw input_out_of_range(message);
      }
    }
    if constexpr (Max < std::numeric_limits<U>::max()) {
      if (a_value > Max) {
        std::string message =
            std::format("Input above maximum allowable value, intput is "
                        "{}, maximum allowable value is {}",
                        a_value, Max);
        throw input_out_of_range(message);
      }
    }
  }

  template <std::signed_integral U, std::intmax_t Min2, std::intmax_t Max2>
  constexpr signed_integer(signed_integer<U, Tag, Min2, Max2> a_other)
      : base_type(a_other.get()) {
    if constexpr (Min2 < Min) {
      if (a_other.get() < Min) {
        std::string message =
            std::format("Input below minimum allowable value, input is {}, "
                        "minimum allowable value is {}",
                        a_other.get(), Min);
        throw input_out_of_range(message);
      }
    }
    if constexpr (Max2 > Max) {
      if (a_other.get() > Max) {
        std::string message =
            std::format("Input above maximum allowable value, intput is "
                        "{}, maximum allowable value is {}",
                        a_other.get(), Max);
        throw input_out_of_range(message);
      }
    }
  }

  template <typename E>
  constexpr explicit signed_integer(const arithmetic_expression<E>& a_expr)
      : base_type(a_expr.get()) {
    if (this->get() < Min) {
      std::string message =
          std::format("Input below minimum allowable value, input is {}, "
                      "minimum allowable value is {}",
                      this->get(), Min);
      throw input_out_of_range(message);
    } else if (this->get() > Max) {
      std::string message =
          std::format("Input above maximum allowable value, intput is "
                      "{}, maximum allowable value is {}",
                      this->get(), Max);
      throw input_out_of_range(message);
    }
  }

  template <std::signed_integral U, std::intmax_t Min2, std::intmax_t Max2>
  constexpr auto operator=(signed_integer<U, Tag, Min2, Max2> a_other)
      -> signed_integer& {
    signed_integer temp(a_other);
    std::swap(*this, temp);
    return *this;
  }

  template <typename E>
  constexpr auto operator=(const arithmetic_expression<E>& a_rhs)
      -> signed_integer& {
    signed_integer temp(a_rhs);
    std::swap(*this, temp);
    return *this;
  }
};
} // namespace ada_types

#endif