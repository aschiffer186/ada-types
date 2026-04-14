/// \file cina.hpp
/// \brief Main header file of Cina library.
/// \author Alex Schiffer

#ifndef CINA_HPP
#define CINA_HPP

#include <format>
#include <iterator>
#include <ostream>
#include <string>
#include <string_view>
#if defined(_MSC_VER) && _MSC_VER >= 1910
#define CINA_EBCO __declspec(empty_bases)
#else
#define CINA_EBCO
#endif

#include <algorithm>  // ranges::copy
#include <concepts>   // constructible_from, default_initializable, same_as
#include <cstddef>    // size_t
#include <functional> // hash
#include <initializer_list> // initializer_list
#include <type_traits> // is_nothrow_constructible, is_nothrow_copy_constructible, is_nothrow_move_constructible, remove_cvref_t, remove_reference_t, void_t
#include <utility>     // declval, forward

/// \namespace cina Main namespace of Cina library.
namespace cina {

// --- Static String ---

/// \brief Compile-time string wrapper.
///
/// Class template \c static_string is a simple wrapper around a string literal.
/// It permits using string literals as template parameters.
/// \tparam N The size of the string literal, including the null terminate.
template <std::size_t N> struct static_string {
  char data[N]{};

  /// \brief Constructor
  ///
  /// Constructs a \c static_string from an array of character.
  /// \param str The string literal to be wrapped.
  constexpr static_string(const char (&str)[N]) {
    std::ranges::copy(str, data);
  }
};

template <std::size_t N> static_string(const char (&)[N]) -> static_string<N>;

// --- C++ Language Concepts ---
/// \brief Concept modeling that a type is \c bool or a reference to \c bool.
///
/// \tparam T The type to check.
template <class T>
concept cxx_boolean = std::same_as<std::remove_cvref_t<T>, bool>;

/// \brief Concept modeling that a type is a signed integral used in arithmetic
/// calculations.
///
/// \tparam T The type to check.
template <class T>
concept cxx_arithmetic_signed_integral =
    std::signed_integral<std::remove_cvref_t<T>> &&
    (!std::same_as<std::remove_cvref_t<T>, bool>) &&
    (!std::same_as<std::remove_cvref_t<T>, char8_t>) &&
    (!std::same_as<std::remove_cvref_t<T>, char16_t>) &&
    (!std::same_as<std::remove_cvref_t<T>, char32_t>) &&
    (!std::same_as<std::remove_cvref_t<T>, wchar_t>);

template <class From, class To>
concept cxx_non_narrowing_integral_conversion =
    std::integral<From> && std::integral<To> &&
    std::is_signed_v<From> == std::is_signed_v<To> &&
    sizeof(From) <= sizeof(To);

// --- Forward Declations ---

/// \brief Strong type wrapper.
///
/// Class template \c strong_type transforms an arbitrary type into a different
/// type with a specified tag. This type is different from other types with
/// different tags even if they have the same underlying type.
///
/// \tparam Tag The tag of the strong type. Used to distinguish different strong
/// types with the same underlying type.
/// \tparam T The underlying type of the strong type.
template <static_string Tag, class T> class strong_type;

/// \brief Strongly-typed boolean.
///
/// Class template \c boolean_type is a strongly-typed wrapper around a \c bool
/// value. However, unlike \c bool, there are no implicit conversions to other
/// integral types. It is also not considered an integral type by Cina.
///
/// \tparam Tag The tag of the boolean type.
/// \tparam T The underlying boolean type. May be a reference.
template <static_string Tag, cxx_boolean T> class boolean_type;

/// \brief Strongly-typed signed integer.
///
/// Class template \c signed_integral_type is a strongly-typed wrapper around a
/// signed integral value. However, the set of supported types is limited to
/// "arithmetic" signed integer types, which excludes \c bool and character
/// types. The type also prohibits narrowing conversions upon construction.
///
/// \tparam Tag The tag of the signed integral type.
/// \tparam T The underlying signed integral type. May be a reference.
template <static_string Tag, cxx_arithmetic_signed_integral T>
class signed_integral_type;

/// --- Cina Concepts and Type Traits ----

/// \cond
namespace _detail {
template <static_string Tag, class T>
auto _as_strong_type(strong_type<Tag, T>) -> strong_type<Tag, T>;

template <class T, class = void> constexpr inline bool _is_strong_type = false;

template <class T>
constexpr inline bool _is_strong_type<
    T, std::void_t<decltype(_as_strong_type(std::declval<T>()))>> = true;

template <static_string Tag, class T>
auto _as_underlying_type(strong_type<Tag, T>) -> T;

template <static_string Tag, cxx_arithmetic_signed_integral T>
auto _as_signed_integral_type(signed_integral_type<Tag, T>)
    -> signed_integral_type<Tag, T>;

template <class T, class = void>
constexpr inline bool _is_signed_integral_type = false;

template <class T>
constexpr inline bool _is_signed_integral_type<
    T, std::void_t<decltype(_as_signed_integral_type(std::declval<T>()))>> =
    true;
} // namespace _detail
/// \endcond

/// Concept modeling that a type \c T is an instantiation of class template \c
/// strong_type.
///
/// \tparam T The type to check.
template <class T>
concept strong_type_like = _detail::_is_strong_type<T>;

/// Type alias for the underlying type of a strong type.
///
/// \tparam T The strong type to extract the underlying type from.
template <class T>
using underlying_type =
    decltype(_detail::_as_underlying_type(std::declval<T>()));

/// \brief Returns the tag of a strong type.
///
/// \tparam Tag The tag of the strong type.
/// \tparam T The underlying type of the strong type.
/// \return Tag
template <static_string Tag, class T>
consteval auto tag_of(const strong_type<Tag, T>&) noexcept {
  return Tag;
}

template <class T>
concept signed_integral = _detail::_is_signed_integral_type<T>;

template <class T>
concept integral = signed_integral<T>;

// --- Skills ---

/// \brief Skill representing equality comparison.
///
/// Deriving from the skill adds the ability to compare two instances of the
/// same type for equality. This allows types to model the \c
/// std::equality_comparable concept.
struct equality_comparison {
  template <class Derived> struct skill {
    friend constexpr auto operator==(const Derived& lhs,
                                     const Derived& rhs) noexcept -> bool {
      return lhs.unwrap() == rhs.unwrap();
    }
  };
};

/// \brief Skill representing three-way comparison.
///
/// Deriving from the skill adds the ability to compare two instances of the
/// same type using the three-way comparison operator. This allows types to
/// model the \c std::three_way_comparable concept.
struct three_way_comparison {
  template <class Derived> struct skill {
    friend constexpr auto operator<=>(const Derived& lhs,
                                      const Derived& rhs) noexcept {
      return lhs.unwrap() <=> rhs.unwrap();
    }
  };
};

/// \brief Skill representing the ability output a type.
///
/// Deriving from this skill adds the ability to output an instance of the type
/// to an output stream using the output stream operator \c <<.
struct output_stream {
  template <class Derived> struct skill {
    template <class CharT, class Traits>
    friend constexpr auto operator<<(std::basic_ostream<CharT, Traits>& os,
                                     const Derived& value)
        -> std::basic_ostream<CharT, Traits>& {
      return os << value.unwrap();
    }
  };
};

/// \brief Skill representing the ability to read a type.
///
/// Deriving from this skill adds the ability to read an instance of the type
/// from an input stream using the input stream operator \c >>.
struct input_stream {
  template <class Derived> struct skill {
    template <class CharT, class Traits>
    friend constexpr auto operator>>(std::basic_istream<CharT, Traits>& is,
                                     Derived& value)
        -> std::basic_istream<CharT, Traits>& {
      return is >> value.unwrap();
    }
  };
};

/// \brief Skill representing less-than comparison.
///
/// Deriving from this skill adds the ability to compare two instances of the
/// same type using the less-than operator. This allows types to model the \c
/// std::totally_ordered concept.
struct less {
  template <class Derived> struct skill {
    friend constexpr auto operator<(const Derived& lhs,
                                    const Derived& rhs) noexcept -> bool {
      return lhs.unwrap() < rhs.unwrap();
    }
  };
};

/// \brief Skill representing addition.
///
/// Deriving from this skill adds the ability to add two instances of the same
/// type.
///
/// \note The type of the result of addition may be different from the type of
/// the operands. An example of when this is the case is when expression
/// templates are used.
struct addition {
  template <class Derived> struct skill {
    friend constexpr auto operator+(const Derived& lhs, const Derived& rhs) {
      using underlying_type = decltype(lhs.unwrap() + rhs.unwrap());
      using return_type = typename Derived::template _rebind<underlying_type>;
      return return_type(lhs.unwrap() + rhs.unwrap());
    }

    friend constexpr auto operator+=(Derived& lhs, const Derived& rhs) {
      lhs.unwrap() += rhs.unwrap();
      return lhs;
    }
  };
};

/// \brief Skill representing subtraction.
///
/// Deriving from this skill adds the ability to subtract two instances of the
/// same type.
///
/// \note The type of the result of subtraction may be different from the type
/// of the operands. An example of when this is the case is when expression
/// templates are used.
struct subtraction {
  template <class Derived> struct skill {
    friend constexpr auto operator-(const Derived& lhs, const Derived& rhs) {
      using underlying_type = decltype(lhs.unwrap() - rhs.unwrap());
      using return_type = typename Derived::template _rebind<underlying_type>;
      return return_type(lhs.unwrap() - rhs.unwrap());
    }

    friend constexpr auto operator-=(Derived& lhs, const Derived& rhs) {
      lhs.unwrap() -= rhs.unwrap();
      return lhs;
    }
  };
};

struct multiplication {
  template <class Derived> struct skill {
    friend constexpr auto operator*(const Derived& lhs, const Derived& rhs) {
      using underlying_type = decltype(lhs.unwrap() * rhs.unwrap());
      using return_type = typename Derived::template _rebind<underlying_type>;
      return return_type(lhs.unwrap() * rhs.unwrap());
    }

    friend constexpr auto operator*=(Derived& lhs, const Derived& rhs) {
      lhs.unwrap() *= rhs.unwrap();
      return lhs;
    }
  };
};

struct division {
  template <class Derived> struct skill {
    friend constexpr auto operator/(const Derived& lhs, const Derived& rhs) {
      using underlying_type = decltype(lhs.unwrap() / rhs.unwrap());
      using return_type = typename Derived::template _rebind<underlying_type>;
      return return_type(lhs.unwrap() / rhs.unwrap());
    }

    friend constexpr auto operator/=(Derived& lhs, const Derived& rhs) {
      lhs.unwrap() /= rhs.unwrap();
      return lhs;
    }
  };
};

struct modulo {
  template <class Derived> struct skill {
    friend constexpr auto operator%(const Derived& lhs, const Derived& rhs) {
      using underlying_type = decltype(lhs.unwrap() % rhs.unwrap());
      using return_type = typename Derived::template _rebind<underlying_type>;
      return return_type(lhs.unwrap() % rhs.unwrap());
    }

    friend constexpr auto operator%=(Derived& lhs, const Derived& rhs) {
      lhs.unwrap() %= rhs.unwrap();
      return lhs;
    }
  };
};

struct negation {
  template <class Derived> struct skill {
    friend constexpr auto operator-(const Derived& value) {
      using underlying_type = decltype(-value.unwrap());
      using return_type = typename Derived::template _rebind<underlying_type>;
      return return_type(-value.unwrap());
    }
  };
};

struct increment {
  template <class Derived> struct skill {
    friend constexpr auto operator++(Derived& value) -> Derived& {
      ++value.unwrap();
      return value;
    }

    friend constexpr auto operator++(Derived& value, int) -> Derived {
      Derived temp = value;
      ++value.unwrap();
      return temp;
    }
  };
};

struct decrement {
  template <class Derived> struct skill {
    friend constexpr auto operator--(Derived& value) -> Derived& {
      --value.unwrap();
      return value;
    }

    friend constexpr auto operator--(Derived& value, int) -> Derived {
      Derived temp = value;
      --value.unwrap();
      return temp;
    }
  };
};

// --- Strong Type Definition ---

/// \cond
namespace _detail {
template <class T> class _strong_type_storage {
public:
  T _m_do_not_use_directly{};

  constexpr _strong_type_storage()
    requires std::default_initializable<T>
  = default;

  template <class U = T>
  constexpr _strong_type_storage(U&& value) noexcept(
      std::is_nothrow_constructible_v<T, U&&>)
      : _m_do_not_use_directly(std::forward<U>(value)) {}

  template <class... Args>
  constexpr _strong_type_storage(std::in_place_t, Args&&... args) noexcept(
      std::is_nothrow_constructible_v<T, Args&&...>)
      : _m_do_not_use_directly(std::forward<Args>(args)...) {}

  template <class U, class... Args>
  constexpr _strong_type_storage(
      std::in_place_t, std::initializer_list<U> il,
      Args&&... args) noexcept(std::
                                   is_nothrow_constructible_v<
                                       T, std::initializer_list<U>&, Args&&...>)
      : _m_do_not_use_directly(std::forward<Args>(args)..., il) {}

  constexpr auto get() & noexcept -> T& { return _m_do_not_use_directly; }

  constexpr auto get() const& noexcept -> const T& {
    return _m_do_not_use_directly;
  }

  constexpr auto get() && noexcept -> T&& {
    return std::move(_m_do_not_use_directly);
  }

  constexpr auto get() const&& noexcept -> const T&& {
    return std::move(_m_do_not_use_directly);
  }
};

template <class T> class _strong_type_storage<T&> {
public:
  T* _m_do_not_use_directly;

  template <class U = T>
  constexpr _strong_type_storage(U& value) noexcept
      : _m_do_not_use_directly(&value) {}

  constexpr auto get() noexcept -> std::remove_reference_t<T>& {
    return *_m_do_not_use_directly;
  }

  constexpr auto get() const noexcept -> const std::remove_reference_t<T>& {
    return *_m_do_not_use_directly;
  }
};

template <class T> class _strong_type_storage<const T&> {
public:
  const T* _m_do_not_use_directly;

  template <class U = T>
  constexpr _strong_type_storage(const U& value) noexcept
      : _m_do_not_use_directly(&value) {}

  constexpr auto get() const noexcept -> const std::remove_reference_t<T>& {
    return *_m_do_not_use_directly;
  }
};
} // namespace _detail
/// \endcond

template <static_string Tag, class T> class CINA_EBCO strong_type {
public:
  template <class U> using _rebind = strong_type<Tag, U>;

  /// \brief Default constructor
  ///
  /// \pre \c T models \c std::default_initializable.
  /// \throw Any exceptions thrown by the default constructor of \c T.
  constexpr strong_type()
    requires std::default_initializable<T>
  = default;

  /// \brief Constructor
  ///
  /// Constructs a \c strong_type instance with a given value. The given value
  /// is copied into the \c strong_type instance.
  ///
  /// \pre \c T model <tt>std::constructible_from<T, U&&></tt>.
  /// \post \c this->unwrap() is equal to \c value.
  ///
  /// \param value The value to initialize the strong type with.
  /// \throw Any exceptions thrown by the copy constructor of \c T.
  constexpr explicit strong_type(const T& value) noexcept(
      std::is_nothrow_copy_constructible_v<T>)
    requires std::constructible_from<T, const T&>
      : _m_do_not_use_directly(value) {}

  /// \brief Constructor
  ///
  /// Constructs a \c strong_type instance with a given value. The given value
  /// is moved into the \c strong_type instance.
  ///
  /// \pre \c T model <tt>std::constructible_from<T, T&&></tt>.
  /// \post \c this->unwrap() is equal to \c value prior to the construction and
  /// \c value is left in a valid but unspecified state.
  ///
  /// \param value The value to initialize the strong type with.
  /// \throw Any exceptions thrown by the move constructor of \c T.
  constexpr explicit strong_type(T&& value) noexcept(
      std::is_nothrow_move_constructible_v<T>)
    requires(!std::is_reference_v<T> && std::constructible_from<T, T &&>)
      : _m_do_not_use_directly(std::move(value)) {}

  /// \brief Constructor
  ///
  /// Constructs a \c strong_type instance in-place with the given arguments as
  /// if by \c T(std::forward<Args>(args)...).
  ///
  /// \pre \c T model <tt>std::constructible_from<T, Args&&...></tt>.
  /// \post \c this->unwrap() is equal to \c T(std::forward<Args>(args)...).
  ///
  /// \tparam Args The types of the arguments to construct the underlying type.
  /// \param args The arguments to construct the underlying type.
  /// \throw Any exceptions thrown by the selected constructor of \c T.
  template <class... Args>
    requires(!std::is_reference_v<T> && std::constructible_from<T, Args...>)
  constexpr strong_type(std::in_place_t, Args&&... args) noexcept(
      std::is_nothrow_constructible_v<T, Args&&...>)
      : _m_do_not_use_directly(std::in_place, std::forward<Args>(args)...) {}

  /// \brief Constructor
  ///
  /// Constructs a \c strong_type instance in-place with the given initializer
  /// list and arguments as if by <tt>T(il, std::forward<Args>(args)...)</tt>.
  ///
  /// \pre \c T model <tt>std::constructible_from<T, std::initializer_list<U>&,
  /// Args&&...></tt>.
  /// \post \c this->unwrap() is equal to <tt>T(il,
  /// std::forward<Args>(args)...)</tt>.
  ///
  /// \tparam U The type of the elements in the initializer list.
  /// \tparam Args The types of the arguments to construct the underlying type.
  /// \param il The initializer list to construct the underlying type.
  /// \param args The arguments to construct the underlying type.
  /// \throw Any exceptions thrown by the selected constructor of \c T.
  template <class U, class... Args>
    requires(!std::is_reference_v<T> &&
             std::constructible_from<T, std::initializer_list<U>&, Args...>)
  constexpr strong_type(
      std::in_place_t, std::initializer_list<U> il,
      Args&&... args) noexcept(std::
                                   is_nothrow_constructible_v<
                                       T, std::initializer_list<U>&, Args&&...>)
      : _m_do_not_use_directly(std::in_place, il, std::forward<Args>(args)...) {
  }

  /// \brief Returns a reference to the underlying value.
  ///
  /// \pre \c T is not \c const.
  ///
  /// \return A reference to the underlying value.
  constexpr auto unwrap() & noexcept -> std::remove_reference_t<T>& {
    return _m_do_not_use_directly.get();
  }

  /// \brief Returns a const reference to the underlying value.
  ///
  /// \pre \c T is \c const.
  ///
  /// \return A const reference to the underlying value.
  constexpr auto unwrap() const& noexcept
      -> std::add_const_t<std::remove_reference_t<T>>& {
    return _m_do_not_use_directly.get();
  }

  /// \brief Returns an rvalue reference to the underlying value.
  ///
  /// \pre \c T is not an lvalue reference.
  ///
  /// \return An rvalue reference to the underlying value.
  constexpr auto unwrap() && noexcept -> std::remove_reference_t<T>&&
    requires(!std::is_lvalue_reference_v<T>)
  {
    return std::move(_m_do_not_use_directly).get();
  }

  /// \brief Returns a const rvalue reference to the underlying value.
  ///
  /// \pre \c T is not an lvalue reference.
  ///
  /// \return A const rvalue reference to the underlying value.
  constexpr auto unwrap() const&& noexcept
      -> std::add_const_t<std::remove_reference_t<T>>&&
    requires(!std::is_lvalue_reference_v<T>)
  {
    return std::move(_m_do_not_use_directly).get();
  }

  /// Do not use this value directly, use \c unwrap() instead.
  _detail::_strong_type_storage<T> _m_do_not_use_directly;
};

// --- Boolean Type Definition ---

template <static_string Tag, cxx_boolean T>
class boolean_type : public strong_type<Tag, T>,
                     public equality_comparison::skill<boolean_type<Tag, T>>,
                     public input_stream::skill<boolean_type<Tag, T>>,
                     public output_stream::skill<boolean_type<Tag, T>> {
  using base_type = strong_type<Tag, T>;

public:
  template <class U> using _rebind = boolean_type<Tag, U>;

  /// \brief Constructor
  ///
  /// Constructs a \c boolean_type instance with a given value.
  ///
  /// \pre \c T is not a reference type.
  /// \post \c this->unwrap() is equal to \c value.
  ///
  /// \param value The value to initialize the boolean type with.
  constexpr boolean_type(const T value) noexcept
    requires(!std::is_reference_v<T>)
      : base_type(value) {}

  /// \brief Constructor
  ///
  /// Constructs a \c boolean_type instance with a given reference.
  ///
  /// \pre \c T is a reference type.
  /// \post \c this->unwrap() is a reference to \c value.
  ///
  /// \param value The reference to initialize the boolean type with.
  constexpr boolean_type(const T& value) noexcept
    requires std::is_reference_v<T>
      : base_type(value) {}

  /// \brief Explicit conversion operator
  ///
  /// \return The stored boolean value.
  constexpr explicit operator bool() const noexcept { return this->unwrap(); }

  /// \brief Logical NOT operator.
  ///
  /// \returns \c boolean_type{!this->unwrap()}.
  constexpr auto operator!() const noexcept -> boolean_type {
    return boolean_type(!this->unwrap());
  }
};

/// --- Integral Type Definitions ---
template <static_string Tag, cxx_arithmetic_signed_integral T>
class signed_integral_type
    : public strong_type<Tag, T>,
      public addition::skill<signed_integral_type<Tag, T>>,
      public subtraction::skill<signed_integral_type<Tag, T>>,
      public multiplication::skill<signed_integral_type<Tag, T>>,
      public division::skill<signed_integral_type<Tag, T>>,
      public modulo::skill<signed_integral_type<Tag, T>>,
      public negation::skill<signed_integral_type<Tag, T>>,
      public increment::skill<signed_integral_type<Tag, T>>,
      public decrement::skill<signed_integral_type<Tag, T>>,
      public three_way_comparison::skill<signed_integral_type<Tag, T>>,
      public output_stream::skill<signed_integral_type<Tag, T>>,
      public input_stream::skill<signed_integral_type<Tag, T>> {
  using base_type = strong_type<Tag, T>;

public:
  template <class U> using _rebind = signed_integral_type<Tag, U>;

  template <cxx_arithmetic_signed_integral U>
    requires cxx_non_narrowing_integral_conversion<U, T> &&
             (!std::is_reference_v<T>)
  constexpr explicit signed_integral_type(const U value) noexcept
      : base_type(static_cast<T>(value)) {}

  constexpr explicit signed_integral_type(const T& value) noexcept
    requires std::is_reference_v<T>
      : base_type(value) {}
};

// -- Type Factory ---
/// \brief Tag type indicating a strong type should not support any skills.
struct no_skills {};

///\cond
namespace _detail {
template <static_string Tag, class T, class... Skills> struct _new_type_impl {
  struct impl : public strong_type<Tag, T>,
                public Skills::template skill<impl>... {
    using strong_type<Tag, T>::strong_type;
  };

  using type = impl;
};

template <static_string Tag, class T, class... Args>
struct _new_type_impl<Tag, T, no_skills, Args...> {
  using type = strong_type<Tag, T>;
};

template <static_string Tag, cxx_boolean T> struct _new_type_impl<Tag, T> {
  using type = boolean_type<Tag, T>;
};
} // namespace _detail
/// \endcond

template <static_string Tag, class T, class... Args>
using new_type = _detail::_new_type_impl<Tag, T, Args...>::type;

} // namespace cina

template <cina::strong_type_like T> struct std::hash<T> {
  auto operator()(const T& value) const noexcept -> size_t {
    return hash<cina::underlying_type<T>>{}(value.unwrap());
  }
};

template <cina::strong_type_like T>
struct std::formatter<T> : formatter<string_view> {
  auto format(const T& value, format_context& ctx) const {
    string temp;
    format_to(back_inserter(temp), "{}", value.unwrap());
    return formatter<string_view>::format(temp, ctx);
  }
};

#endif