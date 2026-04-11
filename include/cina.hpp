#ifndef CINA_HPP
#define CINA_HPP

#if defined(_MSC_VER) && _MSC_VER >= 1910
#define CINA_EBCO __declspec(empty_bases)
#else
#define CINA_EBCO
#endif

#include <algorithm> // ranges::copy
#include <concepts>  // constructible_from, default_initializable, same_as
#include <cstddef>   // size_t
#include <initializer_list> // initializer_list
#include <type_traits> // is_nothrow_constructible, is_nothrow_copy_constructible, is_nothrow_move_constructible, remove_cvref_t, remove_reference_t, void_t
#include <utility>     // declval, forward

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
template <class T>
concept cxx_boolean = std::same_as<std::remove_cvref_t<T>, bool>;

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

/// --- Cina Concepts and Type Traits ----
namespace _detail {
template <static_string Tag, class T>
auto _as_strong_type(strong_type<Tag, T>) -> strong_type<Tag, T>;

template <class T, class = void> constexpr inline bool _is_strong_type = false;

template <class T>
constexpr inline bool _is_strong_type<
    T, std::void_t<decltype(_as_strong_type(std::declval<T>()))>> = true;

template <static_string Tag, class T>
auto _as_underlying_type(strong_type<Tag, T>) -> T;
} // namespace _detail

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

/// \brief Skill representing less-than comparison.
///
/// Deriving from the skill adds the ability to compare two instances of the
/// same type using the less-than operator. This allows types to model the \c
/// std::totally_ordered concept.
struct less {
  template <class Derived> struct skill {
    friend constexpr auto operator<(const Derived& lhs,
                                    const Derived& rhs) noexcept -> bool {
      return lhs.unwrap() < rhs.unwrap();
    }
  };
}

// --- Strong Type Definition ---
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
                                         T, std::initializer_list<U>&,
                                         Args&&...>)
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
    const T* _m_do_not_use_directly;

    template <class U = T>
    constexpr _strong_type_storage(const U& value) noexcept
        : _m_do_not_use_directly(&value) {}

    constexpr auto get() const noexcept -> const std::remove_reference_t<T>& {
      return *_m_do_not_use_directly;
    }
  };
} // namespace _detail

template <static_string Tag, class T> class CINA_EBCO strong_type {
public:
  template <static_string OtherTag> using rebind = strong_type<OtherTag, T>;

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
  constexpr strong_type(const T& value) noexcept(
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
  constexpr strong_type(T&& value) noexcept(
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

template <static_string Tag, cxx_boolean T>
class boolean_type : public strong_type<Tag, T>,
                     public equality_comparison::skill<boolean_type<Tag, T>> {
  using base_type = strong_type<Tag, T>;

public:
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

  /// \cond
  constexpr boolean_type(const T&&)
    requires std::is_reference_v<T>
  = delete;
  /// \endcond

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
} // namespace cina

#endif