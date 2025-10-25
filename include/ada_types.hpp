#ifndef ADA_TYPES_HPP
#define ADA_TYPES_HPP

#include <algorithm>
#include <array>
#include <charconv>
#include <concepts>
#include <cstdint>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>
#include <utility>

namespace ada_types {

template <std::size_t N> struct type_tag {
  std::array<char, N> _data{};

  constexpr type_tag(const char (&str)[N]) {
    std::ranges::copy(str, _data.begin());
  }

  constexpr auto size() const noexcept -> std::size_t { return N - 1; }

  constexpr auto data() const noexcept -> const char* { return _data.data(); }

  constexpr auto begin() const noexcept -> const char* { return _data.data(); }

  constexpr auto end() const noexcept -> const char* {
    return _data.data() + N;
  }
};

template <std::size_t N1, std::size_t N2>
constexpr auto operator==(const type_tag<N1>& lhs,
                          const type_tag<N2>& rhs) noexcept -> bool {
  return std::ranges::equal(lhs, rhs);
}

template <std::size_t N> type_tag(const char (&)[N]) -> type_tag<N>;

template <class Tp, type_tag Tag> class strong_type_base {
public:
  using underlying_type = Tp;
  constexpr static type_tag tag = Tag;

  template <class Self> constexpr auto&& get_value(this Self&& self) {
    return std::forward<Self>(self)._value_;
  }

protected:
  constexpr strong_type_base()
    requires std::is_default_constructible_v<Tp>
  = default;

  constexpr explicit strong_type_base(const Tp& value) : _value_(value) {}

  template <class Up = Tp>
    requires(!std::is_reference_v<Up>)
  constexpr explicit strong_type_base(Up&& value)
      : _value_(std::forward<Up>(value)) {}

  Tp _value_{};
};

template <class Tp>
concept streamable = requires(std::ostream& os, const Tp& obj) { os << obj; };

template <class Derived> struct printing {
  friend auto operator<<(std::ostream& os, const Derived& obj) -> std::ostream&
    requires streamable<typename Derived::underlying_type>
  {
    os << obj.get_value();
    return os;
  }
};

template <class Derived> struct comparison {
  friend constexpr auto operator==(const Derived& lhs, const Derived& rhs)
      -> bool
    requires std::equality_comparable<typename Derived::underlying_type>
  {
    return lhs.get_value() == rhs.get_value();
  }

  friend constexpr auto operator<=>(const Derived& lhs, const Derived& rhs)
    requires std::three_way_comparable<typename Derived::underlying_type>
  {
    return lhs.get_value() <=> rhs.get_value();
  }
};

template <class Tp, type_tag Tag>
class strong_type : public strong_type_base<Tp, Tag>,
                    public printing<strong_type<Tp, Tag>>,
                    public comparison<strong_type<Tp, Tag>> {
protected:
  constexpr strong_type()
    requires std::is_default_constructible_v<Tp>
  = default;

  constexpr explicit strong_type(const Tp& value)
      : strong_type_base<Tp, Tag>(value) {}

  template <class Up = Tp>
    requires(!std::is_reference_v<Up>)
  constexpr explicit strong_type(Up&& value)
      : strong_type_base<Tp, Tag>(std::forward<Up>(value)) {}
}; // namespace ada_types

template <type_tag LHSType, type_tag RHSType>
constexpr bool are_types_compatible = LHSType == RHSType;

// --- Scalar Types ---

template <class T>
concept incrementable = requires(T t) { ++t; };

template <class Derived> struct checked_increment {
  friend constexpr auto operator++(Derived& d) -> Derived&
    requires incrementable<typename Derived::underlying_type>
  {
    ++d.get_value();
    if (d.get_value() > Derived::last) {
      throw std::out_of_range("Value out of range during increment");
    }
    return d;
  }

  friend constexpr auto operator++(Derived& d, int) -> Derived
    requires incrementable<typename Derived::underlying_type>
  {
    Derived temp{d};
    ++d;
    if (d.get_value() > Derived::last) {
      throw std::out_of_range("Value out of range during increment");
    }
    return temp;
  }
};

template <class T>
concept decrementable = requires(T t) { --t; };

template <class Derived> struct checked_decrement {
  friend constexpr auto operator--(Derived& d) -> Derived&
    requires decrementable<typename Derived::underlying_type>
  {
    --d.get_value();
    if (d.get_value() < Derived::first) {
      throw std::out_of_range("Value out of range during decrement");
    }
    return d;
  }

  friend constexpr auto operator--(Derived& d, int) -> Derived
    requires decrementable<typename Derived::underlying_type>
  {
    Derived temp{d};
    --d;
    if (d.get_value() < Derived::first) {
      throw std::out_of_range("Value out of range during decrement");
    }
    return temp;
  }
};

template <class Derived>
struct arithmetic : public checked_increment<Derived>,
                    public checked_decrement<Derived> {
  template <class RHS>
  friend constexpr auto operator+=(Derived& lhs, const RHS& rhs) -> Derived& {
    static_assert(are_types_compatible<Derived::tag, RHS::tag>,
                  "Attempting to add incompatible types");
    lhs.get_value() += rhs.get_value();
    if (lhs.get_value() < Derived::first || lhs.get_value() > Derived::last) {
      throw std::out_of_range("Value out of range during subtraction");
    }
    return lhs;
  }

  template <class RHS>
  friend constexpr auto operator+(Derived lhs, const RHS& rhs) -> Derived {
    static_assert(are_types_compatible<Derived::tag, RHS::tag>,
                  "Attempting to add incompatible types");
    return Derived(lhs.get_value() + rhs.get_value());
  }

  friend constexpr auto operator-=(Derived& lhs, const Derived& rhs)
      -> Derived& {
    lhs.get_value() -= rhs.get_value();
    if (lhs.get_value() < Derived::first || lhs.get_value() > Derived::last) {
      throw std::out_of_range("Value out of range during subtraction");
    }
    return lhs;
  }

  friend constexpr auto operator-(Derived lhs, const Derived& rhs) -> Derived {
    return Derived(lhs.get_value() - rhs.get_value());
  }

  friend constexpr auto operator*=(Derived& lhs, const Derived& rhs)
      -> Derived& {
    lhs.get_value() *= rhs.get_value();
    if (lhs.get_value() < Derived::first || lhs.get_value() > Derived::last) {
      throw std::out_of_range("Value out of range during multiplication");
    }
    return lhs;
  }

  friend constexpr auto operator*(const Derived& lhs, const Derived& rhs)
      -> Derived {
    return Derived(lhs.get_value() * rhs.get_value());
  }

  friend constexpr auto operator/=(Derived& lhs, const Derived& rhs)
      -> Derived& {
    if constexpr (std::integral<typename Derived::underlying_type>) {
      if (rhs.get_value() == 0) {
        throw std::out_of_range("Division by 0");
      }
    }
    lhs.get_value() /= rhs.get_value();
    if (lhs.get_value() < Derived::first || lhs.get_value() > Derived::last) {
      throw std::out_of_range("Value out of range during division");
    }
    return lhs;
  }

  friend constexpr auto operator/(Derived lhs, const Derived& rhs) -> Derived {
    if constexpr (std::integral<typename Derived::underlying_type>) {
      if (rhs.get_value() == 0) {
        throw std::out_of_range("Division by 0");
      }
    }
    return Derived(lhs.get_value() / rhs.get_value());
  }
};

// --- Signed Integer Types ---

template <class Derived>
struct integer_arithmetic : public arithmetic<Derived> {
  friend constexpr auto operator%=(Derived& lhs, const Derived& rhs)
      -> Derived& {
    lhs.get_value() %= rhs.get_value();
    if (lhs.get_value() < Derived::first || lhs.get_value() > Derived::last) {
      throw std::out_of_range("Value out of range during modulo");
    }
    return lhs;
  }

  friend constexpr auto operator%(Derived lhs, const Derived& rhs) -> Derived {
    return Derived(lhs.get_value() % rhs.get_value());
  }
};

namespace _detail {
constexpr type_tag signed_integer_base_tag{"__signed_integer_base"};
}

template <class Tp, std::intmax_t Min, std::intmax_t Max, type_tag Tag>
  requires std::signed_integral<std::remove_cvref_t<Tp>>
class signed_integer;

namespace _detail {
template <class> struct is_signed_integer : std::false_type {};

template <class Tp, std::intmax_t Min, std::intmax_t Max, type_tag Tag>
struct is_signed_integer<signed_integer<Tp, Min, Max, Tag>> : std::true_type {};

template <class T>
constexpr bool is_signed_integer_v =
    is_signed_integer<std::remove_cvref_t<T>>::value;
} // namespace _detail

template <class Tp, std::intmax_t Min, std::intmax_t Max, type_tag Tag>
  requires std::signed_integral<std::remove_cvref_t<Tp>>
class signed_integer
    : public strong_type<Tp, Tag>,
      public integer_arithmetic<signed_integer<Tp, Min, Max, Tag>> {
  static_assert(Min < Max);

public:
  constexpr static Tp first = Min;
  constexpr static Tp last = Max;
  using base_type =
      signed_integer<std::intmax_t, std::numeric_limits<std::intmax_t>::min(),
                     std::numeric_limits<std::intmax_t>::max(),
                     _detail::signed_integer_base_tag>;

  constexpr static auto value(std::string_view v) -> signed_integer {
    std::intmax_t int_value;
    const std::from_chars_result r =
        std::from_chars(v.begin(), v.end(), int_value);
    if (r.ptr != v.end()) {
      throw std::invalid_argument("Could not convert string to integer value");
    }
    if (r.ec != std::errc()) {
      throw std::invalid_argument("Could not convert string to integer value");
    }
    return signed_integer(int_value);
  }

  constexpr signed_integer() = default;

  template <class Up = Tp>
    requires std::signed_integral<std::remove_cvref_t<Up>>
  constexpr explicit signed_integer(Up&& value)
      : strong_type<Tp, Tag>(static_cast<Tp>(value)) {
    if (value < Min || value > Max) {
      std::string message = "Input out of range. Input value: ";
      message.append(std::to_string(value));
      message.append(", value must be in range [");
      message.append(std::to_string(Min)).append(", ");
      message.append(std::to_string(Max)).append("]");
      throw std::out_of_range(message);
    }
  }

  template <class Up, std::intmax_t Min2, std::intmax_t Max2, type_tag Tag2>
  constexpr explicit signed_integer(signed_integer<Up, Min2, Max2, Tag2> other)
      : strong_type<Tp, Tag>(other.get_value()) {
    static_assert(are_types_compatible<Tag, Tag2>,
                  "Cannot construct this from integer of different type");
    if constexpr (Min2 < Min || Max2 < Min || Min2 > Max || Max2 > Max) {
      if (other.get_value() < Min || other.get_value() > Max) {
        std::string message = "Input out of range. Input value: ";
        message.append(std::to_string(this->get_value()));
        message.append(", value must be in range [");
        message.append(std::to_string(Min)).append(", ");
        message.append(std::to_string(Max)).append("]");
        throw std::out_of_range(message);
      }
    }
  }

  template <class Up, std::intmax_t Min2, std::intmax_t Max2, type_tag Tag2>
  constexpr auto operator=(signed_integer<Up, Min2, Max2, Tag2> other)
      -> signed_integer& {
    static_assert(are_types_compatible<Tag, Tag2>,
                  "Cannot construct this from integer of different type");
    if constexpr (Min2 < Min || Max2 < Min || Min2 > Max || Max2 > Max) {
      if (other.get_value() < Min || other.get_value() > Max) {
        std::string message = "Input out of range. Input value: ";
        message.append(std::to_string(this->get_value()));
        message.append(", value must be in range [");
        message.append(std::to_string(Min)).append(", ");
        message.append(std::to_string(Max)).append("]");
        throw std::out_of_range(message);
      }
    }
    this->get_value() = other.get_value();
    return *this;
  }
};

// --- Floating Point Types ---

template <class Tp, type_tag Tag>
  requires std::floating_point<std::remove_cvref_t<Tp>>
class floating_point;

namespace _detail {
template <class> struct is_floating_point : std::false_type {};

template <class Tp, type_tag Tag>
struct is_floating_point<floating_point<Tp, Tag>> : std::true_type {};

template <class T>
constexpr bool is_floating_point_v = is_floating_point<T>::value;
} // namespace _detail

template <class Tp, type_tag Tag>
  requires std::floating_point<std::remove_cvref_t<Tp>>
class floating_point : public strong_type<Tp, Tag>,
                       public arithmetic<floating_point<Tp, Tag>> {
public:
  constexpr floating_point() = default;

  template <class Up = Tp>
    requires std::floating_point<std::remove_cvref_t<Up>>
  constexpr floating_point(Up&& up)
      : strong_type<Tp, Tag>(std::forward<Up>(up)) {}
};

// --- Type Creation Helpers ---

template <std::intmax_t Min, std::intmax_t Max> struct range {
  static_assert(Min < Max);
  constexpr static std::intmax_t first = Min;
  constexpr static std::intmax_t last = Max;
};

template <type_tag Tag, class...> struct new_type_impl {};

template <class...> struct sub_type_impl {};

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
} // namespace _detail

template <type_tag Tag, std::intmax_t Min, std::intmax_t Max>
struct new_type_impl<Tag, range<Min, Max>> {
  using type = signed_integer<typename _detail::integer_type<Min, Max>::type,
                              Min, Max, Tag>;
};

template <type_tag Tag, class I>
  requires _detail::is_signed_integer_v<I>
struct new_type_impl<Tag, I> {
  using type =
      signed_integer<typename I::underlying_type, I::first, I::last, Tag>;
};

template <type_tag Tag, class T, std::intmax_t Min, std::intmax_t Max>
  requires std::signed_integral<std::remove_cvref_t<T>>
struct new_type_impl<Tag, T, range<Min, Max>> {
  using type = signed_integer<T, Min, Max, Tag>;
};

template <type_tag Tag, class T>
  requires(!std::floating_point<T> && !_detail::is_signed_integer_v<T>)
struct new_type_impl<Tag, T> {
  using type = strong_type<T, Tag>;
};

template <type_tag Tag, class T>
  requires std::floating_point<T>
struct new_type_impl<Tag, T> {
  using type = floating_point<T, Tag>;
};

template <class I>
  requires _detail::is_signed_integer_v<I>
struct sub_type_impl<I> {
  using type =
      signed_integer<typename I::underlying_type, I::first, I::last, I::tag>;
};

template <class I, std::intmax_t Min, std::intmax_t Max>
struct sub_type_impl<I, range<Min, Max>> {
  using type = signed_integer<typename _detail::integer_type<Min, Max>::type,
                              Min, Max, I::tag>;
};

template <type_tag Tag, class... Args>
using new_type = new_type_impl<Tag, Args...>::type;

template <class... Args> using sub_type = sub_type_impl<Args...>::type;

// --- Predefined Types ---

namespace _detail {
constexpr type_tag integer_tag{"__integer"};
}

using integer =
    new_type<_detail::integer_tag, range<std::numeric_limits<int>::min(),
                                         std::numeric_limits<int>::max()>>;
using natural = sub_type<integer, range<0, integer::last>>;
using positive = sub_type<integer, range<1, integer::last>>;

namespace _detail {
constexpr type_tag short_float_tag{"__short_float"};
constexpr type_tag long_float_tag{"__long_float"};
constexpr type_tag long_long_float_tag{"__long_long_float"};
} // namespace _detail

using short_float = new_type<_detail::short_float_tag, float>;
using long_float = new_type<_detail::long_float_tag, double>;
using long_long_float = new_type<_detail::long_long_float_tag, long double>;
} // namespace ada_types
#endif