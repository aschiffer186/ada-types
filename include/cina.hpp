/// \file cina.hpp
/// \brief Main header file of Cina library.
/// \author Alex Schiffer

#ifndef CINA_HPP
#define CINA_HPP

#include <compare>
#if defined(_MSC_VER) && _MSC_VER >= 1910
#define CINA_EBCO __declspec(empty_bases)
#else
#define CINA_EBCO
#endif

#include <algorithm> // ranges::copy
#include <concepts> // constructible_from, default_initializable, floating_point, integral, same_as
#include <cstddef>          // size_t
#include <format>           // formatter
#include <functional>       // hash
#include <initializer_list> // initializer_list
#include <iterator>         // back_inserter
#include <ostream>          // ostream
#include <string>           // string
#include <string_view>      // string_view
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

/// \brief Concatenates two instances of \c static_string.
///
/// \tparam LHSSize The size of the left-hand side string.
/// \tparam RHSSize The size of the right-hand side string.
/// \param lhs The left-hand side string.
/// \param rhs The right-hand side string.
/// \return The concatenated string.
template <std::size_t LHSSize, std::size_t RHSSize>
constexpr auto operator+(const static_string<LHSSize>& lhs,
                         const static_string<RHSSize>& rhs) {
  static_string<LHSSize + RHSSize - 1> result{};
  std::ranges::copy(lhs.data, result.data);
  std::ranges::copy(rhs.data, result.data + LHSSize - 1);
  return result;
};

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
    (!std::same_as<std::remove_cvref_t<T>, char>) &&
    (!std::same_as<std::remove_cvref_t<T>, wchar_t>);

template <class T>
concept cxx_arithmetic_unsigned_integral =
    std::unsigned_integral<std::remove_cvref_t<T>> &&
    (!std::same_as<std::remove_cvref_t<T>, char>) &&
    (!std::same_as<std::remove_cvref_t<T>, wchar_t>) &&
    (!std::same_as<std::remove_cvref_t<T>, char8_t>) &&
    (!std::same_as<std::remove_cvref_t<T>, char16_t>) &&
    (!std::same_as<std::remove_cvref_t<T>, char32_t>) &&
    (!std::same_as<std::remove_cvref_t<T>, bool>);

/// \brief Concept modeling that a conversion between two integers is
/// non-narrowing.
///
/// A conversion between two integers is non-narrowing if both integers have the
/// same signedness and no information will be lost during the conversion.
///
/// \param From The type to convert from.
/// \param To The type to convert to.
template <class From, class To>
concept cxx_non_narrowing_integral_conversion =
    std::integral<From> && std::integral<To> &&
    std::is_signed_v<From> == std::is_signed_v<To> &&
    sizeof(From) <= sizeof(To);

/// \brief Concept modeling that a conversion between two floating-point types
/// is non-narrowing.
///
/// This conversion is non-narrowing if the type being convert from is an
/// integer or if both types are floating-point types and the type being
/// converted to is at least as large as the type being converted from.
//
/// \param From The type to convert from.
/// \param To The type to convert to.
template <class From, class To>
concept cxx_non_narrowing_floating_point_conversion =
    (std::integral<From> && std::floating_point<To>) ||
    (std::floating_point<From> && std::floating_point<To> &&
     sizeof(From) <= sizeof(To));

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
/// "arithmetic" signed integer types, which character
/// types (except <tt>signed char</tt> due to the fact \c std::int8_t is often
/// an alias for <tt>signed char</tt>). The type also prohibits narrowing
/// conversions upon construction.
///
/// \tparam Tag The tag of the signed integral type.
/// \tparam T The underlying signed integral type. May be a reference.
template <static_string Tag, cxx_arithmetic_signed_integral T>
class signed_integral_type;

/// \brief Strongly-typed unsigned integer.
///
/// Class template \c unsigned_integral_type is a strongly-typed wrapper around
/// an unsigned integral value. However, the set of supported types is limited
/// to "arithmetic" unsigned integer types, which excludes \c bool. The type
/// also prohibits narrowing conversions upon construction.
///
/// \tparam Tag The tag of the unsigned integral type.
/// \tparam T The underlying unsigned integral type. May be a reference.
template <static_string Tag, cxx_arithmetic_unsigned_integral T>
class unsigned_integral_type;

/// \brief Strongly-typed floating-point number.
///
/// Class template \c floating_point_type is a strongly-typed wrapper around a
/// floating-point value.
///
/// \tparam Tag The tag of the floating-point type.
/// \tparam T The underlying floating-point type. May be a reference.
template <static_string Tag, std::floating_point T> class floating_point_type;

template <static_string Tag, std::floating_point T> class complex_type;

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

template <static_string Tag, cxx_arithmetic_unsigned_integral T>
auto _as_unsigned_integral_type(unsigned_integral_type<Tag, T>)
    -> unsigned_integral_type<Tag, T>;

template <class T, class = void>
constexpr inline bool _is_unsigned_integral_type = false;

template <class T>
constexpr inline bool _is_unsigned_integral_type<
    T, std::void_t<decltype(_as_unsigned_integral_type(std::declval<T>()))>> =
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
concept unsigned_integral = _detail::_is_unsigned_integral_type<T>;

template <class T>
concept integral = signed_integral<T> || unsigned_integral<T>;

// --- Cina Type Traits ---

template <class S>
using remove_reference_t =
    S::template _rebind<std::remove_reference_t<underlying_type<S>>>;

// --- Skills ---

/// \brief Skill representing equality comparison.
///
/// Deriving from the skill adds the ability to compare two instances of the
/// same type for equality. This allows types to model the \c
/// std::equality_comparable concept.
struct equality_comparison {
  template <class Derived> struct skill {
    friend constexpr auto operator==(const Derived& lhs, const Derived& rhs)
        -> bool {
      return lhs.unwrap() == rhs.unwrap();
    }
  };
};

struct three_way_comparison {
  template <class Derived> struct skill {
    friend constexpr auto operator<=>(const Derived& lhs, const Derived& rhs) {
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
      if constexpr (std::is_same_v<
                        std::remove_cvref_t<underlying_type<Derived>>,
                        unsigned char> &&
                    integral<Derived>) {
        return os << +value.unwrap();
      } else if constexpr (std::is_same_v<
                               std::remove_cvref_t<underlying_type<Derived>>,
                               signed char> &&
                           integral<Derived>) {
        return os << +value.unwrap();
      } else {
        return os << value.unwrap();
      }
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

/// \brief Skill representing multiplication.
///
/// Deriving from this skill adds the ability to multiply two instances of the
/// same type.
///
/// \note The type of the result of multiplication may be different from the
/// type of the operands. An example of when this is the case is when expression
/// templates are used.
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

struct bitwise_and {
  template <class Derived> struct skill {
    friend constexpr auto operator&=(Derived& lhs, const Derived& rhs)
        -> Derived& {
      lhs.unwrap() &= rhs.unwrap();
      return lhs;
    }

    friend constexpr auto operator&(const Derived& lhs, const Derived& rhs)
        -> Derived {
      return lhs &= rhs;
    }
  };
};

struct bitwise_or {
  template <class Derived> struct skill {
    friend constexpr auto operator|=(Derived& lhs, const Derived& rhs)
        -> Derived& {
      lhs.unwrap() |= rhs.unwrap();
      return lhs;
    }

    friend constexpr auto operator|(const Derived& lhs, const Derived& rhs)
        -> Derived {
      return lhs |= rhs;
    }
  };
};

struct bitwise_xor {
  template <class Derived> struct skill {
    friend constexpr auto operator^=(Derived& lhs, const Derived& rhs)
        -> Derived& {
      lhs.unwrap() ^= rhs.unwrap();
      return lhs;
    }

    friend constexpr auto operator^(const Derived& lhs, const Derived& rhs)
        -> Derived {
      return lhs ^= rhs;
    }
  };
};

struct bitwise_not {
  template <class Derived> struct skill {
    friend constexpr auto operator~(const Derived& value) -> Derived {
      return Derived(~value.unwrap());
    }
  };
};

struct bitwise_shift {
  template <class Derived> struct skill {
    template <class U>
    friend constexpr auto operator>>=(Derived& lhs, const U& rhs) -> Derived& {
      lhs.unwrap() >> rhs;
      return lhs;
    }

    template <strong_type_like U>
    friend constexpr auto operator>>=(Derived& lhs, const U& rhs) -> Derived& {
      lhs.unwrap() >>= rhs.unwrap();
      return lhs;
    }

    template <class U>
    friend constexpr auto operator<<=(Derived& lhs, const U& rhs) -> Derived& {
      lhs.unwrap() << rhs;
      return lhs;
    }

    template <strong_type_like U>
    friend constexpr auto operator<<=(Derived& lhs, const U& rhs) -> Derived& {
      lhs.unwrap() <<= rhs.unwrap();
      return lhs;
    }

    template <class U>
    friend constexpr auto operator>>(const Derived& lhs, const U& rhs)
        -> Derived {
      return lhs >> rhs;
    }

    template <strong_type_like U>
    friend constexpr auto operator>>(const Derived& lhs, const U& rhs)
        -> Derived {
      return lhs >>= rhs;
    }

    template <class U>
    friend constexpr auto operator<<(const Derived& lhs, const U& rhs)
        -> Derived {
      return lhs << rhs;
    }

    template <strong_type_like U>
    friend constexpr auto operator<<(const Derived& lhs, const U& rhs)
        -> Derived {
      return lhs <<= rhs;
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

  constexpr auto get() const noexcept -> T& { return *_m_do_not_use_directly; }
};
} // namespace _detail
/// \endcond

template <static_string Tag, class T>
class CINA_EBCO strong_type
    : public equality_comparison::skill<strong_type<Tag, T>> {
  using _storage_type = _detail::_strong_type_storage<T>;

  static_assert(!std::is_rvalue_reference_v<T>);
  static_assert(!std::is_void_v<T>);

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

  /// \brief Converting Constructor
  ///
  /// Consructs a \c strong_type instance from another \c strong_type instance
  /// with a different underlying type.
  ///
  /// \pre <tt>std::constructible_from<T, const U&></tt> is modeled.
  /// \post \c other is not modified.
  ///
  /// \tparam U The underlying type of the other \c strong_type instance.
  /// \param other The other \c strong_type instance to construct from.
  /// \throw Any exceptions thrown by the selected constrctor of \c T.
  template <class U>
  constexpr explicit strong_type(const strong_type<Tag, U>& other) noexcept(
      std::is_nothrow_constructible_v<T, const U&>)
    requires std::constructible_from<T, const U&>
      : _m_do_not_use_directly(other.unwrap()) {}

  /// \brief Converting Constructor
  ///
  /// Consructs a \c strong_type instance from another \c strong_type instance
  /// with a different underlying type.
  ///
  /// \pre <tt>std::constructible_from<T, U&&></tt> is modeled and \c T is not a
  /// reference type.
  /// \post \c other is left in a valid but unspecified state.
  ///
  /// \tparam U The underlying type of the other \c strong_type instance.
  /// \param other The other \c strong_type instance to construct from.
  /// \throw Any exceptions thrown by the selected constrctor of \c T.
  template <class U>
  constexpr explicit strong_type(strong_type<Tag, U>&& other) noexcept(
      std::is_nothrow_constructible_v<T, U&&>)
    requires(!std::is_reference_v<T> && std::constructible_from<T, U &&>)
      : _m_do_not_use_directly(std::move(other.unwrap())) {}

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

  template <typename Self>
  constexpr auto unwrap(this Self&& self) noexcept -> auto&&
    requires(!std::is_lvalue_reference_v<T>)
  {
    return std::forward<Self>(self)._m_do_not_use_directly.get();
  }

  constexpr auto unwrap() const noexcept -> T
    requires std::is_lvalue_reference_v<T>
  {
    return _m_do_not_use_directly.get();
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
  constexpr explicit boolean_type(const T& value) noexcept
    requires std::is_reference_v<T>
      : base_type(value) {}

  constexpr explicit boolean_type(const std::remove_reference_t<T>&& value)
    requires std::is_reference_v<T>
  = delete;

  /// \brief Explicit conversion operator
  ///
  /// \return The stored boolean value.
  constexpr explicit operator bool() const noexcept { return this->unwrap(); }

  /// \brief Logical NOT operator.
  ///
  /// \returns \c boolean_type{!this->unwrap()}.
  template <class U = boolean_type> constexpr auto operator!() const noexcept {
    using return_type = boolean_type<Tag, decltype(!this->unwrap())>;
    return return_type{!this->unwrap()};
  }
};

// --- Integral Type Definitions ---

template <static_string Tag, cxx_arithmetic_signed_integral T>
class CINA_EBCO signed_integral_type
    : public strong_type<Tag, T>,
      public three_way_comparison::skill<signed_integral_type<Tag, T>>,
      public addition::skill<signed_integral_type<Tag, T>>,
      public subtraction::skill<signed_integral_type<Tag, T>>,
      public multiplication::skill<signed_integral_type<Tag, T>>,
      public division::skill<signed_integral_type<Tag, T>>,
      public modulo::skill<signed_integral_type<Tag, T>>,
      public negation::skill<signed_integral_type<Tag, T>>,
      public increment::skill<signed_integral_type<Tag, T>>,
      public decrement::skill<signed_integral_type<Tag, T>>,
      public output_stream::skill<signed_integral_type<Tag, T>>,
      public input_stream::skill<signed_integral_type<Tag, T>> {
  using base_type = strong_type<Tag, T>;

public:
  template <class U> using _rebind = signed_integral_type<Tag, U>;

  /// \brief Constructor.
  ///
  /// \pre Converting from \c U to \c T is a non-narrowing conversion and \c T
  /// is not a reference
  /// \post \c this->unwrap() is equal to \c value.
  ///
  /// \tparam U The type to convert from.
  /// \param value The value to initialize the signed integral type with.
  template <cxx_arithmetic_signed_integral U>
    requires cxx_non_narrowing_integral_conversion<U, T> &&
             (!std::is_reference_v<T>)
  constexpr explicit signed_integral_type(const U value) noexcept
      : base_type(static_cast<T>(value)) {}

  /// \brief Constructor.
  ///
  /// \pre \c T is a reference type.
  /// \post \c this->unwrap() is a reference to \c value.
  ///
  /// \param value The reference to initialize the signed integral type with.
  constexpr explicit signed_integral_type(const T& value) noexcept
    requires std::is_reference_v<T>
      : base_type(value) {}

  constexpr explicit signed_integral_type(
      const std::remove_reference_t<T>&& value)
    requires std::is_reference_v<T>
  = delete;
};

template <static_string Tag, cxx_arithmetic_unsigned_integral T>
class CINA_EBCO unsigned_integral_type
    : public strong_type<Tag, T>,
      public three_way_comparison::skill<unsigned_integral_type<Tag, T>>,
      public addition::skill<unsigned_integral_type<Tag, T>>,
      public subtraction::skill<unsigned_integral_type<Tag, T>>,
      public multiplication::skill<unsigned_integral_type<Tag, T>>,
      public division::skill<unsigned_integral_type<Tag, T>>,
      public modulo::skill<unsigned_integral_type<Tag, T>>,
      public increment::skill<unsigned_integral_type<Tag, T>>,
      public decrement::skill<unsigned_integral_type<Tag, T>>,
      public bitwise_and::skill<unsigned_integral_type<Tag, T>>,
      public bitwise_or::skill<unsigned_integral_type<Tag, T>>,
      public bitwise_xor::skill<unsigned_integral_type<Tag, T>>,
      public bitwise_not::skill<unsigned_integral_type<Tag, T>>,
      public bitwise_shift::skill<unsigned_integral_type<Tag, T>>,
      public output_stream::skill<unsigned_integral_type<Tag, T>>,
      public input_stream::skill<unsigned_integral_type<Tag, T>> {
  using base_type = strong_type<Tag, T>;

public:
  template <class U> using _rebind = unsigned_integral_type<Tag, U>;

  template <std::unsigned_integral U>
    requires cxx_non_narrowing_integral_conversion<U, T> &&
             (!std::is_reference_v<T>)
  constexpr explicit unsigned_integral_type(const U value) noexcept
      : base_type(static_cast<T>(value)) {}

  constexpr explicit unsigned_integral_type(const T& value) noexcept
    requires std::is_reference_v<T>
      : base_type(value) {}
};

// --- Floating-Point Type Definition ---

template <static_string Tag, std::floating_point T>
class CINA_EBCO floating_point_type
    : public strong_type<Tag, T>,
      public three_way_comparison::skill<floating_point_type<Tag, T>>,
      public addition::skill<floating_point_type<Tag, T>>,
      public subtraction::skill<floating_point_type<Tag, T>>,
      public multiplication::skill<floating_point_type<Tag, T>>,
      public division::skill<floating_point_type<Tag, T>>,
      public negation::skill<floating_point_type<Tag, T>>,
      public increment::skill<floating_point_type<Tag, T>>,
      public decrement::skill<floating_point_type<Tag, T>>,
      public output_stream::skill<floating_point_type<Tag, T>>,
      public input_stream::skill<floating_point_type<Tag, T>> {
  using base_type = strong_type<Tag, T>;

public:
  template <class U> using _rebind = floating_point_type<Tag, U>;

  template <std::floating_point U>
    requires cxx_non_narrowing_floating_point_conversion<U, T> &&
             (!std::is_reference_v<T>)
  constexpr explicit floating_point_type(const U value) noexcept
      : base_type(static_cast<T>(value)) {}

  constexpr explicit floating_point_type(const T& value) noexcept
    requires std::is_reference_v<T>
      : base_type(value) {}
};

template <static_string Tag, std::floating_point T>
class complex_type : public strong_type<Tag, std::complex<T>> {
  constexpr static static_string real_tag = Tag + static_string{"_real"};
  constexpr static static_string imag_tag = Tag + static_string{"_imag"};

  using base_type = strong_type<Tag, std::complex<T>>;

public:
  using real_type = floating_point_type<real_tag, T>;
  using imaginary_type = floating_point_type<imag_tag, T>;

  template <class U> using _rebind = complex_type<Tag, U>;

  constexpr complex_type(const real_type real, const imaginary_type imag =
                                                   imaginary_type{0.0}) noexcept
      : base_type(std::in_place, real.unwrap(), imag.unwrap()) {}

  constexpr complex_type(const std::complex<T>& value) : base_type(value) {}
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

template <static_string Tag, cxx_arithmetic_signed_integral T>
struct _new_type_impl<Tag, T> {
  using type = signed_integral_type<Tag, T>;
};

template <static_string Tg, cxx_arithmetic_unsigned_integral T>
struct _new_type_impl<Tg, T> {
  using type = unsigned_integral_type<Tg, T>;
};

template <static_string Tag, std::floating_point T>
struct _new_type_impl<Tag, T> {
  using type = floating_point_type<Tag, T>;
};

template <static_string Tag> struct _new_type_impl<Tag, std::complex<float>> {
  using type = complex_type<Tag, float>;
};

template <static_string Tag> struct _new_type_impl<Tag, std::complex<double>> {
  using type = complex_type<Tag, double>;
};

template <static_string Tag>
struct _new_type_impl<Tag, std::complex<long double>> {
  using type = complex_type<Tag, long double>;
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