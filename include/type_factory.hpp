#ifndef TYPE_FACTORY_HPP
#define TYPE_FACTORY_HPP

#include "ada_types_fwd.hpp"
#include "arithmetic_type.hpp"
#include "signed_integer_type.hpp"
#include <limits>
namespace ada_types {
struct arithmetic_tag {};

template <std::intmax_t Min, std::intmax_t Max> struct range {};

namespace _detail {
template <std::intmax_t Min, std::intmax_t Max> struct integer_type {
  using type = std::int8_t;
};

template <std::intmax_t Min, std::intmax_t Max>
  requires((Min < std::numeric_limits<std::int8_t>::min() &&
            Min >= std::numeric_limits<std::int16_t>::min()) ||
           (Max > std::numeric_limits<std::int8_t>::max() &&
            Max <= std::numeric_limits<std::int16_t>::max()))
struct integer_type<Min, Max> {
  using type = std::int16_t;
};

template <std::intmax_t Min, std::intmax_t Max>
  requires((Min < std::numeric_limits<std::int16_t>::min() &&
            Min >= std::numeric_limits<std::int32_t>::min()) ||
           (Max > std::numeric_limits<std::int16_t>::max() &&
            Max <= std::numeric_limits<std::int32_t>::max()))
struct integer_type<Min, Max> {
  using type = std::int32_t;
};

template <std::intmax_t Min, std::intmax_t Max>
  requires((Min < std::numeric_limits<std::int32_t>::min() &&
            Min >= std::numeric_limits<std::intmax_t>::min()) ||
           (Max > std::numeric_limits<std::int32_t>::max() &&
            Max <= std::numeric_limits<std::intmax_t>::max()))
struct integer_type<Min, Max> {
  using type = std::intmax_t;
};

template <auto, typename...> struct new_type_impl;

template <string_literal Tag, typename T> struct new_type_impl<Tag, T> {
  using type = strong_type<T, Tag>;
};

template <string_literal Tag, typename T>
struct new_type_impl<Tag, T, arithmetic_tag> {
  using type = arithmetic_type<T, Tag>;
};

template <string_literal Tag, std::intmax_t Min, std::intmax_t Max>
struct new_type_impl<Tag, range<Min, Max>> {
  using type =
      signed_integer<typename integer_type<Min, Max>::type, Tag, Min, Max>;
};
} // namespace _detail

template <string_literal Tag, typename... Args>
using new_type = _detail::new_type_impl<Tag, Args...>::type;

namespace _detail {
template <typename...> struct subtype_impl;

template <typename T>
  requires strong_type_like<T>
struct subtype_impl<T> {
  using type = strong_type<typename T::underlying_type, T::tag>;
};

template <typename T, std::intmax_t Min, std::intmax_t Max>
struct subtype_impl<T, range<Min, Max>> {
  using type =
      signed_integer<typename integer_type<Min, Max>::type, T::tag, Min, Max>;
};
} // namespace _detail

template <typename... Args>
using subtype = _detail::subtype_impl<Args...>::type;

// Pre-defined types
using integer = new_type<"__integer", range<std::numeric_limits<int>::min(),
                                            std::numeric_limits<int>::max()>>;
using natural = subtype<integer, range<0, std::numeric_limits<int>::max()>>;
using positive = subtype<natural, range<1, std::numeric_limits<int>::max()>>;
} // namespace ada_types

#endif