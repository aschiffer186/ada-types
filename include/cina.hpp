/// \file cina.hpp
/// \author Alex Schiffer
/// \brief Library for declaring strong types.

#ifndef CINA_HPP
#define CINA_HPP

#include <initializer_list>
#include <type_traits>
#include <utility>

namespace cina {
template <typename Tag, typename UnderlyingType> class strong_type;

namespace _detail {
template <typename Tag, typename UnderlyingType>
auto _as_strong_type(strong_type<Tag, UnderlyingType>)
    -> strong_type<Tag, UnderlyingType>;

template <typename T>
using _as_strong_type_t = decltype(_as_strong_type(std::declval<T>()));

template <typename T, typename Enable = void>
constexpr bool _is_strong_type_v = false;

template <typename T>
constexpr bool _is_strong_type_v<T, std::void_t<_as_strong_type_t<T>>> = true;
} // namespace _detail

template <typename T>
concept strong_type_like = _detail::_is_strong_type_v<T>;

namespace _detail {
template <typename Tag, typename UnderlyingType>
constexpr auto _as_underlying_type(strong_type<Tag, UnderlyingType>)
    -> UnderlyingType;
}

template <typename T>
using underlying_type_t =
    decltype(_detail::_as_underlying_type(std::declval<T>()));

struct equality_comparison {
  template <typename Derived> struct skill {
    friend constexpr auto operator==(const Derived& lhs, const Derived& rhs)
        -> bool {
      return lhs.unwrap() == rhs.unwrap();
    }
  };
};

struct uninitialized_t {
  constexpr explicit uninitialized_t() = default;
};
constexpr inline uninitialized_t uninitialized{};

template <typename Tag, typename UnderlyingType>
class strong_type : public equality_comparison::template skill<
                        strong_type<Tag, UnderlyingType>> {
  static_assert(!std::is_rvalue_reference_v<UnderlyingType>,
                "Underlying type must not be an rvalue reference");
  static_assert(!std::is_void_v<UnderlyingType>,
                "Underlying type must not be void");

public:
  constexpr strong_type()
    requires std::is_default_constructible_v<UnderlyingType>
      : _m_do_not_use_this() {}

  constexpr explicit strong_type(const uninitialized_t) {}

  template <class U>
    requires std::is_constructible_v<UnderlyingType, U> &&
             (!strong_type_like<std::remove_cvref_t<U>> &&
              !std::is_same_v<std::remove_cvref_t<U>, std::in_place_t>)
  constexpr explicit strong_type(U&& value)
      : _m_do_not_use_this(std::forward<U>(value)) {}

  template <typename... Args>
    requires std::is_constructible_v<UnderlyingType, Args...>
  constexpr explicit strong_type(std::in_place_t, Args&&... args)
      : _m_do_not_use_this(std::forward<Args>(args)...) {}

  template <typename U, typename... Args>
    requires std::is_constructible_v<UnderlyingType, std::initializer_list<U>&,
                                     Args...>
  constexpr explicit strong_type(std::in_place_t, std::initializer_list<U> il,
                                 Args&&... args)
      : _m_do_not_use_this(il, std::forward<Args>(args)...) {}

  template <typename U>
    requires std::is_constructible_v<UnderlyingType, const U&>
  constexpr explicit(!std::is_convertible_v<const U&, UnderlyingType>)
      strong_type(const strong_type<Tag, U>& other)
      : _m_do_not_use_this(other.unwrap()) {}

  template <typename U>
    requires std::is_constructible_v<UnderlyingType, U>
  constexpr explicit(!std::is_convertible_v<U, UnderlyingType>)
      strong_type(strong_type<Tag, U>&& other)
      : _m_do_not_use_this(std::move(other.unwrap())) {}

  template <class U>
    requires std::is_assignable_v<UnderlyingType&, const U&>
  constexpr auto operator=(const strong_type<Tag, U>& other)
      -> strong_type<Tag, UnderlyingType>&
    requires(!std::is_reference_v<UnderlyingType>)
  {
    _m_do_not_use_this = other.unwrap();
    return *this;
  }

  template <class U>
    requires std::is_assignable_v<UnderlyingType&, U>
  constexpr auto operator=(strong_type<Tag, U>&& other)
      -> strong_type<Tag, UnderlyingType>&
    requires(!std::is_reference_v<UnderlyingType>)
  {
    _m_do_not_use_this = std::move(other).unwrap();
    return *this;
  }

  template <typename Self>
  [[nodiscard]] constexpr auto&& unwrap(this Self&& self) noexcept
    requires(!std::is_reference_v<UnderlyingType>)
  {
    return std::forward<Self>(self)._m_do_not_use_this;
  }

  // Design goa: usable as NTTP
  UnderlyingType _m_do_not_use_this;
};

template <typename Tag, typename UnderlyingType>
class strong_type<Tag, UnderlyingType&>
    : public equality_comparison::template skill<
          strong_type<Tag, UnderlyingType&>> {
public:
  template <typename U>
    requires(std::is_constructible_v<UnderlyingType&, U> &&
             std::is_lvalue_reference_v<U> &&
             !strong_type_like<std::remove_cvref_t<U>>)
  constexpr strong_type(U&& value)
      : _m_do_not_use_this(&std::forward<U>(value)) {}

  constexpr strong_type(const strong_type& other) = default;
  constexpr strong_type(strong_type&& other) = default;

  template <typename U>
    requires std::is_constructible_v<
                 std::add_lvalue_reference_t<UnderlyingType>, U> &&
             std::is_lvalue_reference_v<U>
  constexpr explicit strong_type(const strong_type<Tag, U>& other)
      : _m_do_not_use_this(&other.unwrap()) {}

  constexpr auto operator=(const strong_type& other) -> strong_type& {
    *_m_do_not_use_this = other.unwrap();
    return *this;
  }

  constexpr auto operator=(strong_type&& other) -> strong_type& {
    *_m_do_not_use_this = std::move(other.unwrap());
    return *this;
  }

  [[nodiscard]] constexpr auto unwrap() const volatile noexcept
      -> UnderlyingType& {
    return *_m_do_not_use_this;
  }

private:
  UnderlyingType* _m_do_not_use_this{};
};

namespace _detail {
template <typename Tag, typename T, typename... Skills> struct _new_type_impl {
  struct impl : public strong_type<Tag, T>,
                public Skills::template skill<impl>... {
    using strong_type<Tag, T>::strong_type;
  };

  using type = impl;
};
} // namespace _detail

template <typename Tag, typename T, typename... Args>
using new_type = _detail::_new_type_impl<Tag, T, Args...>::type;

} // namespace cina

#endif