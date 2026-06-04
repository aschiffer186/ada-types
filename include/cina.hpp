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

/// \namespace cina
/// \brief The \c cina namespace contains all the entities of the library.
namespace cina {

///////////////////
// --- C++ Concepts
///////////////////

/// \brief Concept indicating a type models \c bool.
///
/// Concept indicating that a type models <i>cv</i>-<tt>bool</tt> or a reference
/// to <i>cv</i>-<tt>bool</tt>.
///
/// \tparam T The type to test.
template <typename T>
concept cxx_boolean = std::same_as<std::remove_cvref_t<T>, bool>;

/// \brief Concept indicating a types an integer for use in arithmetic
/// expressions.
///
/// Concept indicating that a type is an integer or reference to an integer
/// type, potentially <i>cv</i>-qualified, for use in arithmetic expressions.
/// Specifically excludes character types.
///
/// \tparam T The type to test.
template <typename T>
concept cxx_mathematical_signed_integer =
    std::signed_integral<std::remove_cvref_t<T>> &&
    !std::same_as<std::remove_cvref_t<T>, char> &&
    !std::same_as<std::remove_cvref_t<T>, wchar_t>;

template <typename From, typename To>
concept cxx_non_narrowing_integer_conversion =
    std::integral<From> && std::integral<To> &&
    std::is_signed_v<From> == std::is_signed_v<To> &&
    sizeof(From) <= sizeof(To);

////////////////////////////////
// --- Forward Declarations ---
///////////////////////////////

/// \brief Strong typedef.
///
/// Clas template \c strong_type can be used to create strong typedefs. Unlike
/// builtin \c typedef or \c using statements, strong typedefs created using \c
/// strong_type represent distinc types. They are not interchangable with each
/// other. The \c Tag template parameter is used to create distinct types.
/// Class template \c strong_type is meant to be used as a fallback when more
/// specialized types are not suitable. For example, for boolean types, class
/// template \c boolean_type should be preferred.
///
///
/// \tparam Tag A unique type used to create a distinct strong type. It is
/// \tparam UnderlyingType The underlying type that the strong type wraps. It
/// must not be an rvalue reference or void.
template <typename Tag, typename UnderlyingType> class strong_type;

/// \brief Strongly-typed boolean.
///
/// Class template \c boolean_type can be used to create strongly-typed
/// booleans. Unlike \c bool, this type does not support arithmetic operations
/// and is not convertible to other integer types.
///
/// \c Tag is a unique type used to create a distinct boolean type.
/// \c UnderlyingType is the underlying boolean type. If this type is a
/// reference, the boolean type is implemented using "assign through" semantics.
/// This means that assigning a new value to the type updates the value the \c
/// boolean_type instance refers to.
template <typename Tag, cxx_boolean UnderlyingType> class boolean_type;

template <typename Tag, cxx_mathematical_signed_integer>
class signed_integer_type;

////////////////////////
// --- Cina Concepts ---
////////////////////////

/// \cond
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
/// \endcond

/// \brief Concept indicating a type can be converted to a strong type.
///
/// \tparam T The type to test.
template <typename T>
concept strong_type_like = _detail::_is_strong_type_v<T>;

/// \cond
namespace _detail {
template <typename Tag, typename UnderlyingType>
constexpr auto _as_underlying_type(strong_type<Tag, UnderlyingType>)
    -> UnderlyingType;
}
/// \endcond

/// \brief Alias template to get the underlying type of a strong type.
///
/// \tparam T The strong type to get the underlying type of.
template <typename T>
using underlying_type_t =
    decltype(_detail::_as_underlying_type(std::declval<T>()));

namespace _detail {
template <strong_type_like T> struct remove_reference {
  using type = typename T::template rebind<
      std::remove_reference_t<underlying_type_t<T>>>;
};
} // namespace _detail

template <strong_type_like T>
using remove_reference_t = typename _detail::remove_reference<T>::type;

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

struct three_way_comparison {
  template <typename Derived> struct skill {
    friend constexpr auto operator<=>(const Derived& lhs, const Derived& rhs) {
      return lhs.unwrap() <=> rhs.unwrap();
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

struct addition {
  template <typename Derived> struct skill {
    friend auto operator+(const Derived& lhs, const Derived& rhs) {
      using return_underlying_type = decltype(lhs.unwrap() + rhs.unwrap());
      return typename Derived::template rebind<return_underlying_type>{
          lhs.unwrap() + rhs.unwrap()};
    }
  };
};

struct subtration {
  template <typename Derived> struct skill {
    friend auto operator-(const Derived& lhs, const Derived& rhs) {
      using return_underlying_type = decltype(lhs.unwrap() - rhs.unwrap());
      return typename Derived::template rebind<return_underlying_type>{
          lhs.unwrap() - rhs.unwrap()};
    }
  };
};

struct multiplication {
  template <typename Derived> struct skill {
    friend auto operator*(const Derived& lhs, const Derived& rhs) {
      using return_underlying_type = decltype(lhs.unwrap() * rhs.unwrap());
      return typename Derived::template rebind<return_underlying_type>{
          lhs.unwrap() * rhs.unwrap()};
    }
  };
};

struct division {
  template <typename Derived> struct skill {
    friend auto operator/(const Derived& lhs, const Derived& rhs) {
      using return_underlying_type = decltype(lhs.unwrap() / rhs.unwrap());
      return typename Derived::template rebind<return_underlying_type>{
          lhs.unwrap() / rhs.unwrap()};
    }
  };
};

struct modulo {
  template <typename Derived> struct skill {
    friend auto operator%(const Derived& lhs, const Derived& rhs) {
      using return_underlying_type = decltype(lhs.unwrap() % rhs.unwrap());
      return typename Derived::template rebind<return_underlying_type>{
          lhs.unwrap() % rhs.unwrap()};
    }
  };
};

//////////////////////
// --- Unitialized ---
//////////////////////

/// \brief Tag type to indicating a strong type should be constructed
/// unitialized.
struct uninitialized_t {
  constexpr explicit uninitialized_t() = default;
};
/// \brief Tag value indicating a strong type should be constructed unitialized.
constexpr inline uninitialized_t uninitialized{};

//////////////////////////////////////
// --- Strong Type Implementation ---
//////////////////////////////////////

template <typename Tag, typename UnderlyingType>
class CINA_EBCO strong_type
    : public equality_comparison::skill<strong_type<Tag, UnderlyingType>> {
  static_assert(!std::is_rvalue_reference_v<UnderlyingType>,
                "Underlying type must not be an rvalue reference");
  static_assert(!std::is_void_v<UnderlyingType>,
                "Underlying type must not be void");

public:
  /// \brief Alias template to rebind the strong type to a different underlying
  /// type with the same tag.
  template <typename U> using rebind = strong_type<Tag, U>;

  constexpr strong_type()
    requires std::is_default_constructible_v<UnderlyingType>
      : _m_do_not_use_this{} {}

  explicit strong_type(const uninitialized_t)
    requires std::is_default_constructible_v<UnderlyingType>
  {}

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

/// \brief Specialization of \c strong_type for lvalue reference types.
///
/// Specialization for lvalue-reference types. This specialization is
/// implemented using "assign through" semantics. This means that assigning a
/// new value to the type updates the value the \c strong_type instance refers
/// to.
///
/// \tparam Tag A unique type used to create a distinct strong type.
/// \tparam UnderlyingType The underlying type that the strong type wraps.
template <typename Tag, typename UnderlyingType>
class CINA_EBCO strong_type<Tag, UnderlyingType&>
    : public equality_comparison::skill<strong_type<Tag, UnderlyingType&>> {
public:
  template <typename U> using rebind = strong_type<Tag, U>;

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

  template <typename U>
    requires std::is_assignable_v<UnderlyingType&, const U&>
  constexpr auto operator=(const strong_type<Tag, U>& other) -> strong_type& {
    *_m_do_not_use_this = other.unwrap();
    return *this;
  }

  template <typename U>
    requires std::is_assignable_v<UnderlyingType&, U>
  constexpr auto operator=(strong_type<Tag, U>&& other) -> strong_type& {
    *_m_do_not_use_this = std::move(other).unwrap();
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
      public equality_comparison::skill<boolean_type<Tag, UnderlyingType>>,
      public output_stream::skill<boolean_type<Tag, UnderlyingType>>,
      public input_stream::skill<boolean_type<Tag, UnderlyingType>> {

  using base_type = strong_type<Tag, UnderlyingType>;

public:
  template <typename U> using rebind = boolean_type<Tag, U>;

  explicit boolean_type(const uninitialized_t) : base_type(uninitialized) {}

  constexpr explicit boolean_type(const UnderlyingType value)
    requires(!std::is_reference_v<UnderlyingType>)
      : base_type(value) {}

  template <typename U>
    requires std::is_constructible_v<UnderlyingType, U> &&
             std::is_lvalue_reference_v<U>
             constexpr explicit boolean_type(U&& value)
               requires std::is_reference_v<UnderlyingType>
      : base_type(std::forward<U>(value)) {}

  template <typename U>
  constexpr auto operator=(const boolean_type<Tag, U> other) -> boolean_type&
    requires(!std::is_reference_v<UnderlyingType>)
  {
    this->unwrap() = other.unwrap();
    return *this;
  }

  template <typename U>
  constexpr auto operator=(const boolean_type<Tag, U> other) -> boolean_type&
    requires std::is_reference_v<UnderlyingType>
  {
    *this->_m_do_not_use_this = other.unwrap();
    return *this;
  }

  constexpr explicit operator bool() const noexcept { return this->unwrap(); }
};

//////////////////////////////
// --- Signed Integer Type ---
//////////////////////////////

template <typename Tag, cxx_mathematical_signed_integer UnderlyingType>
class CINA_EBCO signed_integer_type
    : public strong_type<Tag, UnderlyingType>,
      public equality_comparison::skill<
          signed_integer_type<Tag, UnderlyingType>>,
      public three_way_comparison::skill<
          signed_integer_type<Tag, UnderlyingType>>,
      public output_stream::skill<signed_integer_type<Tag, UnderlyingType>>,
      public input_stream::skill<signed_integer_type<Tag, UnderlyingType>>,
      public addition::skill<signed_integer_type<Tag, UnderlyingType>>,
      public subtration::skill<signed_integer_type<Tag, UnderlyingType>>,
      public multiplication::skill<signed_integer_type<Tag, UnderlyingType>>,
      public division::skill<signed_integer_type<Tag, UnderlyingType>>,
      public modulo::skill<signed_integer_type<Tag, UnderlyingType>> {
  using base_type = strong_type<Tag, UnderlyingType>;

public:
  template <typename U> using rebind = signed_integer_type<Tag, U>;

  explicit signed_integer_type(const uninitialized_t)
      : base_type(uninitialized) {}

  template <typename U>
    requires cxx_non_narrowing_integer_conversion<U, UnderlyingType>
  constexpr explicit signed_integer_type(const U value)
    requires(!std::is_reference_v<UnderlyingType>)
      : base_type(static_cast<UnderlyingType>(value)) {}

  template <typename U>
    requires std::is_constructible_v<UnderlyingType, U> &&
             std::is_lvalue_reference_v<U>
             constexpr explicit signed_integer_type(U&& value)
               requires std::is_reference_v<UnderlyingType>
      : base_type(std::forward<U>(value)) {}

  template <typename U>
    requires cxx_non_narrowing_integer_conversion<U, UnderlyingType>
  constexpr auto operator=(const signed_integer_type<Tag, U> other)
      -> signed_integer_type&
    requires(!std::is_reference_v<UnderlyingType>)
  {
    this->unwrap() = static_cast<UnderlyingType>(other.unwrap());
    return *this;
  }

  template <typename U>
    requires cxx_non_narrowing_integer_conversion<U, UnderlyingType>
  constexpr auto operator=(const signed_integer_type<Tag, U> other)
      -> signed_integer_type&
    requires std::is_reference_v<UnderlyingType>
  {
    *this->_m_do_not_use_this = static_cast<UnderlyingType>(other.unwrap());
    return *this;
  }
};

////////////////////////
// --- Type Factory ---
////////////////////////

/// \brief Tag type to prevent selection of specialized types in \c new_type.
struct no_skills;

/// \cond
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

template <typename Tag, cxx_mathematical_signed_integer T>
struct _new_type_impl<Tag, T> {
  using type = signed_integer_type<Tag, T>;
};
} // namespace _detail
/// \endcond

// clang-format off
/// \brief Type factory for creating strong types with a specified set of
/// skills.
///
/// The alias template \c new_type creates a strong typedef. The alias template
/// selects the appropriate type in the library based on the provided template
/// arguments. For example, if \c bool is passed as \c T, the resulting type is
/// a specialization of \c boolean_type. If no specialized type is suitable, the
/// resulting type is a specialization of \c strong_type with the specified
/// skills. The user may prevent the selection of specialized types by passing
/// \c no_skills as a skill or explicitly providing a list of skills.
///
/// Examples
///
/// ```cpp
/// using strong_boolean = cina::new_type<struct Tag1, bool>; // strong_boolean is a specialization of boolean_type
/// using strong_integer = cina::new_type<struct Tag2, int>; // strong_integer is a specialization of strong_type
/// using no_skills = cina::new_type<struct Tag3, bool, cina::no_skills>; // no_skills is a specialization of strong_type with no skills
/// using strong_type = cina::new_type<struct Tag4, my_type, cina::output_stream>; // strong_type is a specialization of strong_type that
/// supports outputting to a stream
/// ```
///
/// \tparam Tag A unique type used to create a distinct strong type.
/// \tparam UnderlyingType The underlying type that the strong type wraps.
/// \tparam Args A list of types to customize the generated type.
// clang-format on
template <typename Tag, typename UnderlyingType, typename... Args>
using new_type = _detail::_new_type_impl<Tag, UnderlyingType, Args...>::type;

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