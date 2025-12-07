/// \file strong_type_base.hpp
/// \brief Definition of base class of strong type hierarchy

#ifndef STRONG_TYPE_BASE_HPP
#define STRONG_TYPE_BASE_HPP

#include <concepts>
#include <initializer_list>
#include <type_traits>
#include <utility>

#include "comparison.hpp"
#include "streaming.hpp"

#include "ada_types_fwd.hpp"

/// \namespace ada_types Namespace containing public API
namespace ada_types {

template <typename T, string_literal Tag>
class strong_type : comparison_operators<strong_type<T, Tag>>,
                    stream_operator<strong_type<T, Tag>> {
  template <auto> constexpr static bool dependent_false = false;

public:
  /// The type tag
  constexpr static auto tag = Tag;
  /// The underlying type of the strongly type value
  using underlying_type = T;

  /// \brief Default constructor
  ///
  /// Default initializes the underlying value of the strongly typed value.
  /// This constructor only participates in overload resolution if
  /// \c std::is_default_constructible_v<T> is \c true.
  constexpr strong_type()
    requires std::is_default_constructible_v<T>
  = default;

  template <string_literal Tag2>
    requires(Tag2 != Tag)
  constexpr strong_type(const strong_type<T, Tag2>&) {
    static_assert(dependent_false<Tag2>,
                  "Attempting to construct strong type from incompatible type");
  }

  /// \brief Constructor
  ///
  /// Direct non-list initializes the underlying value of the strongly type
  /// valued with the specified value as if by \c std::forward<U>(a_value).
  /// This constructor is a constexpr constructor if the selected constructor of
  /// \c T is a constexpr constructor.
  ///
  /// This constructor only participates in overload resolution if all of the
  /// following are true:
  ///
  /// 1. <tt>std::constructible_from<T, U></tt>
  /// 2.`
  ///
  /// \tparam U The type of the value used to initialize \c *this.
  /// \param a_value The value used to initialize \c *this.
  template <typename U = std::remove_cv_t<T>>
    requires std::constructible_from<T, U> &&
             (!std::same_as<std::remove_cvref_t<U>, strong_type>) &&
             (!std::same_as<std::remove_cvref_t<U>, std::in_place_t>)
  constexpr explicit strong_type(U&& a_value) noexcept(
      std::is_nothrow_constructible_v<T, U&&>)
      : m_value(std::forward<U>(a_value)) {}

  template <typename... Args>
    requires std::constructible_from<T, Args...>
  constexpr strong_type(std::in_place_t, Args&&... a_args) noexcept(
      std::is_nothrow_constructible_v<T, Args&&...>)
      : m_value(std::forward<Args>(a_args)...) {}

  template <typename U, typename... Args>
    requires std::constructible_from<T, std::initializer_list<U>&, Args...>
  constexpr strong_type(
      std::in_place_t, std::initializer_list<U> a_il,
      Args&&... a_args) noexcept(std::
                                     is_nothrow_constructible_v<
                                         T, std::initializer_list<U>&,
                                         Args&&...>)
      : m_value(a_il, std::forward<Args>(a_args)...) {}

  template <typename Self>
  [[nodiscard]] constexpr auto&& get(this Self&& a_self) noexcept {
    return std::forward<Self>(a_self).m_value;
  }

private:
  T m_value;
};
} // namespace ada_types

#endif