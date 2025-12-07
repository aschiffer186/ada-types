#ifndef CONCEPTS_HPP
#define CONCEPTS_HPP

#include <format>
#include <type_traits>

#include "ada_types_fwd.hpp"

namespace ada_types {
namespace _detail {
template <typename T, string_literal Tag>
auto strong_type_base(const strong_type<T, Tag>&) -> strong_type<T, Tag>;

template <typename T>
using strong_type_base_t = decltype(strong_type_base(std::declval<T>()));

template <typename, typename = void> struct is_strong_type : std::false_type {};

template <typename T>
struct is_strong_type<T, std::void_t<T>> : std::true_type {};
} // namespace _detail

template <typename T>
concept strong_type_like =
    _detail::is_strong_type<std::remove_cvref_t<T>>::value;

namespace _detail {
template <typename> struct is_signed_integer : std::false_type {};

template <typename T, string_literal Tag, std::intmax_t Min, std::intmax_t Max>
struct is_signed_integer<signed_integer<T, Tag, Min, Max>> : std::true_type {};
} // namespace _detail

template <typename T>
concept signed_integer_like =
    _detail::is_signed_integer<std::remove_cvref_t<T>>::value;

template <typename T, std::intmax_t Min, std::intmax_t Max>
concept signed_integer_in_range =
    signed_integer_like<T> && (T::min <= Min) && (T::max >= Max);

template <typename T>
concept streamable = requires(std::ostream& os, T t) { os << t; };

namespace _detail {
template <typename> struct specializes_formatter : std::false_type {};

template <typename T>
struct specializes_formatter<std::formatter<T>> : std::true_type {};
} // namespace _detail

template <typename T>
concept incrementable = requires(T t) {
  { ++t } -> std::same_as<T&>;
  { t++ } -> std::same_as<T>;
};

template <typename T>
concept decrementable = requires(T t) {
  { --t } -> std::same_as<T&>;
  { t-- } -> std::same_as<T>;
};

template <typename T, typename U>
concept add_with = requires(T t, U u) { t + u; };

template <typename T, typename U>
concept add_assignable_with = requires(T t, U u) {
  { t += u } -> std::same_as<T&>;
};

template <typename T, typename U>
concept subtract_with = requires(T t, U u) { t - u; };

template <typename T, typename U>
concept subtract_assignable_with = requires(T t, U u) {
  { t -= u } -> std::same_as<T&>;
};

template <typename T, typename U>
concept multiply_with = requires(T t, U u) { t * u; };

template <typename T, typename U>
concept multiply_assignable_with = requires(T t, U u) {
  { t *= u } -> std::same_as<T&>;
};

template <typename T, typename U>
concept divide_with = requires(T t, U u) { t / u; };

template <typename T, typename U>
concept divide_assignable_with = requires(T t, U u) {
  { t /= u } -> std::same_as<T&>;
};

template <typename T, typename U>
concept modulo_with = requires(T t, U u) { t % u; };

template <typename T, typename U>
concept modulo_assignable_with = requires(T t, U u) {
  { t %= u } -> std::same_as<T&>;
};
} // namespace ada_types

#endif