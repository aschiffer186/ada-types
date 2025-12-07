#ifndef ARITHMETIC_TYPE_HPP
#define ARITHMETIC_TYPE_HPP

#include "arithmetic_operations.hpp"
#include "strong_type_base.hpp"
#include <initializer_list>
#include <type_traits>

namespace ada_types {
template <typename T, string_literal Tag>
class arithmetic_type : public strong_type<T, Tag>,
                        addition<arithmetic_type<T, Tag>>,
                        subtraction<arithmetic_type<T, Tag>>,
                        multiplication<arithmetic_type<T, Tag>>,
                        division<arithmetic_type<T, Tag>>,
                        modulo<arithmetic_type<T, Tag>> {
  using base_type = strong_type<T, Tag>;

public:
  constexpr arithmetic_type()
    requires std::is_default_constructible_v<
                 typename base_type::underlying_type>
  = default;

  template <typename U = std::remove_cvref_t<T>>
    requires std::constructible_from<T, U> &&
             (!std::same_as<U, std::in_place_t>) &&
             (!std::same_as<U, arithmetic_type>)
  constexpr explicit arithmetic_type(U&& a_value) noexcept(
      std::is_nothrow_constructible_v<T, U&&>)
      : base_type(std::forward<U>(a_value)) {}

  template <typename... Args>
    requires std::constructible_from<T, Args...>
  constexpr arithmetic_type(std::in_place_t, Args&&... a_args) noexcept(
      std::is_nothrow_constructible_v<T, Args&&...>)
      : base_type(std::forward<Args>(a_args)...) {}

  template <typename U, typename... Args>
    requires std::constructible_from<T, std::initializer_list<U>&, Args...>
  constexpr arithmetic_type(
      std::in_place_t, std::initializer_list<U> a_il,
      Args&&... a_args) noexcept(std::
                                     is_nothrow_constructible_v<
                                         T, std::initializer_list<U>&,
                                         Args&&...>)
      : base_type(a_il, std::forward<Args>(a_args)...) {}
};
} // namespace ada_types

#endif