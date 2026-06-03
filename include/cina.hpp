/// \file cina.hpp
/// \author Alex Schiffer
/// \brief Library for declaring strong types.

#ifndef CINA_HPP
#define CINA_HPP

#include <concepts>         // same_as
#include <format>           // formatter
#include <functional>       // hash
#include <initializer_list> // initializer_list
#include <istream>          // basic_istream
#include <iterator>         // back_inserter
#include <ostream>          // basic_ostream
#include <string>           // string
#include <string_view>      // string_view
#include <type_traits> // is_same, is_constructible, is_reference, is_assignable, remove_cvref, void_t
#include <utility> // declval, forward

#ifdef _MSVC_VER
#define CINA_EBCO __declspec(empty_bases)
#else
#define CINA_EBCO
#endif

namespace cina {

///////////////////
// --- C++ Concepts
///////////////////

template <typename T>
concept cxx_boolean = std::same_as<std::remove_cvref_t<T>, bool>;

////////////////////////////////
// --- Forward Declarations ---
///////////////////////////////

template <typename Tag, typename UnderlyingType> class strong_type;

template <typename Tag, cxx_boolean UnderlyingType> class boolean_type;

////////////////////////
// --- Cina Concepts ---
////////////////////////

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

//////////////////
// --- Skills ----
//////////////////

struct equality_comparison {
  template <typename Derived> struct skill {
    friend constexpr auto operator==(const Derived& lhs, const Derived& rhs)
        -> bool {
      return lhs.unwrap() == rhs.unwrap();
    }
  };
};

struct output_stream {
  template <typename Derived> struct skill {
    template <typename CharT, typename Traits>
    friend auto operator<<(std::basic_ostream<CharT, Traits>& os,
                           const Derived& value)
        -> std::basic_ostream<CharT, Traits>& {
      return os << value.unwrap();
    }
  };
};

struct input_stream {
  template <typename Derived> struct skill {
    template <typename CharT, typename Traits>
    friend auto operator>>(std::basic_istream<CharT, Traits>& is,
                           Derived& value)
        -> std::basic_istream<CharT, Traits>& {
      return is >> value.unwrap();
    }
  };
};

//////////////////////
// --- Unitialized ---
//////////////////////

struct uninitialized_t {
  constexpr explicit uninitialized_t() = default;
};
constexpr inline uninitialized_t uninitialized{};

//////////////////////////////////////
// --- Strong Type Implementation ---
//////////////////////////////////////

template <typename Tag, typename UnderlyingType>
class CINA_EBCO strong_type : public equality_comparison::template skill<
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

  constexpr auto
  swap(strong_type& other) noexcept(std::is_nothrow_swappable_v<UnderlyingType>)
      -> void
    requires std::is_swappable_v<UnderlyingType>
  {
    using std::swap;
    swap(_m_do_not_use_this, other._m_do_not_use_this);
  }

  // Design goa: usable as NTTP
  UnderlyingType _m_do_not_use_this;

private:
  friend auto swap(strong_type& lhs, strong_type& rhs) noexcept(
      std::is_nothrow_swappable_v<UnderlyingType>) -> void
    requires std::is_swappable_v<UnderlyingType>
  {
    lhs.swap(rhs);
  }
};

template <typename Tag, typename UnderlyingType>
class CINA_EBCO strong_type<Tag, UnderlyingType&>
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

  constexpr auto
  swap(strong_type& other) noexcept(std::is_nothrow_swappable_v<UnderlyingType>)
      -> void
    requires std::is_swappable_v<UnderlyingType>
  {
    using std::swap;
    swap(*_m_do_not_use_this, *other._m_do_not_use_this);
  }

protected:
  UnderlyingType* _m_do_not_use_this{};

private:
  friend auto swap(strong_type& lhs, strong_type& rhs) noexcept(
      std::is_nothrow_swappable_v<UnderlyingType>) -> void
    requires std::is_swappable_v<UnderlyingType>
  {
    lhs.swap(rhs);
  }
};

///////////////////////
// --- Boolean Type ---
///////////////////////

template <typename Tag, cxx_boolean UnderlyingType>
class CINA_EBCO boolean_type
    : public strong_type<Tag, UnderlyingType>,
      public equality_comparison::template skill<
          boolean_type<Tag, UnderlyingType>>,
      public output_stream::template skill<boolean_type<Tag, UnderlyingType>>,
      public input_stream::template skill<boolean_type<Tag, UnderlyingType>> {

  using base_type = strong_type<Tag, UnderlyingType>;

  class bool_literal {
    template <int> constexpr static bool dependent_false = false;

  public:
    consteval bool_literal(const bool&& value) : _m_value(value) {}

    template <int i = 0> constexpr bool_literal(const bool&) {
      static_assert(
          dependent_false<i>,
          "Only bool literals can be assigned to a cina::boolean_type");
    }

    constexpr operator bool() const noexcept { return _m_value; }

  private:
    bool _m_value;
  };

public:
  constexpr explicit boolean_type(const uninitialized_t)
      : base_type(uninitialized) {}

  constexpr explicit boolean_type(const UnderlyingType value)
    requires(!std::is_reference_v<UnderlyingType>)
      : base_type(value) {}

  template <typename U>
    requires std::is_constructible_v<UnderlyingType, U> &&
             std::is_lvalue_reference_v<U>
             constexpr explicit boolean_type(U&& value)
               requires std::is_reference_v<UnderlyingType>
      : base_type(std::forward<U>(value)) {}

  constexpr auto operator=(const bool_literal value) -> boolean_type&
    requires(!std::is_reference_v<UnderlyingType>)
  {
    this->unwrap() = value;
    return *this;
  }

  constexpr auto operator=(const bool_literal value) -> boolean_type&
    requires std::is_reference_v<UnderlyingType>
  {
    *this->_m_do_not_use_this = value;
    return *this;
  }

  constexpr explicit operator bool() const noexcept { return this->unwrap(); }
};

////////////////////////
// --- Type Factory ---
////////////////////////

struct no_skills;

namespace _detail {

template <typename Tag, typename T, typename... Args> struct _new_type_impl {
  struct impl : public strong_type<Tag, T>,
                public Args::template skill<impl>... {
    using strong_type<Tag, T>::strong_type;
  };

  using type = impl;
};

template <typename Tag, typename T> struct _new_type_impl<Tag, T, no_skills> {
  using type = strong_type<Tag, T>;
};

template <typename Tag, cxx_boolean T> struct _new_type_impl<Tag, T> {
  using type = boolean_type<Tag, T>;
};
} // namespace _detail

template <typename Tag, typename T, typename... Args>
using new_type = _detail::_new_type_impl<Tag, T, Args...>::type;

} // namespace cina

//////////////////////////////////////////
// --- Standard Libary Specializations ---
//////////////////////////////////////////

template <cina::strong_type_like T> struct std::hash<T> {
  auto operator()(const T& value) const -> std::size_t {
    return std::hash<cina::underlying_type_t<T>>{}(value.unwrap());
  }
};

template <cina::strong_type_like T>
  requires std::formattable<cina::underlying_type_t<T>, char>
struct std::formatter<T> : std::formatter<std::string_view> {
  auto format(const T& value, format_context& ctx) const {
    std::string out;
    std::format_to(std::back_inserter(out), "{}", value.unwrap());
    return std::formatter<std::string_view>::format(out, ctx);
  }
};

#endif