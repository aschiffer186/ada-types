#ifndef CINA_HPP
#define CINA_HPP

#include <cmath>            // abs, sqrt, etc.
#include <complex>          // complex
#include <concepts>         // same_as
#include <format>           // formatter
#include <initializer_list> // initializer_list
#include <limits>           // numeric_limits
#include <memory>           // unique_ptr
#include <ostream>          // ostream
#include <stdexcept>        // runtime_error
#include <string>           // string
#include <string_view>      // string_view
#include <type_traits>      // remove_cvref_t, other traits
#include <utility>          // in_place, in_place_t
#include <version>          // version macros

#if defined(__cpp_lib_constexpr_memory) && __cpp_lib_constexpr_memory >= 202202L
#define CINA_POINTER_CONSTEXPR constexpr
#else
#define CINA_POINTER_CONSTEXPR
#endif

#if defined(__cpp_lib_constexpr_cmath) && __cpp_lib_constexpr_cmath >= 202202L
#define CINA_BASIC_CMATH_CONSTEXPR constexpr
#else
#define CINA_BASIC_CMATH_CONSTEXPR
#endif

#if defined(__cpp_lib_constexpr_cmath) && __cpp_lib_constexpr_cmath >= 202306L
#define CINA_CMATH_CONSTEXPR constexpr
#else
#define CINA_CMATH_CONSTEXPR
#endif

namespace cina {

// --- C++ Concepts ---

/// \brief Concept modeling that a type supports being written to an output
/// stream.
///
/// \tparam T The type to check.
template <typename T>
concept cxx_streamable = requires(T a, std::ostream& os) {
  { os << a } -> std::same_as<std::ostream&>;
};

template <typename T>
concept cxx_input_streamable = requires(T a, std::istream& is) {
  { is >> a } -> std::same_as<std::istream&>;
};

/// \tparam Concept modeling that \c std::hash is enabled for a type.
///
/// \tparam T The type to check.
template <typename T>
concept cxx_hashable = requires(T a) {
  { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
};

/// \brief Concept modeling that a type is an "arithmetic" signed integral type.
///
/// This concept is modeled if \c T is a signed-integral type and \c T is not
/// one of \c bool, \c char16_t, \c char32_t, or \c wchar_t.
///
/// \tparam T The type to check.
template <typename T>
concept cxx_arithmetic_integral =
    std::signed_integral<T> && !std::same_as<T, bool> &&
    !std::same_as<T, char16_t> && !std::same_as<T, char32_t> &&
    !std::same_as<T, wchar_t>;

/// \brief Concept modeling that a type is an unsigned integral type.
///
/// \tparam T The type to check.
template <typename T>
concept cxx_unsigned_integral = std::unsigned_integral<T>;

/// \brief Concept modeling that a type satisfies the NullbalePointer
/// requirement in the C++ standard.
///
/// \tparam T The type to check.
template <typename T>
concept cxx_nullable_pointer =
    std::equality_comparable<T> && std::is_default_constructible_v<T> &&
    std::is_copy_constructible_v<T> && std::is_copy_assignable_v<T> &&
    std::is_swappable_v<T> && std::is_destructible_v<T> &&
    requires(T ptr1, T ptr2) {
      { ptr1 = nullptr } -> std::same_as<T&>;
      { ptr1 == ptr2 } -> std::convertible_to<bool>;
      { ptr1 == nullptr } -> std::convertible_to<bool>;
      { nullptr == ptr1 } -> std::convertible_to<bool>;
      { ptr1 != nullptr } -> std::convertible_to<bool>;
      { nullptr != ptr1 } -> std::convertible_to<bool>;
    };

/// \brief Concept modeling that a type satisifes the Container requirement in
/// the C++ standard.
///
/// \tparam T The type to check.
template <typename T>
concept cxx_container = requires {
  typename T::value_type;
  typename T::reference;
  typename T::const_reference;
  typename T::size_type;
  typename T::difference_type;
  typename T::iterator;
  typename T::const_iterator;
};

/// \brief Concept modeling that a type satisfies the AllocatorAwareContainer
/// requirement in the C++ standard.
///
/// \tparam C The type to check.
template <typename C>
concept cxx_allocator_aware_container =
    cxx_container<C> && requires(const C c) {
      typename C::allocator_type;
      { c.get_allocator() } -> std::same_as<typename C::allocator_type>;
    };

/// \brief Concept modeling that a type satisfies the SequenceContainer
/// requirement in the C++ standard.
///
/// \tparam C The type to check.
template <typename C>
concept cxx_sequence_container =
    cxx_container<C> &&
    requires(std::initializer_list<typename C::value_type> il, C v, const C& cv,
             typename C::const_iterator p, C::value_type t,
             C::value_type&& rt) {
      { C(il) } -> std::same_as<C>;
      { v = il } -> std::same_as<C&>;
      { v.insert(p, t) } -> std::same_as<typename C::iterator>;
      { v.insert(p, rt) } -> std::same_as<typename C::iterator>;
    };

/// \brief Concept indicating that a conversion between two integer types is
/// non-narrowing.
///
/// Concept indicating that a conversion between two integer types is
/// non-narrowing. This concept is only modeled if:
/// 1. Both \c From and \c To are integral types of the same signedness, and
///    the size of \c From is less than or equal to the size of \c To.
template <typename From, typename To>
concept non_narrowing_integer_conversion =
    std::integral<From> && std::integral<To> && sizeof(From) <= sizeof(To) &&
    std::is_signed_v<From> == std::is_signed_v<To>;

/// \brief Concept indicating that a floating-point conversion is non-narrowing.
///
/// Concpet indicating that a floating-point conversion is non-narrowing. This
/// concept is modeled if
/// 1. \c From is an integral type and \c To is a floating-point type.
/// 2. \c From and \c To are both floating-point types and \c sizeof(From) <= \c
/// sizeof(To).
template <typename From, typename To>
concept non_narrowing_floating_point_conversion =
    (std::integral<From> && std::floating_point<To>) ||
    (std::floating_point<From> && std::floating_point<To> &&
     sizeof(From) <= sizeof(To));

// --- Forward Declarations ---

template <typename Tag, typename UnderlyingType> class strong_type;

template <typename Tag> class boolean_type;

template <typename Tag, cxx_arithmetic_integral T> class signed_integral_type;

template <typename Tag, cxx_arithmetic_integral T>
class bitwise_signed_integral_type;

template <typename Tag, std::floating_point T> class floating_point_type;

template <typename Tag, std::floating_point T> class complex_type;

template <typename Tag, cxx_arithmetic_integral T, std::intmax_t Min,
          std::intmax_t Max>
class bounded_integral_type;

template <typename Tag, cxx_nullable_pointer Pointer> class pointer_type;

template <typename Tag, typename T, typename Deleter = std::default_delete<T>>
class unique_ptr_type;

template <typename Tag, cxx_container Container> class container_type;

template <typename Tag, cxx_allocator_aware_container Container>
class allocator_aware_container_type;

template <typename Tag, cxx_sequence_container Container>
class sequence_container_type;

// --- Cina Concepts ---

/// \cond
namespace _detail {
template <typename Tag, typename UnderlyingType>
auto _strong_type_base(const strong_type<Tag, UnderlyingType>&)
    -> strong_type<Tag, UnderlyingType>;

template <typename T>
using _strong_type_base_t = decltype(_strong_type_base(std::declval<T>()));

template <typename, typename = void> constexpr bool is_strong_type_impl = false;

template <typename T>
constexpr bool is_strong_type_impl<T, std::void_t<_strong_type_base_t<T>>> =
    true;

template <typename Tag, typename UnderlyingType>
auto _get_underlying_type(const strong_type<Tag, UnderlyingType>&)
    -> UnderlyingType;

} // namespace _detail
/// \endcond

/// \brief Concept modeling that a type is a strong type provided by the cina
/// library.
template <typename T>
concept strong_type_like = _detail::is_strong_type_impl<std::remove_cvref_t<T>>;

/// \brief Computes the underlying type of a strong type provided by the cina
/// library.
///
/// \tparam T The strong type to compute the underlying type of.
template <strong_type_like T>
using underlying_type =
    decltype(_detail::_get_underlying_type(std::declval<T>()));

/// \cond
namespace _detail {
template <typename> constexpr bool is_integral_type = false;

template <typename Tag, cxx_arithmetic_integral T>
constexpr bool is_integral_type<signed_integral_type<Tag, T>> = true;

template <typename> constexpr bool is_bitwise_integral_type = false;

template <typename Tag, cxx_arithmetic_integral T>
constexpr bool is_bitwise_integral_type<bitwise_signed_integral_type<Tag, T>> =
    true;

template <typename> constexpr bool is_floating_point_type = false;

template <typename Tag, std::floating_point T>
constexpr bool is_floating_point_type<floating_point_type<Tag, T>> = true;

template <typename> constexpr bool is_bounded_integral = false;

template <typename Tag, cxx_arithmetic_integral T, std::intmax_t Min,
          std::intmax_t Max>
constexpr bool is_bounded_integral<bounded_integral_type<Tag, T, Min, Max>> =
    true;

template <typename> constexpr bool is_complex_type = false;

template <typename Tag, std::floating_point T>
constexpr bool is_complex_type<complex_type<Tag, T>> = true;
} // namespace _detail
/// \endcond

/// \brief Concept modeling that a type is an instantiation of \c
/// cina::integral_type or \c cina::bitwise_integral_type.
///
/// \tparam T The type to check.
template <typename T>
concept integral = _detail::is_integral_type<std::remove_cvref_t<T>> ||
                   _detail::is_bitwise_integral_type<std::remove_cvref_t<T>>;

/// \brief Concept modeling that a type is an instantiation of \c
/// cina::bounded_integral_type.
///
/// \tparam T The type to check.
template <typename T>
concept bounded_integral = _detail::is_bounded_integral<std::remove_cvref_t<T>>;

template <typename T, std::intmax_t Min, std::intmax_t Max>
concept integer_in_range =
    bounded_integral<T> && T::min.unwrap() >= Min && T::max.unwrap() <= Max;

template <typename T>
concept floating_point =
    _detail::is_floating_point_type<std::remove_cvref_t<T>>;

template <typename T>
concept arithmetic = integral<T> || floating_point<T> || bounded_integral<T>;

template <typename T>
concept complex = _detail::is_complex_type<std::remove_cvref_t<T>>;

// --- Custom Exceptions ---

class cina_exception : public std::runtime_error {
public:
  explicit cina_exception(const std::string& what_arg)
      : std::runtime_error(what_arg) {}
};

class out_of_range : public cina_exception {
public:
  explicit out_of_range(const std::string& what_arg)
      : cina_exception(what_arg) {}
};

class overflow_error : public cina_exception {
public:
  explicit overflow_error(const std::string& what_arg)
      : cina_exception(what_arg) {}
};

// --- Skills ---

struct equality_comparison {
  template <typename Derived> struct skill {
    friend constexpr auto operator==(const Derived& lhs, const Derived& rhs)
        -> bool
      requires std::equality_comparable<underlying_type<Derived>>
    {
      return lhs.unwrap() == rhs.unwrap();
    }
  };
};

struct three_way_comparison {
  template <typename Derived> struct skill {
    friend constexpr auto operator<=>(const Derived& lhs, const Derived& rhs)
      requires std::three_way_comparable<underlying_type<Derived>>
    {
      return lhs.unwrap() <=> rhs.unwrap();
    }
  };
};

struct less {
  template <typename Derived> struct skill {
    friend constexpr auto operator<(const Derived& lhs,
                                    const Derived& rhs) noexcept -> bool {
      return lhs.unwrap() < rhs.unwrap();
    }
  };
};

/// \cond
namespace _detail {
template <typename> constexpr bool is_int8_type = false;

template <typename Tag>
constexpr bool is_int8_type<signed_integral_type<Tag, std::int8_t>> = true;

template <typename Tag>
constexpr bool is_int8_type<bitwise_signed_integral_type<Tag, std::int8_t>> =
    true;
} // namespace _detail
/// \endcond

struct output_stream {
  template <typename Derived> struct skill {
    friend constexpr auto operator<<(std::ostream& os, const Derived& value)
        -> std::ostream&
      requires cxx_streamable<underlying_type<Derived>>
    {
      if constexpr (_detail::is_int8_type<Derived>) {
        // Prevent printing as char
        return os << static_cast<int>(value.unwrap());
      } else {
      }
      return os << value.unwrap();
    }
  };
};

struct input_stream {
  template <typename Derived> struct skill {
    friend auto operator>>(std::istream& is, Derived& value) -> std::istream&
      requires cxx_input_streamable<underlying_type<Derived>>
    {
      return is >> value.unwrap();
    }
  };
};

struct addition {
  template <typename Derived> struct skill {
    friend constexpr auto operator+(const Derived& lhs,
                                    const Derived& rhs) noexcept -> Derived {
      return Derived(lhs.unwrap() + rhs.unwrap());
    }

    friend constexpr auto operator+=(Derived& lhs, const Derived& rhs) noexcept
        -> Derived& {
      lhs = lhs + rhs;
      return lhs;
    }
  };
};

struct subtraction {
  template <typename Derived> struct skill {
    friend constexpr auto operator-(const Derived& lhs,
                                    const Derived& rhs) noexcept -> Derived {
      return Derived(lhs.unwrap() - rhs.unwrap());
    }

    friend constexpr auto operator-=(Derived& lhs, const Derived& rhs) noexcept
        -> Derived& {
      lhs = lhs - rhs;
      return lhs;
    }
  };
};

struct multiplication {
  template <typename Derived> struct skill {
    friend constexpr auto operator*(const Derived& lhs,
                                    const Derived& rhs) noexcept -> Derived {
      return Derived(lhs.unwrap() * rhs.unwrap());
    }

    friend constexpr auto operator*=(Derived& lhs, const Derived& rhs) noexcept
        -> Derived& {
      lhs = lhs * rhs;
      return lhs;
    }
  };
};

struct division {
  template <typename Derived> struct skill {
    friend constexpr auto operator/(const Derived& lhs,
                                    const Derived& rhs) noexcept -> Derived {
      return Derived(lhs.unwrap() / rhs.unwrap());
    }

    friend constexpr auto operator/=(Derived& lhs, const Derived& rhs) noexcept
        -> Derived& {
      lhs = lhs / rhs;
      return lhs;
    }
  };
};

struct modulo {
  template <typename Derived> struct skill {
    friend constexpr auto operator%(const Derived& lhs,
                                    const Derived& rhs) noexcept -> Derived {
      return Derived(lhs.unwrap() % rhs.unwrap());
    }

    friend constexpr auto operator%=(Derived& lhs, const Derived& rhs) noexcept
        -> Derived& {
      lhs = lhs % rhs;
      return lhs;
    }
  };
};

struct negation {
  template <typename Derived> struct skill {
    friend constexpr auto operator-(const Derived& value) noexcept -> Derived {
      return Derived(-value.unwrap());
    }
  };
};

struct increment {
  template <typename Derived> struct skill {
    friend constexpr auto operator++(Derived& value) noexcept -> Derived& {
      value = Derived(value.unwrap() + 1);
      return value;
    }

    friend constexpr auto operator++(Derived& value, int) noexcept -> Derived {
      Derived old_value = value;
      ++value;
      return old_value;
    }
  };
};

struct decrement {
  template <typename Derived> struct skill {
    friend constexpr auto operator--(Derived& value) noexcept -> Derived& {
      value = Derived(value.unwrap() - 1);
      return value;
    }

    friend constexpr auto operator--(Derived& value, int) noexcept -> Derived {
      Derived old_value = value;
      --value;
      return old_value;
    }
  };
};
struct bitwise_and {
  template <typename Derived> struct skill {
    friend constexpr auto operator&(const Derived& lhs,
                                    const Derived& rhs) noexcept -> Derived {
      return Derived(lhs.unwrap() & rhs.unwrap());
    }

    friend constexpr auto operator&=(Derived& lhs, const Derived& rhs) noexcept
        -> Derived& {
      lhs = lhs & rhs;
      return lhs;
    }
  };
};

struct bitwise_or {
  template <typename Derived> struct skill {
    friend constexpr auto operator|(const Derived& lhs,
                                    const Derived& rhs) noexcept -> Derived {
      return Derived(lhs.unwrap() | rhs.unwrap());
    }

    friend constexpr auto operator|=(Derived& lhs, const Derived& rhs) noexcept
        -> Derived& {
      lhs = lhs | rhs;
      return lhs;
    }
  };
};

struct bitwise_xor {
  template <typename Derived> struct skill {
    friend constexpr auto operator^(const Derived& lhs,
                                    const Derived& rhs) noexcept -> Derived {
      return Derived(lhs.unwrap() ^ rhs.unwrap());
    }

    friend constexpr auto operator^=(Derived& lhs, const Derived& rhs) noexcept
        -> Derived& {
      lhs = lhs ^ rhs;
      return lhs;
    }
  };
};

struct bitwise_not {
  template <typename Derived> struct skill {
    friend constexpr auto operator~(const Derived& value) noexcept -> Derived {
      return Derived(~value.unwrap());
    }
  };
};

struct bitwise_shift {
  template <typename Derived> struct skill {
    friend constexpr auto operator<<(const Derived& lhs,
                                     const Derived& rhs) noexcept -> Derived {
      return Derived(lhs.unwrap() << rhs.unwrap());
    }

    friend constexpr auto operator<<=(Derived& lhs, const Derived& rhs) noexcept
        -> Derived& {
      lhs = lhs << rhs;
      return lhs;
    }

    friend constexpr auto operator>>(const Derived& lhs,
                                     const Derived& rhs) noexcept -> Derived {
      return Derived(lhs.unwrap() >> rhs.unwrap());
    }

    friend constexpr auto operator>>=(Derived& lhs, const Derived& rhs) noexcept
        -> Derived& {
      lhs = lhs >> rhs;
      return lhs;
    }
  };
};

struct dereference {
  template <typename Derived> struct skill {
    friend constexpr decltype(auto) operator*(const Derived& derived) {
      return *derived.unwrap();
    }
  };
};

namespace _detail {
template <typename...> constexpr bool dependent_false = false;
}

template <typename Tag, typename UnderlyingType>
class strong_type
    : public equality_comparison::skill<strong_type<Tag, UnderlyingType>> {
public:
  /// Type alias for the underlying type.
  using underlying_type = UnderlyingType;
  /// Type alias for the tag type.
  using tag = Tag;

  template <typename NewTag> using rebind = strong_type<NewTag, UnderlyingType>;

  /// \brief Default constructor.
  ///
  /// Default initializes the underlying value of the \c strong_type.
  /// This constructor is a \c constexpr constructor if the default
  /// constructor of \c UnderlyingType is a \c constexpr constructor.
  ///
  /// \pre The underlying type is default constructible.
  /// \post \c unwrap() returns a value equivalent to \c UnderlyingType{}.
  /// \throws Any exceptions thrown by the default constructor of \c
  /// UnderlyingType.
  constexpr strong_type()
    requires std::is_default_constructible_v<UnderlyingType>
  = default;

  /// \brief Constructor
  ///
  /// Initializes the underlying value of the \c strong_type with the given
  /// value as if by \c std::forward<U>(value).
  /// This constructor is a \c constexpr constructor if the selected
  /// constructor of \c UnderlyingType is a \c constexpr constructor.
  ///
  /// \pre <tt>std::constructible_from<UnderlyingType, U></tt> is modeled and
  /// \c U is not a specialization of \c strong_type.
  /// \post \c unwrap() returns a value equivalent to \c
  /// UnderlyingType(std::forward<U>(value)).
  ///
  /// \tparam U The type of the value to initialize the underlying value with.
  /// \param value The value to initialize the underlying value with.
  /// \throws Any exceptions thrown by the selected constructor of \c
  /// UnderlyingType.
  template <typename U = UnderlyingType>
    requires std::constructible_from<UnderlyingType, U> &&
             (!strong_type_like<U>)
  constexpr explicit strong_type(U&& value) noexcept(
      std::is_nothrow_constructible_v<UnderlyingType, U>)
      : _m_do_not_use_only_public_to_make_usable_as_nttp(
            std::forward<U>(value)) {}

  /// \brief Constructor
  ///
  /// Initializes the underlying value of the \c strong_type in-place with the
  /// given arguments.
  /// This constructor is a \c constexpr constructor if the selected
  /// constructor of \c UnderlyingType is a \c constexpr constructor.
  ///
  /// \pre <tt>std::constructible_from<UnderlyingType, Args...></tt> is
  /// modeled.
  /// \post \c unwrap() returns a value equivalent to \c
  /// UnderlyingType(std::forward<Args>(args)...).
  ///
  /// \tparam Args The types of the arguments to initialize the underlying
  /// value with.
  /// \param args The arguments to initialize the underlying value with.
  /// \throws Any exceptions thrown by the selected constructor of \c
  /// UnderlyingType.
  template <typename... Args>
    requires std::constructible_from<UnderlyingType, Args...>
  constexpr explicit strong_type(std::in_place_t, Args&&... args) noexcept(
      std::is_nothrow_constructible_v<UnderlyingType, Args...>)
      : _m_do_not_use_only_public_to_make_usable_as_nttp(
            std::forward<Args>(args)...) {}

  /// \brief Constructor
  ///
  /// Initializes the underlying value of the \c strong_type in-place with the
  /// given arguments and initializer list.
  /// This constructor is a \c constexpr constructor if the selected
  /// constructor of \c UnderlyingType is a \c constexpr constructor.
  ///
  /// \pre <tt>std::constructible_from<UnderlyingType,
  /// std::initializer_list<U>&, Args...></tt> is modeled.
  /// \post \c unwrap() returns a value equivalent to \c
  /// UnderlyingType(std::forward<Args>(args)...).
  ///
  /// \tparam U The type of the elements in the initializer list.
  /// \tparam Args The types of the arguments to initialize the underlying
  /// value with.
  /// \param il The initializer list to initialize the underlying value with.
  /// \param args The arguments to initialize the underlying value with.
  /// \throws Any exceptions thrown by the selected constructor of \c
  /// UnderlyingType.
  template <typename U, typename... Args>
    requires std::constructible_from<UnderlyingType, std::initializer_list<U>&,
                                     Args...>
  constexpr explicit strong_type(
      std::in_place_t, std::initializer_list<U> il,
      Args&&... args) noexcept(std::
                                   is_nothrow_constructible_v<
                                       UnderlyingType,
                                       std::initializer_list<U>&, Args...>)
      : _m_do_not_use_only_public_to_make_usable_as_nttp(
            il, std::forward<Args>(args)...) {}

  /// \brief Returns a reference to the underlying value.
  ///
  /// \return A const-reference to the underlying value.
  constexpr auto unwrap() const& noexcept -> const UnderlyingType& {
    return _m_do_not_use_only_public_to_make_usable_as_nttp;
  }

  constexpr auto unwrap() && noexcept -> UnderlyingType&& {
    return std::move(_m_do_not_use_only_public_to_make_usable_as_nttp);
  }

  constexpr auto unwrap() const&& noexcept -> const UnderlyingType&& {
    return std::move(_m_do_not_use_only_public_to_make_usable_as_nttp);
  }

  /// Should only be used by Cina, not by users directly.
  /// Use unwrap() instead!.
  UnderlyingType _m_do_not_use_only_public_to_make_usable_as_nttp{};
};

// --- Boolean Type ---

/// \brief Strongly-type boolean value.
///
/// Class template \c boolean_type is a strongly typed boolean value.
/// This type provides additional safety over the built-in \c bool by not
/// being implicitly convertible to integral types and not providing overloads
/// of any arithmetic operations.
///
/// \tparam Tag A unique tag type to distinguish different boolean types.
template <typename Tag>
class boolean_type : public strong_type<Tag, bool>,
                     public equality_comparison::skill<boolean_type<Tag>>,
                     public less::skill<boolean_type<Tag>>,
                     public output_stream::skill<boolean_type<Tag>>,
                     public input_stream::skill<boolean_type<Tag>> {

  using base_type = strong_type<Tag, bool>;

public:
  template <typename NewTag> using rebind = boolean_type<NewTag>;

  /// \brief Constructors
  ///
  /// Initializes the underlying boolean value of the \c boolean_type.
  /// This constructor is a \c constexpr constructor.
  ///
  /// \post <tt>unwrap() == value</tt>.
  ///
  /// \param value The boolean value to initialize the \c boolean_type with.
  /// \throws Nothing.
  constexpr explicit boolean_type(const bool value) noexcept
      : base_type(value) {}

  constexpr explicit operator bool() const noexcept { return this->unwrap(); }

private:
  friend constexpr auto operator&&(const boolean_type lhs,
                                   const boolean_type rhs) noexcept
      -> boolean_type {
    return boolean_type(lhs.unwrap() && rhs.unwrap());
  }

  friend constexpr auto operator||(const boolean_type lhs,
                                   const boolean_type rhs) noexcept
      -> boolean_type {
    return boolean_type(lhs.unwrap() || rhs.unwrap());
  }

  friend constexpr auto operator!(const boolean_type value) noexcept
      -> boolean_type {
    return boolean_type(!value.unwrap());
  }
};

// --- Basic Arithmetic Types ---

/// \brief Strongly-type integral type.
///
/// Class template \c integral_type is a strongly typed integral type.
/// It provides all of the same functionality as built-in signed-integral
/// types except it cannot be instantiated from \c bool, \c char16_t, \c
/// char32_t, or
/// \c wchar_t and does not provide bitwise operations. Due to common practice
/// of \c std::int8_t being an alias for \c char, this class can be
/// instantiated from \c char. Use \c bitwise_integral_type for integral types
/// that should support bitwise operations.
///
/// \tparam Tag A unique tag type to distinguish different integral types.
/// \tparam T The underlying integral type.
template <typename Tag, cxx_arithmetic_integral T>
class signed_integral_type
    : public strong_type<Tag, T>,
      public three_way_comparison::skill<signed_integral_type<Tag, T>>,
      public output_stream::skill<signed_integral_type<Tag, T>>,
      public input_stream::skill<signed_integral_type<Tag, T>>,
      public addition::skill<signed_integral_type<Tag, T>>,
      public subtraction::skill<signed_integral_type<Tag, T>>,
      public multiplication::skill<signed_integral_type<Tag, T>>,
      public division::skill<signed_integral_type<Tag, T>>,
      public modulo::skill<signed_integral_type<Tag, T>>,
      public negation::skill<signed_integral_type<Tag, T>>,
      public increment::skill<signed_integral_type<Tag, T>>,
      public decrement::skill<signed_integral_type<Tag, T>> {
  using base_type = strong_type<Tag, T>;

public:
  template <typename NewTag> using rebind = signed_integral_type<NewTag, T>;

  /// \brief Constructor
  ///
  /// Constructs a \c integral_type from the specified value.
  ///
  /// \pre The conversion from \c U to \c T is non-narrowing.
  /// \post <tt>unwrap() == static_cast<T>(value)</tt>.
  ///
  /// \tparam U The type of the value to construct the \c integral_type from.
  /// \param value The value to construct the \c integral_type from.
  template <cxx_arithmetic_integral U>
    requires non_narrowing_integer_conversion<U, T>
  constexpr explicit signed_integral_type(const U value) noexcept
      : base_type(static_cast<T>(value)) {}
};

template <typename Tag, cxx_arithmetic_integral T>
class bitwise_signed_integral_type
    : public strong_type<Tag, T>,
      public three_way_comparison::skill<bitwise_signed_integral_type<Tag, T>>,
      public output_stream::skill<bitwise_signed_integral_type<Tag, T>>,
      public input_stream::skill<bitwise_signed_integral_type<Tag, T>>,
      public addition::skill<bitwise_signed_integral_type<Tag, T>>,
      public subtraction::skill<bitwise_signed_integral_type<Tag, T>>,
      public multiplication::skill<bitwise_signed_integral_type<Tag, T>>,
      public division::skill<bitwise_signed_integral_type<Tag, T>>,
      public modulo::skill<bitwise_signed_integral_type<Tag, T>>,
      public negation::skill<bitwise_signed_integral_type<Tag, T>>,
      public increment::skill<bitwise_signed_integral_type<Tag, T>>,
      public decrement::skill<bitwise_signed_integral_type<Tag, T>>,
      public bitwise_and::skill<bitwise_signed_integral_type<Tag, T>>,
      public bitwise_or::skill<bitwise_signed_integral_type<Tag, T>>,
      public bitwise_xor::skill<bitwise_signed_integral_type<Tag, T>>,
      public bitwise_not::skill<bitwise_signed_integral_type<Tag, T>>,
      public bitwise_shift::skill<bitwise_signed_integral_type<Tag, T>> {
  using base_type = strong_type<Tag, T>;

public:
  template <typename NewTag>
  using rebind = bitwise_signed_integral_type<NewTag, T>;

  template <typename U>
    requires non_narrowing_integer_conversion<U, T>
  constexpr explicit bitwise_signed_integral_type(const U value) noexcept
      : base_type(static_cast<T>(value)) {}
};

template <typename Tag, cxx_unsigned_integral T>
struct unsigned_integral_type
    : public strong_type<Tag, T>,
      public three_way_comparison::skill<signed_integral_type<Tag, T>>,
      public output_stream::skill<signed_integral_type<Tag, T>>,
      public input_stream::skill<signed_integral_type<Tag, T>>,
      public addition::skill<signed_integral_type<Tag, T>>,
      public subtraction::skill<signed_integral_type<Tag, T>>,
      public multiplication::skill<signed_integral_type<Tag, T>>,
      public division::skill<signed_integral_type<Tag, T>>,
      public modulo::skill<signed_integral_type<Tag, T>>,
      public negation::skill<signed_integral_type<Tag, T>>,
      public increment::skill<signed_integral_type<Tag, T>>,
      public decrement::skill<signed_integral_type<Tag, T>>,
      public bitwise_and::skill<unsigned_integral_type<Tag, T>>,
      public bitwise_or::skill<unsigned_integral_type<Tag, T>>,
      public bitwise_xor::skill<unsigned_integral_type<Tag, T>>,
      public bitwise_not::skill<unsigned_integral_type<Tag, T>>,
      public bitwise_shift::skill<unsigned_integral_type<Tag, T>> {
  using base_type = strong_type<Tag, T>;

public:
  template <typename NewTag> using rebind = unsigned_integral_type<NewTag, T>;

  template <typename U>
    requires non_narrowing_integer_conversion<U, T>
  constexpr explicit unsigned_integral_type(const U value) noexcept
      : base_type(static_cast<T>(value)) {}
};

template <typename Tag, std::floating_point T>
class floating_point_type
    : public strong_type<Tag, T>,
      public three_way_comparison::skill<floating_point_type<Tag, T>>,
      public output_stream::skill<floating_point_type<Tag, T>>,
      public input_stream::skill<floating_point_type<Tag, T>>,
      public addition::skill<floating_point_type<Tag, T>>,
      public subtraction::skill<floating_point_type<Tag, T>>,
      public multiplication::skill<floating_point_type<Tag, T>>,
      public division::skill<floating_point_type<Tag, T>>,
      public negation::skill<floating_point_type<Tag, T>>,
      public increment::skill<floating_point_type<Tag, T>>,
      public decrement::skill<floating_point_type<Tag, T>> {
  using base_type = strong_type<Tag, T>;

public:
  template <typename NewTag> using rebind = floating_point_type<NewTag, T>;

  template <typename U>
    requires non_narrowing_floating_point_conversion<U, T>
  constexpr explicit floating_point_type(const U value) noexcept
      : base_type(static_cast<T>(value)) {}

  constexpr auto
  is_approximately_equal(const floating_point_type /*other*/) const noexcept
      -> bool {
    return false;
  }

  /// \brief Returns true if two floating-point types have the same in-memory
  /// representation
  ///
  /// Compares the in-memory representation of the underlying floating-point
  /// values of \c this and \c other. This can be used to check for exact
  /// equality.
  /// \param other The other floating-point type to compare with.
  /// \return \c true if the in-memory representations are the same.
  constexpr auto
  is_same_representation(const floating_point_type other) const noexcept
      -> bool {
    const std::uint64_t this_representation =
        std::bit_cast<std::uint64_t>(this->unwrap());
    const std::uint64_t other_representation =
        std::bit_cast<std::uint64_t>(other.unwrap());
    return this_representation == other_representation;
  }

  /// \brief Returns true if \c this is NaN
  ///
  /// \return \c true \c this is NaN
  constexpr auto is_nan() const noexcept -> bool {
    return std::isnan(this->unwrap());
  }
};

// --- Bounded Integral Type ---
template <typename Tag, cxx_arithmetic_integral T, std::intmax_t Min,
          std::intmax_t Max>
class bounded_integral_type : public strong_type<Tag, T> {
  using base_type = strong_type<Tag, T>;
  static_assert(Min <= Max);
  static_assert(Max <= std::numeric_limits<T>::max() &&
                Max >= std::numeric_limits<T>::min());
  static_assert(Min >= std::numeric_limits<T>::min() &&
                Min <= std::numeric_limits<T>::max());

  template <typename U> constexpr static bool dependent_false = false;

public:
  template <typename NewTag>
  using rebind = bounded_integral_type<NewTag, T, Min, Max>;

  /// The minimum value that the specified \c bounded_integral_type can hold.
  constexpr static bounded_integral_type min{Min};
  /// The maximum value that the specified \c bounded_integral_type can hold.
  constexpr static bounded_integral_type max{Max};

  template <typename U>
  constexpr explicit bounded_integral_type(const U value)
      : base_type(static_cast<T>(value)) {
    constexpr U min_input_value = std::numeric_limits<U>::min();
    constexpr U max_input_value = std::numeric_limits<U>::max();

    static_assert(min_input_value < Max || max_input_value > Min,
                  "The range of the input type must not be completely "
                  "outside the bounds of the bounded_integral_type");

    if constexpr (min_input_value < Min) {
      if (value < Min) [[unlikely]] {
        if (std::is_constant_evaluated()) {
          throw out_of_range("Value is below minimum bound");
        } else {
          std::string message =
              std::format("Value {} is below minimum bound of {}", value, Min);
          throw out_of_range(message);
        }
      }
    }

    if constexpr (max_input_value > Max) {
      if (value > Max) [[unlikely]] {
        if (std::is_constant_evaluated()) {
          throw out_of_range("Value is above maximum bound");
        } else {
          std::string message =
              std::format("Value {} is above maximum bound of {}", value, Max);
          throw out_of_range(message);
        }
      }
    }
  }

  template <typename U, std::intmax_t OtherMin, std::intmax_t OtherMax>
  constexpr explicit bounded_integral_type(
      const bounded_integral_type<Tag, U, OtherMin, OtherMax> value)
      : base_type(value.unwrap()) {
    constexpr U min_input_value =
        bounded_integral_type<Tag, U, OtherMin, OtherMax>::min.unwrap();
    constexpr U max_input_value =
        bounded_integral_type<Tag, U, OtherMin, OtherMax>::max.unwrap();

    static_assert(min_input_value < Max || max_input_value > Min,
                  "The range of the input type must not be completely "
                  "outside the bounds of the bounded_integral_type");

    if constexpr (min_input_value < Min) {
      if (value.unwrap() < Min) [[unlikely]] {
        if (std::is_constant_evaluated()) {
          throw out_of_range("Value is below minimum bound");
        } else {
          std::string message =
              std::format("Value {} is below minimum bound of {}", value, Min);
          throw out_of_range(message);
        }
      }
    }

    if constexpr (max_input_value > Max) {
      if (value.unwrap() > Max) [[unlikely]] {
        if (std::is_constant_evaluated()) {
          throw out_of_range("Value is above maximum bound");
        } else {
          std::string message =
              std::format("Value {} is above maximum bound of {}", value, Max);
          throw out_of_range(message);
        }
      }
    }
  }

  template <typename U, std::intmax_t OtherMin, std::intmax_t OtherMax>
  constexpr auto
  operator=(const bounded_integral_type<Tag, U, OtherMin, OtherMax> value)
      -> bounded_integral_type& {
    bounded_integral_type temp(value);
    this->_m_do_not_use_only_public_to_make_usable_as_nttp = temp.unwrap();
    return *this;
  }
};

template <typename Tag, std::floating_point T>
class complex_type : public strong_type<Tag, std::complex<T>>,
                     public equality_comparison::skill<complex_type<Tag, T>>,
                     public output_stream::skill<complex_type<Tag, T>>,
                     public input_stream::skill<complex_type<Tag, T>>,
                     public addition::skill<complex_type<Tag, T>>,
                     public subtraction::skill<complex_type<Tag, T>>,
                     public multiplication::skill<complex_type<Tag, T>>,
                     public division::skill<complex_type<Tag, T>>,
                     public negation::skill<complex_type<Tag, T>> {
  using base_type = strong_type<Tag, std::complex<T>>;

  struct real_tag : Tag {};
  struct imag_tag : Tag {};

public:
  using real_part = floating_point_type<real_tag, T>;
  using imaginary_part = floating_point_type<imag_tag, T>;

  template <typename NewTag> using rebind = complex_type<NewTag, T>;

  template <typename U1, typename U2>
    requires non_narrowing_floating_point_conversion<U1, T> &&
             non_narrowing_floating_point_conversion<U2, T>
  constexpr complex_type(const U1 real, const U2 imag = T())
      : base_type(std::in_place, real, imag) {}

  constexpr complex_type(const real_part real,
                         const imaginary_part imag = imaginary_part{0.0})
      : base_type(std::in_place, real.unwrap(), imag.unwrap()) {}

  constexpr auto real() const -> real_part {
    return real_part{this->unwrap().real()};
  }

  constexpr auto imag() const -> imaginary_part {
    return imaginary_part{this->unwrap().imag()};
  }
};

// --- Pointer Types ---

template <typename Tag, cxx_nullable_pointer Pointer>
class pointer_type
    : public strong_type<Tag, Pointer>,
      public equality_comparison::skill<pointer_type<Tag, Pointer>>,
      public three_way_comparison::skill<pointer_type<Tag, Pointer>>,
      public output_stream::skill<pointer_type<Tag, Pointer>>,
      public dereference::skill<pointer_type<Tag, Pointer>> {
  using base_type = strong_type<Tag, Pointer>;

public:
  using pointer = Pointer;
  using element_type = std::pointer_traits<Pointer>::element_type;

  template <typename NewTag> using rebind = pointer_type<NewTag, Pointer>;

  constexpr pointer_type(std::nullptr_t) noexcept : base_type(nullptr) {}

  template <typename U>
    requires std::convertible_to<U*, Pointer>
  constexpr explicit pointer_type(U* const ptr) noexcept : base_type(ptr) {}

  constexpr explicit operator bool() const noexcept {
    return this->unwrap() != nullptr;
  }

  constexpr auto operator->() const noexcept -> pointer {
    return this->unwrap();
  }

private:
  friend constexpr auto operator==(std::nullptr_t /*lhs*/,
                                   const pointer_type rhs) -> bool {
    return rhs.unwrap() == nullptr;
  }

  friend constexpr auto operator==(const pointer_type lhs,
                                   std::nullptr_t /*rhs*/) -> bool {
    return lhs.unwrap() == nullptr;
  }

  friend constexpr auto operator!=(std::nullptr_t /*lhs*/,
                                   const pointer_type rhs) -> bool {
    return rhs.unwrap() != nullptr;
  }

  friend constexpr auto operator!=(const pointer_type lhs,
                                   std::nullptr_t /*rhs*/) -> bool {
    return lhs.unwrap() != nullptr;
  }
};

template <typename Tag, typename T, typename Deleter>
class unique_ptr_type
    : public strong_type<Tag, std::unique_ptr<T, Deleter>>,
      public equality_comparison::skill<unique_ptr_type<Tag, T, Deleter>>,
      public three_way_comparison::skill<unique_ptr_type<Tag, T, Deleter>>,
      public output_stream::skill<unique_ptr_type<Tag, T, Deleter>>,
      public dereference::skill<unique_ptr_type<Tag, T, Deleter>> {
  using base_type = strong_type<Tag, std::unique_ptr<T, Deleter>>;

public:
  using pointer =
      pointer_type<Tag, typename std::unique_ptr<T, Deleter>::pointer>;
  using element_type = std::unique_ptr<T, Deleter>::element_type;
  using deleter_type = Deleter;

  template <typename NewTag> using rebind = unique_ptr_type<NewTag, T, Deleter>;

  constexpr unique_ptr_type(std::nullptr_t) noexcept
      : base_type(std::in_place, nullptr) {}

  CINA_POINTER_CONSTEXPR explicit unique_ptr_type(T* const p) noexcept
      : base_type(std::in_place, p) {}

  CINA_POINTER_CONSTEXPR explicit unique_ptr_type(const pointer p) noexcept
      : base_type(std::in_place, p.unwrap()) {}

  CINA_POINTER_CONSTEXPR explicit unique_ptr_type(
      T* const p, const deleter_type& d) noexcept
    requires(!std::is_reference_v<deleter_type>)
      : base_type(std::in_place, p, d) {}

  CINA_POINTER_CONSTEXPR unique_ptr_type(const pointer p,
                                         const deleter_type& d) noexcept
    requires(!std::is_reference_v<deleter_type>)
      : base_type(std::in_place, p.unwrap(), d) {}

  CINA_POINTER_CONSTEXPR unique_ptr_type(const pointer p,
                                         deleter_type&& d) noexcept
    requires(!std::is_reference_v<deleter_type>)
      : base_type(std::in_place, p.unwrap(), std::move(d)) {}

  CINA_POINTER_CONSTEXPR auto reset(const pointer ptr) noexcept -> void {
    this->unwrap().reset(ptr.unwrap());
  }

  CINA_POINTER_CONSTEXPR auto release() noexcept -> pointer {
    return pointer(this->unwrap().release());
  }

  CINA_POINTER_CONSTEXPR auto swap(unique_ptr_type& other) noexcept -> void {
    this->unwrap().swap(other.unwrap());
  }

  CINA_POINTER_CONSTEXPR auto get() const -> pointer {
    return pointer(this->unwrap().get());
  }

  CINA_POINTER_CONSTEXPR auto get_deleter() noexcept -> deleter_type& {
    return this->unwrap().get_deleter();
  }

  CINA_POINTER_CONSTEXPR auto get_deleter() const noexcept
      -> const deleter_type& {
    return this->unwrap().get_deleter();
  }

  CINA_POINTER_CONSTEXPR explicit operator bool() const noexcept {
    return this->unwrap() != nullptr;
  }

  CINA_POINTER_CONSTEXPR auto operator->() const noexcept -> pointer {
    return pointer{this->unwrap().operator->()};
  }

private:
  friend CINA_POINTER_CONSTEXPR auto
  operator==(std::nullptr_t /*lhs*/, const unique_ptr_type rhs) noexcept
      -> bool {
    return rhs.unwrap() == nullptr;
  }

  friend CINA_POINTER_CONSTEXPR auto operator==(const unique_ptr_type lhs,
                                                std::nullptr_t /*rhs*/) noexcept
      -> bool {
    return lhs.unwrap() == nullptr;
  }

  friend CINA_POINTER_CONSTEXPR auto
  operator<=>(std::nullptr_t /*lhs*/, const unique_ptr_type rhs) noexcept {
    return rhs.unwrap() <=> nullptr;
  }

  friend CINA_POINTER_CONSTEXPR auto
  operator<=>(const unique_ptr_type lhs, std::nullptr_t /*rhs*/) noexcept {
    return lhs.unwrap() <=> nullptr;
  }
};

// --- Container Types ---
namespace _detail {
template <typename Itr, typename Tag> class const_iterator;

template <typename Itr, typename Tag> class iterator {
public:
  using difference_type = std::iterator_traits<Itr>::difference_type;
  using value_type = std::iterator_traits<Itr>::value_type;
  using pointer = std::iterator_traits<Itr>::pointer;
  using reference = std::iterator_traits<Itr>::reference;
  using iterator_category = std::iterator_traits<Itr>::iterator_category;

  constexpr explicit iterator(const Itr itr) : m_itr{itr} {}

  constexpr auto operator*() const -> reference { return *m_itr; }

  constexpr auto operator->() const -> pointer { return m_itr.operator->(); }

  constexpr auto operator++() -> iterator& {
    ++m_itr;
    return *this;
  }

  constexpr auto operator++(int) -> iterator {
    iterator temp{*this};
    ++(*this);
    return temp;
  }

private:
  friend constexpr auto operator==(const iterator lhs, const iterator rhs)
      -> bool {
    return lhs.m_itr == rhs.m_itr;
  }

  template <typename I, typename T> friend class const_iterator;

  Itr m_itr;
};

template <typename Itr, typename Tag> class const_iterator {
public:
  constexpr explicit const_iterator(const Itr itr) : m_itr{itr} {}

  constexpr const_iterator(const iterator<Itr, Tag> itr) : m_itr{itr.m_itr} {}

private:
  Itr m_itr;
};

template <typename C>
concept has_size = requires(C c) { c.size(); };
} // namespace _detail

template <typename Tag, cxx_container Container>
class container_type
    : public strong_type<Tag, Container>,
      public equality_comparison::skill<container_type<Tag, Container>>,
      public three_way_comparison::skill<container_type<Tag, Container>> {
public:
  using value_type = Container::value_type;
  using reference = Container::reference;
  using const_reference = Container::const_reference;
  using size_type = Container::size_type;
  using difference_type = Container::difference_type;
  using iterator = _detail::iterator<typename Container::iterator, Tag>;
  using const_iterator =
      _detail::const_iterator<typename Container::const_iterator, Tag>;

  constexpr auto begin() -> iterator {
    return iterator(
        this->_m_do_not_use_only_public_to_make_usable_as_nttp.begin());
  }

  constexpr auto begin() const -> const_iterator {
    return const_iterator(this->unwrap().begin());
  }

  constexpr auto cbegin() const -> const_iterator {
    return const_iterator(this->unwrap().cbegin());
  }

  constexpr auto end() -> iterator {
    return iterator(
        this->_m_do_not_use_only_public_to_make_usable_as_nttp.end());
  }

  constexpr auto end() const -> const_iterator {
    return const_iterator(this->unwrap().end());
  }

  constexpr auto cend() const -> const_iterator {
    return const_iterator(this->unwrap().cend());
  }

  constexpr auto swap(container_type& other) -> void {
    this->_m_do_not_use_only_public_to_make_usable_as_nttp.swap(
        other._m_do_not_use_only_public_to_make_usable_as_nttp);
  }

  constexpr auto size() const -> size_type
    requires _detail::has_size<Container>
  {
    return this->unwrap().size();
  }

  constexpr auto max_size() const -> size_type {
    return this->unwrap().max_size();
  }

  [[nodiscard]] constexpr auto empty() const -> bool {
    return this->unwrap().empty();
  }
};

template <typename Tag, cxx_allocator_aware_container Container>
class allocator_aware_container_type : public strong_type<Tag, Container> {
  using base_type = strong_type<Tag, Container>;

public:
  using allocator_type = typename Container::allocator_type;

  constexpr allocator_aware_container_type(const allocator_type& allocator)
      : base_type(std::in_place, allocator) {}

  constexpr allocator_aware_container_type(const Container& container,
                                           const allocator_type& allocator)
      : base_type(std::in_place, container, allocator) {}

  constexpr allocator_aware_container_type(Container&& container,
                                           const allocator_type& allocator)
      : base_type(std::in_place, std::move(container), allocator) {}

  constexpr allocator_aware_container_type(
      const allocator_aware_container_type& other,
      const allocator_type& allocator)
      : base_type(std::in_place, other.unwrap(), allocator) {}

  constexpr allocator_aware_container_type(
      allocator_aware_container_type&& other, const allocator_type& allocator)
      : base_type(std::in_place, std::move(other).unwrap(), allocator) {}

  constexpr auto get_allocator() const -> allocator_type {
    return this->unwrap().get_allocator();
  }
};

template <typename Tag, cxx_sequence_container Container>
class sequence_container_type : public strong_type<Tag, Container>,
                                public equality_comparison::skill<
                                    sequence_container_type<Tag, Container>>,
                                public three_way_comparison::skill<
                                    sequence_container_type<Tag, Container>> {
  using base_type = strong_type<Tag, Container>;

public:
  using value_type = Container::value_type;
  using reference = Container::reference;
  using const_reference = Container::const_reference;
  using size_type = Container::size_type;
  using difference_type = Container::difference_type;
  using iterator = _detail::iterator<typename Container::iterator, Tag>;
  using const_iterator =
      _detail::const_iterator<typename Container::const_iterator, Tag>;

  constexpr sequence_container_type() = default;

  constexpr explicit sequence_container_type(const Container& container)
      : base_type(container) {}

  constexpr explicit sequence_container_type(Container&& container)
      : base_type(std::move(container)) {}

  constexpr sequence_container_type(const size_type n, const value_type& value)
      : base_type(std::in_place, n, value) {}

  template <typename InputItr>
  constexpr sequence_container_type(InputItr first, InputItr last)
      : base_type(std::in_place, first, last) {}

  constexpr sequence_container_type(std::initializer_list<value_type> il)
      : base_type(std::in_place, il) {}

  constexpr auto begin() -> iterator {
    return iterator(
        this->_m_do_not_use_only_public_to_make_usable_as_nttp.begin());
  }

  constexpr auto begin() const -> const_iterator {
    return const_iterator(this->unwrap().begin());
  }

  constexpr auto cbegin() const -> const_iterator {
    return const_iterator(this->unwrap().cbegin());
  }

  constexpr auto end() -> iterator {
    return iterator(
        this->_m_do_not_use_only_public_to_make_usable_as_nttp.end());
  }

  constexpr auto end() const -> const_iterator {
    return const_iterator(this->unwrap().end());
  }

  constexpr auto cend() const -> const_iterator {
    return const_iterator(this->unwrap().cend());
  }

  constexpr auto swap(sequence_container_type& other) -> void {
    this->_m_do_not_use_only_public_to_make_usable_as_nttp.swap(
        other._m_do_not_use_only_public_to_make_usable_as_nttp);
  }

  constexpr auto size() const -> size_type
    requires _detail::has_size<Container>
  {
    return this->unwrap().size();
  }

  constexpr auto max_size() const -> size_type {
    return this->unwrap().max_size();
  }

  [[nodiscard]] constexpr auto empty() const -> bool {
    return this->unwrap().empty();
  }
};

// --- Type Factory ----

/// \brief Tag type to indicate no skills should be supported.
struct no_skills {};
/// \brief Tag to indicate signed integer type should have bitwise operations
struct enable_bitwise {};
/// \brief Tag to indicate an pointer type with unique ownership semantics.
struct owning_pointer {};
/// \brief Tag to indicate a range constraint for bounded integral types
/// \tparam Min The minimum value (inclusive) allowed for the integral type.
/// \tparam Max The maximum value (inclusive) allowed for the integral type.
template <std::intmax_t Min, std::intmax_t Max> struct range {};

/// \cond
namespace _detail {
template <typename Tag, typename T, typename... Args> struct new_type_impl {
  struct impl : strong_type<Tag, T>, Args::template skill<impl>... {
    using strong_type<Tag, T>::strong_type;
  };

  using type = impl;
};

template <typename Tag, typename T> struct new_type_impl<Tag, T> {
  using type = strong_type<Tag, T>;
};

template <typename Tag, typename T, typename... Args>
struct new_type_impl<Tag, T, no_skills, Args...> {
  using type = strong_type<Tag, T>;
};

template <typename Tag> struct new_type_impl<Tag, bool> {
  using type = boolean_type<Tag>;
};

template <typename Tag, cxx_arithmetic_integral T>
struct new_type_impl<Tag, T> {
  using type = signed_integral_type<Tag, T>;
};

template <typename Tag, cxx_arithmetic_integral T>
struct new_type_impl<Tag, T, enable_bitwise> {
  using type = bitwise_signed_integral_type<Tag, T>;
};

template <typename Tag, cxx_arithmetic_integral T, std::intmax_t Min,
          std::intmax_t Max>
struct new_type_impl<Tag, T, range<Min, Max>> {
  using type = bounded_integral_type<Tag, T, Min, Max>;
};

template <typename Tag, cxx_unsigned_integral T> struct new_type_impl<Tag, T> {
  using type = unsigned_integral_type<Tag, T>;
};

template <typename Tag, std::floating_point T> struct new_type_impl<Tag, T> {
  using type = floating_point_type<Tag, T>;
};

template <typename Tag> struct new_type_impl<Tag, std::complex<float>> {
  using type = complex_type<Tag, float>;
};

template <typename Tag> struct new_type_impl<Tag, std::complex<double>> {
  using type = complex_type<Tag, double>;
};

template <typename Tag> struct new_type_impl<Tag, std::complex<long double>> {
  using type = complex_type<Tag, long double>;
};

template <typename Tag, cxx_nullable_pointer Ptr>
struct new_type_impl<Tag, Ptr> {
  using type = pointer_type<Tag, Ptr>;
};

template <typename Tag, cxx_nullable_pointer Ptr>
struct new_type_impl<Tag, Ptr, owning_pointer> {
  using type = unique_ptr_type<Tag, std::remove_pointer_t<Ptr>>;
};

template <typename Tag, cxx_allocator_aware_container Container>
  requires cxx_sequence_container<Container>
class selected_container_type
    : public allocator_aware_container_type<Tag, Container>,
      public sequence_container_type<Tag, Container> {
public:
  template <typename NewTag>
  using rebind = selected_container_type<NewTag, Container>;
  using allocator_aware_container_type<Container,
                                       Tag>::allocator_aware_container;
  using sequence_container_type<Container, Tag>::sequence_container_type;
};

template <typename Tag, cxx_allocator_aware_container Container>
struct new_type_impl<Tag, Container> {
  using type = selected_container_type<Tag, Container>;
};

template <typename Tag, strong_type_like T> struct new_type_impl<Tag, T> {
  using type = T::template rebind<Tag>;
};
} // namespace _detail
/// \cond

template <typename Tag, typename... Args>
using new_type = typename _detail::new_type_impl<Tag, Args...>::type;

namespace _detail {
template <strong_type_like T, typename... Args> struct subtype_impl {
  template <typename... U> static constexpr bool dependent_false = false;

  static_assert(dependent_false<Args...>,
                "Invalid arguments for cina::subtype");
};

template <strong_type_like T> struct subtype_impl<T> {
  using type = T;
};

template <typename Tag, cxx_arithmetic_integral T, std::intmax_t Min,
          std::intmax_t Max>
struct subtype_impl<bounded_integral_type<Tag, T, Min, Max>> {
  using type = bounded_integral_type<Tag, T, Min, Max>;
};
} // namespace _detail

template <strong_type_like T, typename... Args>
using subtype = _detail::subtype_impl<T, Args...>::type;

// --- Arithmetic Functions ----

template <arithmetic T> CINA_BASIC_CMATH_CONSTEXPR auto abs(const T x) -> T {
  return T{std::abs(x.unwrap())};
}

template <complex T> auto abs(const T& z) -> typename T::real_part {
  return typename T::real_part{std::abs(z.unwrap())};
}

template <arithmetic T> constexpr auto real(const T x) -> T { return x; }

template <complex T> auto real(const T& z) -> typename T::real_part {
  return z.real();
}

template <arithmetic T> constexpr auto imag(const T /*x*/) -> T { return T{0}; }

template <complex T> auto imag(const T& z) -> typename T::imaginary_part {
  return z.imag();
}

template <arithmetic T>
CINA_BASIC_CMATH_CONSTEXPR auto fmod(const T x, const T y) -> T {
  return T{std::fmod(x.unwrap(), y.unwrap())};
}

template <arithmetic T>
CINA_BASIC_CMATH_CONSTEXPR auto remainder(const T x, const T y) -> T {
  return T{std::remainder(x.unwrap(), y.unwrap())};
}

template <arithmetic T>
CINA_BASIC_CMATH_CONSTEXPR auto remquo(const T x, const T y, int* quo) -> T {
  return T{std::remquo(x.unwrap(), y.unwrap(), quo)};
}

template <arithmetic Res>
CINA_BASIC_CMATH_CONSTEXPR auto fma(const arithmetic auto x,
                                    const arithmetic auto y,
                                    const arithmetic auto z) -> Res {
  return Res{std::fma(x.unwrap(), y.unwrap(), z.unwrap())};
}

template <arithmetic T>
CINA_BASIC_CMATH_CONSTEXPR auto fmax(const T x, const T y) -> T {
  return T{std::fmax(x.unwrap(), y.unwrap())};
}

template <arithmetic T>
CINA_BASIC_CMATH_CONSTEXPR auto fmin(const T x, const T y) -> T {
  return T{std::fmin(x.unwrap(), y.unwrap())};
}

template <typename T>
CINA_BASIC_CMATH_CONSTEXPR auto fdim(const T x, const T y) -> T {
  return T{std::fdim(x.unwrap(), y.unwrap())};
}

template <floating_point T> auto nan(const char* arg) -> T {
  return T{std::nan(arg)};
}

template <arithmetic T>
constexpr auto lerp(const T a, const T b, const T t) noexcept -> T {
  return std::lerp(a.unwrap(), b.unwrap(), t.unwrap());
}

template <arithmetic T> CINA_CMATH_CONSTEXPR auto exp(const T x) -> T {
  return T{std::exp(x.unwrap())};
}

template <arithmetic T> CINA_CMATH_CONSTEXPR auto exp2(T x) -> T {
  return T{std::exp2(x.unwrap())};
}

template <arithmetic T> CINA_CMATH_CONSTEXPR auto expm1(T x) -> T {
  return T{std::expm1(x.unwrap())};
}

template <arithmetic T> CINA_CMATH_CONSTEXPR auto log(const T x) -> T {
  return T{std::log(x.unwrap())};
}

template <arithmetic T> CINA_CMATH_CONSTEXPR auto log10(const T x) -> T {
  return T{std::log10(x.unwrap())};
}

template <arithmetic T> CINA_CMATH_CONSTEXPR auto log2(const T x) -> T {
  return T{std::log2(x.unwrap())};
}

template <arithmetic Exp>
CINA_CMATH_CONSTEXPR auto pow(const arithmetic auto base,
                              const arithmetic auto power) -> Exp {
  return Exp{std::pow(base.unwrap(), power.unwrap())};
}

template <arithmetic T> CINA_CMATH_CONSTEXPR auto sqrt(const T x) -> T {
  return T{std::sqrt(x.unwrap())};
}

template <arithmetic T> CINA_CMATH_CONSTEXPR auto cbrt(const T x) -> T {
  return T{std::cbrt(x.unwrap())};
}

template <typename Res>
CINA_CMATH_CONSTEXPR auto hypot(const arithmetic auto x,
                                const arithmetic auto y) -> Res {
  return Res{std::hypot(x.unwrap(), y.unwrap())};
}

template <typename Res>
CINA_CMATH_CONSTEXPR auto hypot(const arithmetic auto x,
                                const arithmetic auto y,
                                const arithmetic auto z) -> Res {
  return Res{std::hypot(x.unwrap(), y.unwrap(), z.unwrap())};
}

} // namespace cina

// --- Standard Library Specializations ---
template <cina::strong_type_like T> struct std::hash<T> {
  constexpr auto operator()(const T& value) const noexcept -> std::size_t
    requires cina::cxx_hashable<cina::underlying_type<T>>
  {
    return std::hash<cina::underlying_type<T>>{}(value.unwrap());
  }
};

template <cina::strong_type_like T>
  requires std::formattable<cina::underlying_type<T>, char>
struct std::formatter<T> : std::formatter<std::string_view> {
  constexpr auto parse(std::format_parse_context& ctx) const {
    return ctx.begin();
  }

  constexpr auto format(const T& value, std::format_context& ctx) const {
    std::string out;
    std::format_to(std::back_inserter(out), "{}", value.unwrap());
    return std::formatter<std::string_view>::format(out, ctx);
  }
};

template <cina::arithmetic T> struct std::numeric_limits<T> {
private:
  using base_type = std::numeric_limits<cina::underlying_type<T>>;

public:
  constexpr static bool is_specialized = true;
  constexpr static bool is_signed = base_type::is_signed;
  constexpr static bool is_integer = base_type::is_integer;
  constexpr static bool is_exact = base_type::is_exact;
  constexpr static bool has_infinity = base_type::has_infinity;
  constexpr static bool has_quiet_NaN = base_type::has_quiet_NaN;
  constexpr static bool has_signaling_NaN = base_type::has_signaling_NaN;
  [[deprecated]] constexpr static std::float_denorm_style has_denorm =
      base_type::has_denorm;
  constexpr static bool has_denorm_loss = base_type::has_denorm_loss;
  constexpr static std::float_round_style round_style = base_type::round_style;
  constexpr static bool is_iec559 = base_type::is_iec559;
  constexpr static bool is_bounded = base_type::is_bounded;
  constexpr static bool is_modulo = base_type::is_modulo;
  constexpr static int digits = base_type::digits;
  constexpr static int digits10 = base_type::digits10;
  constexpr static int max_digits10 = base_type::max_digits10;
  constexpr static int radix = base_type::radix;
  constexpr static int min_exponent = base_type::min_exponent;
  constexpr static int min_exponent10 = base_type::min_exponent10;
  constexpr static int max_exponent = base_type::max_exponent;
  constexpr static int max_exponent10 = base_type::max_exponent10;
  constexpr static bool traps = base_type::traps;
  constexpr static bool tinyness_before = base_type::tinyness_before;

  constexpr static T min() noexcept { return T(base_type::min()); }
  constexpr static T max() noexcept { return T(base_type::max()); }
  constexpr static T lowest() noexcept { return T(base_type::lowest()); }
  constexpr static T epsilon() noexcept { return T(base_type::epsilon()); }
  constexpr static T round_error() noexcept {
    return T(base_type::round_error());
  }
  constexpr static T infinity() noexcept { return T(base_type::infinity()); }
  constexpr static T quiet_NaN() noexcept { return T(base_type::quiet_NaN()); }
  constexpr static T signaling_NaN() noexcept {
    return T(base_type::signaling_NaN());
  }
  constexpr static T denorm_min() noexcept {
    return T(base_type::denorm_min());
  }
};

template <cina::bounded_integral T> struct std::numeric_limits<T> {
private:
  using base_type = std::numeric_limits<T>;
  constexpr static bool is_specialized = true;
  constexpr static bool is_signed = base_type::is_signed;
  constexpr static bool is_integer = base_type::is_integer;
  constexpr static bool is_exact = base_type::is_exact;
  constexpr static bool has_infinity = base_type::has_infinity;
  constexpr static bool has_quiet_NaN = base_type::has_quiet_NaN;
  constexpr static bool has_signaling_NaN = base_type::has_signaling_NaN;
  [[deprecated]] constexpr static std::float_denorm_style has_denorm =
      base_type::has_denorm;
  constexpr static bool has_denorm_loss = base_type::has_denorm_loss;
  constexpr static std::float_round_style round_style = base_type::round_style;
  constexpr static bool is_iec559 = base_type::is_iec559;
  constexpr static bool is_bounded = base_type::is_bounded;
  constexpr static bool is_modulo = base_type::is_modulo;
  constexpr static int digits = base_type::digits;
  constexpr static int digits10 = base_type::digits10;
  constexpr static int max_digits10 = base_type::max_digits10;
  constexpr static int radix = base_type::radix;
  constexpr static int min_exponent = base_type::min_exponent;
  constexpr static int min_exponent10 = base_type::min_exponent10;
  constexpr static int max_exponent = base_type::max_exponent;
  constexpr static int max_exponent10 = base_type::max_exponent10;
  constexpr static bool traps = base_type::traps;
  constexpr static bool tinyness_before = base_type::tinyness_before;

  constexpr static T min() noexcept { return T::min; }
  constexpr static T max() noexcept { return T::max; }
  constexpr static T lowest() noexcept { return T::min; }
  constexpr static T epsilon() noexcept { return T::min; }
  constexpr static T round_error() noexcept { return T::min; }
  constexpr static T infinity() noexcept { return T::min; }
  constexpr static T quiet_NaN() noexcept { return T::min; }
  constexpr static T signaling_NaN() noexcept { return T::min; }
  constexpr static T denorm_min() noexcept { return T::min; }
};

#endif