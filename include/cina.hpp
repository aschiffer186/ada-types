#ifndef CINA_HPP
#define CINA_HPP

// Design goals:
//  1) Prevent accidental mixing of different types (similar to Ada).
//  2) Minimal runtime overhead (zero-cost abstractions).
//  3) Eliminate unfriendly C++-isms
//  4) Add as much information into pre-condtions of types as possible.

#include <concepts>
#include <format>
#include <limits>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace cina {

// --- Traits and Concepts ---
// --- C++ Concepts ---
template <typename T>
concept cxx_streamable = requires(T a, std::ostream& os) {
  { os << a } -> std::same_as<std::ostream&>;
};

template <typename T>
concept cxx_hashable = requires(T a) {
  { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
};

template <typename T>
concept cxx_arithmetic_integral =
    std::signed_integral<T> && !std::same_as<T, bool> &&
    !std::same_as<T, char16_t> && !std::same_as<T, char32_t> &&
    !std::same_as<T, wchar_t>;

template <typename T>
concept cxx_unsigned_integral = std::unsigned_integral<T>;

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

/// \brief Concept indicating that a conversion from \c From to \c To is
/// non-narrowing.
///
/// Concept indicating that a conversion from \c From to \c To is non-narrowing.
/// This concept is only modeled if:
/// 1. Both \c From and \c To are integral types of the same signedness, and
///    the size of \c From is less than or equal to the size of \c To.
/// 2. \c From is an integral type and \c To is a floating-point type.
/// 3. Both \c From and \c To are floating-point types, and the size of \c From
///    is less than or equal to the size of \c To
template <typename From, typename To>
concept non_narrowing_conversion =
    (std::integral<From> && std::integral<To> && sizeof(From) <= sizeof(To) &&
     std::is_signed_v<From> == std::is_signed_v<To>) ||
    (std::integral<From> && std::floating_point<To>) ||
    (std::floating_point<From> && std::floating_point<To> &&
     sizeof(From) <= sizeof(To));

// --- Forward Declarations ---

template <typename Tag, typename UnderlyingType> class strong_type;

template <typename Tag, cxx_arithmetic_integral T> class integral_type;

template <typename Tag, cxx_arithmetic_integral T> class bitwise_integral_type;

template <typename Tag, std::floating_point T> class floating_point_type;

template <typename Tag, cxx_arithmetic_integral T, std::intmax_t Min,
          std::intmax_t Max>
class bounded_integral_type;

template <typename Tag, cxx_nullable_pointer Pointer> class pointer_type;

// --- Cina Concepts ---

namespace _detail {
template <typename Tag, typename UnderlyingType>
auto _strong_type_base(strong_type<Tag, UnderlyingType>)
    -> strong_type<Tag, UnderlyingType>;

template <typename T>
using _strong_type_base_t = decltype(_strong_type_base(std::declval<T>()));

template <typename, typename = void> constexpr bool is_strong_type_impl = false;

template <typename T>
constexpr bool is_strong_type_impl<T, std::void_t<_strong_type_base_t<T>>> =
    true;

template <typename Tag, typename UnderlyingType>
auto _get_underlying_type(strong_type<Tag, UnderlyingType>) -> UnderlyingType;

} // namespace _detail

template <typename T>
concept strong_type_like = _detail::is_strong_type_impl<std::remove_cvref_t<T>>;

template <strong_type_like T>
using underlying_type =
    decltype(_detail::_get_underlying_type(std::declval<T>()));

// Custom Exceptions
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

template <typename Derived> struct equality_comparison {
  friend constexpr auto operator==(const Derived& lhs, const Derived& rhs)
      -> bool
    requires std::equality_comparable<underlying_type<Derived>>
  {
    return lhs.unwrap() == rhs.unwrap();
  }
};

template <typename Derived> struct three_way_comparison {
  friend constexpr auto operator<=>(const Derived& lhs, const Derived& rhs)
    requires std::three_way_comparable<underlying_type<Derived>>
  {
    return lhs.unwrap() <=> rhs.unwrap();
  }
};

template <typename Derived> struct less {
  friend constexpr auto operator<(const Derived& lhs,
                                  const Derived& rhs) noexcept -> bool {
    return lhs.unwrap() < rhs.unwrap();
  }
};

namespace _detail {
template <typename> constexpr bool is_int8_type = false;

template <typename Tag>
constexpr bool is_int8_type<integral_type<Tag, std::int8_t>> = true;

template <typename Tag>
constexpr bool is_int8_type<bitwise_integral_type<Tag, std::int8_t>> = true;
} // namespace _detail

template <typename Derived> struct output_stream {
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

template <typename Derived> struct addition {
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

template <typename Derived> struct subtraction {
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

template <typename Derived> struct multiplication {
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

template <typename Derived> struct division {
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

template <typename Derived> struct modulo {
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

template <typename Derived> struct negation {
  static_assert(strong_type_like<Derived>);

  friend constexpr auto operator-(const Derived& value) noexcept -> Derived {
    return Derived(-value.unwrap());
  }
};

template <typename Derived> struct increment {
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

template <typename Derived> struct decrement {
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

template <typename Derived> struct bitwise_and {
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

template <typename Derived> struct bitwise_or {
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

template <typename Derived> struct bitwise_xor {
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

template <typename Derived> struct bitwise_not {
  friend constexpr auto operator~(const Derived& value) noexcept -> Derived {
    return Derived(~value.unwrap());
  }
};

template <typename Derived> struct bitwise_shift {
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
    return Derived(lhs.unwrap() >> rhs);
  }

  friend constexpr auto operator>>=(Derived& lhs, const Derived& rhs) noexcept
      -> Derived& {
    lhs = lhs >> rhs;
    return lhs;
  }
};

template <typename Tag, typename UnderlyingType>
class strong_type : equality_comparison<strong_type<Tag, UnderlyingType>> {
public:
  /// Type alias for the underlying type.
  using underlying_type = UnderlyingType;
  /// Type alias for the tag type.
  using tag = Tag;

  /// \brief Default constructor.
  ///
  /// Default initializes the underlying value of the \c strong_type.
  /// This constructor is a \c constexpr constructor if the default constructor
  /// of \c UnderlyingType is a \c constexpr constructor.
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
  /// \pre <tt>std::constructible_from<UnderlyingType, U></tt> is modeled and \c
  /// U is not a specialization of \c strong_type.
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
      : _m_dont_use(std::forward<U>(value)) {}

  /// \brief Constructor
  ///
  /// Initializes the underlying value of the \c strong_type in-place with the
  /// given arguments.
  /// This constructor is a \c constexpr constructor if the selected
  /// constructor of \c UnderlyingType is a \c constexpr constructor.
  ///
  /// \pre <tt>std::constructible_from<UnderlyingType, Args...></tt> is modeled.
  /// \post \c unwrap() returns a value equivalent to \c
  /// UnderlyingType(std::forward<Args>(args)...).
  ///
  /// \tparam Args The types of the arguments to initialize the underlying value
  /// with.
  /// \param args The arguments to initialize the underlying value with.
  /// \throws Any exceptions thrown by the selected constructor of \c
  /// UnderlyingType.
  template <typename... Args>
    requires std::constructible_from<UnderlyingType, Args...>
  constexpr explicit strong_type(std::in_place_t, Args&&... args) noexcept(
      std::is_nothrow_constructible_v<UnderlyingType, Args...>)
      : _m_dont_use(std::forward<Args>(args)...) {}

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
  /// \tparam Args The types of the arguments to initialize the underlying value
  /// with.
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
      : _m_dont_use(il, std::forward<Args>(args)...) {}

  /// \brief Returns a reference to the underlying value.
  ///
  /// \return A const-reference to the underlying value.
  constexpr auto unwrap() const& noexcept -> const UnderlyingType& {
    return _m_dont_use;
  }

  // Should only be used by Cina, not by users directly.
  // Use unwrap() instead!.
  UnderlyingType _m_dont_use{};
};

// --- Boolean Type ---

/// \brief Strongly-type boolean value.
///
/// Class template \c boolean_type is a strongly typed boolean value.
/// This type provides additional safety over the built-in \c bool by not being
/// implicitly convertible to integral types and not providing overloads of any
/// arithmetic operations.
/// \tparam Tag A unique tag type to distinguish different boolean types.
template <typename Tag>
class boolean_type : public strong_type<Tag, bool>,
                     equality_comparison<boolean_type<Tag>>,
                     less<boolean_type<Tag>>,
                     output_stream<boolean_type<Tag>> {

  using base_type = strong_type<Tag, bool>;

public:
  /// \brief Constructor
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

  constexpr explicit operator bool() const noexcept {
    return this->_m_dont_use;
  }

private:
  friend constexpr auto operator&&(const boolean_type lhs,
                                   const boolean_type rhs) noexcept
      -> boolean_type {
    return boolean_type(lhs._m_dont_use && rhs._m_dont_use);
  }

  friend constexpr auto operator||(const boolean_type lhs,
                                   const boolean_type rhs) noexcept
      -> boolean_type {
    return boolean_type(lhs._m_dont_use || rhs._m_dont_use);
  }

  friend constexpr auto operator!(const boolean_type value) noexcept
      -> boolean_type {
    return boolean_type(!value._m_dont_use);
  }
};

// --- Basic Arithmetic Types ---

template <typename Tag, cxx_arithmetic_integral T>
class integral_type : strong_type<Tag, T>,
                      equality_comparison<integral_type<Tag, T>>,
                      three_way_comparison<integral_type<Tag, T>>,
                      output_stream<integral_type<Tag, T>>,
                      addition<integral_type<Tag, T>>,
                      subtraction<integral_type<Tag, T>>,
                      multiplication<integral_type<Tag, T>>,
                      division<integral_type<Tag, T>>,
                      modulo<integral_type<Tag, T>>,
                      negation<integral_type<Tag, T>>,
                      increment<integral_type<Tag, T>>,
                      decrement<integral_type<Tag, T>> {
  using base_type = strong_type<Tag, T>;

public:
  template <typename U>
    requires non_narrowing_conversion<U, T>
  constexpr explicit integral_type(const U value) noexcept
      : base_type(static_cast<T>(value)) {}
};

template <typename Tag, cxx_arithmetic_integral T>
class bitwise_integral_type
    : public strong_type<Tag, T>,
      equality_comparison<bitwise_integral_type<Tag, T>>,
      three_way_comparison<bitwise_integral_type<Tag, T>>,
      output_stream<bitwise_integral_type<Tag, T>>,
      addition<bitwise_integral_type<Tag, T>>,
      subtraction<bitwise_integral_type<Tag, T>>,
      multiplication<bitwise_integral_type<Tag, T>>,
      division<bitwise_integral_type<Tag, T>>,
      modulo<bitwise_integral_type<Tag, T>>,
      negation<bitwise_integral_type<Tag, T>>,
      increment<bitwise_integral_type<Tag, T>>,
      decrement<bitwise_integral_type<Tag, T>>,
      bitwise_and<bitwise_integral_type<Tag, T>>,
      bitwise_or<bitwise_integral_type<Tag, T>>,
      bitwise_xor<bitwise_integral_type<Tag, T>>,
      bitwise_not<bitwise_integral_type<Tag, T>>,
      bitwise_shift<bitwise_integral_type<Tag, T>> {
  using base_type = strong_type<Tag, T>;

public:
  template <typename U>
    requires non_narrowing_conversion<U, T>
  constexpr explicit bitwise_integral_type(const U value) noexcept
      : base_type(static_cast<T>(value)) {}
};

template <typename Tag, cxx_unsigned_integral T>
struct unsigned_integral_type : public strong_type<Tag, T>,
                                equality_comparison<integral_type<Tag, T>>,
                                three_way_comparison<integral_type<Tag, T>>,
                                output_stream<integral_type<Tag, T>>,
                                addition<integral_type<Tag, T>>,
                                subtraction<integral_type<Tag, T>>,
                                multiplication<integral_type<Tag, T>>,
                                division<integral_type<Tag, T>>,
                                modulo<integral_type<Tag, T>>,
                                negation<integral_type<Tag, T>>,
                                increment<integral_type<Tag, T>>,
                                decrement<integral_type<Tag, T>>,
                                bitwise_and<unsigned_integral_type<Tag, T>>,
                                bitwise_or<unsigned_integral_type<Tag, T>>,
                                bitwise_xor<unsigned_integral_type<Tag, T>>,
                                bitwise_not<unsigned_integral_type<Tag, T>>,
                                bitwise_shift<unsigned_integral_type<Tag, T>> {
  using base_type = strong_type<Tag, T>;

public:
  template <typename U>
    requires non_narrowing_conversion<U, T>
  constexpr explicit unsigned_integral_type(const U value) noexcept
      : base_type(static_cast<T>(value)) {}
};

template <typename Tag, std::floating_point T>
class floating_point_type : strong_type<Tag, T>,
                            equality_comparison<floating_point_type<Tag, T>>,
                            three_way_comparison<floating_point_type<Tag, T>>,
                            output_stream<floating_point_type<Tag, T>>,
                            addition<floating_point_type<Tag, T>>,
                            subtraction<floating_point_type<Tag, T>>,
                            multiplication<floating_point_type<Tag, T>>,
                            division<floating_point_type<Tag, T>>,
                            negation<floating_point_type<Tag, T>>,
                            increment<floating_point_type<Tag, T>>,
                            decrement<floating_point_type<Tag, T>> {
  using base_type = strong_type<Tag, T>;

public:
  template <typename U>
    requires non_narrowing_conversion<U, T>
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

private:
  friend constexpr auto operator==(const floating_point_type /*lhs*/,
                                   const floating_point_type /*rhs*/) noexcept
      -> bool = delete;
};

// --- Bounded Integral Type ---
template <typename Tag, cxx_arithmetic_integral T, std::intmax_t Min,
          std::intmax_t Max>
class bounded_integral_type : public strong_type<Tag, T> {
  using base_type = strong_type<Tag, T>;
  static_assert(Min <= Max);

  template <typename U> constexpr static bool dependent_false = false;

public:
  template <typename U>
  constexpr explicit bounded_integral_type(const U value)
      : base_type(static_cast<T>(value)) {
    constexpr U min_input_value = std::numeric_limits<U>::min();
    constexpr U max_input_value = std::numeric_limits<U>::max();

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
      : base_type(value.unwrap()) {}
};

// --- Pointer Types ---
template <typename Tag, cxx_nullable_pointer Pointer>
class pointer_type : public strong_type<Tag, Pointer>,
                     equality_comparison<pointer_type<Tag, Pointer>>,
                     three_way_comparison<pointer_type<Tag, Pointer>>,
                     output_stream<pointer_type<Tag, Pointer>> {
  using base_type = strong_type<Tag, Pointer>;

public:
  using pointer = Pointer;
  using element_type = std::pointer_traits<Pointer>::element_type;

  constexpr pointer_type(std::nullptr_t) noexcept : base_type(nullptr) {}

  template <typename U>
    requires std::convertible_to<U, Pointer>
  constexpr explicit pointer_type(U ptr) noexcept : base_type(ptr) {}

  constexpr explicit operator bool() const noexcept {
    return this->_m_dont_use != nullptr;
  }

  constexpr auto operator*() const
      -> std::add_lvalue_reference_t<element_type> {
    return *this->_m_dont_use;
  }

  constexpr auto operator->() const noexcept -> pointer {
    return this->_m_dont_use;
  }

private:
  friend constexpr auto operator==(std::nullptr_t /*lhs*/,
                                   const pointer_type rhs) -> bool {
    return rhs._m_dont_use == nullptr;
  }

  friend constexpr auto operator==(const pointer_type lhs,
                                   std::nullptr_t /*rhs*/) -> bool {
    return lhs._m_dont_use == nullptr;
  }

  friend constexpr auto operator!=(std::nullptr_t /*lhs*/,
                                   const pointer_type rhs) -> bool {
    return rhs._m_dont_use != nullptr;
  }

  friend constexpr auto operator!=(const pointer_type lhs,
                                   std::nullptr_t /*rhs*/) -> bool {
    return lhs._m_dont_use != nullptr;
  }
};

template <typename Tag, typename T, typename Deleter = std::default_delete<T>>
class unique_ptr_type : public strong_type<Tag, std::unique_ptr<T, Deleter>>,
                        equality_comparison<unique_ptr_type<Tag, T, Deleter>>,
                        three_way_comparison<unique_ptr_type<Tag, T, Deleter>>,
                        output_stream<unique_ptr_type<Tag, T, Deleter>> {
  using base_type = strong_type<Tag, std::unique_ptr<T, Deleter>>;

public:
  using pointer = std::unique_ptr<T, Deleter>::pointer;
  using element_type = std::unique_ptr<T, Deleter>::element_type;
  using deleter_type = Deleter;

  constexpr unique_ptr_type(std::nullptr_t) noexcept
      : base_type(std::in_place, nullptr) {}

  explicit unique_ptr_type(pointer p) noexcept : base_type(std::in_place, p) {}

  unique_ptr_type(pointer p, const deleter_type& d) noexcept
    requires(!std::is_reference_v<deleter_type>)
      : base_type(std::in_place, p, d) {}

  unique_ptr_type(pointer p, deleter_type&& d) noexcept
    requires(!std::is_reference_v<deleter_type>)
      : base_type(std::in_place, p, std::move(d)) {}
};

// --- Type Factory ----

/// \brief Tag type to indicate no skills should be supported.
struct no_skills {};
/// \brief Tag to indicate signed integer type should have bitwise operations
struct enable_bitwise {};
/// \brief Tag to indicate a pointer like type
struct as_pointer {};
/// \brief Tag to indicate an pointer type with unique ownership semantics.
struct owning_pointer {};
/// \brief Tag to indicate a range constraint for bounded integral types
/// \tparam Min The minimum value (inclusive) allowed for the integral type.
/// \tparam Max The maximum value (inclusive) allowed for the integral type.
template <std::intmax_t Min, std::intmax_t Max> struct range {};

namespace _detail {
template <typename Tag, typename T, typename... Args> struct new_type_impl {
  template <typename U> static constexpr bool dependent_false = false;

  static_assert(dependent_false<Tag>,
                "Invalid combination of type arguments for cina::new_type");
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
  using type = integral_type<Tag, T>;
};

template <typename Tag, cxx_arithmetic_integral T>
struct new_type_impl<Tag, T, enable_bitwise> {
  using type = bitwise_integral_type<Tag, T>;
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

template <typename Tag, typename Ptr>
  requires std::is_pointer_v<Ptr>
struct new_type_impl<Tag, Ptr> {
  using type = pointer_type<Tag, Ptr>;
};

template <typename Tag, typename T> struct new_type_impl<Tag, T, as_pointer> {
  using type = pointer_type<Tag, T>;
};

template <typename Tag, typename Ptr>
  requires std::is_pointer_v<Ptr>
struct new_type_impl<Tag, Ptr, owning_pointer> {
  using type = unique_ptr_type<Tag, std::remove_pointer_t<Ptr>>;
};
} // namespace _detail

template <typename Tag, typename T, typename... Args>
using new_type = typename _detail::new_type_impl<Tag, T, Args...>::type;
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

#endif