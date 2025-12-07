#ifndef TEST_TYPES_HPP
#define TEST_TYPES_HPP

#include <initializer_list>
#include <numeric>

template <bool Nothrow> struct tattle {
  static inline int copy_ctor_count = 0;
  static inline int move_ctor_count = 0;
  static inline int copy_assign_count = 0;
  static inline int move_assign_count = 0;
  static inline int value_ctor_count = 0;
  static inline int il_ctor_count = 0;

  constexpr tattle() noexcept(Nothrow) {}

  explicit constexpr tattle(double v1, double v2) noexcept(Nothrow)
      : value(v1 + v2) {
    ++value_ctor_count;
  }

  explicit constexpr tattle(std::initializer_list<double> il) noexcept(Nothrow)
      : value(std::accumulate(il.begin(), il.end(), 0.0)) {
    ++il_ctor_count;
  }

  constexpr tattle(const tattle& other) noexcept(Nothrow) : value(other.value) {
    ++copy_ctor_count;
  }
  constexpr tattle(tattle&& other) noexcept(Nothrow) : value(other.value) {
    ++move_ctor_count;
  }
  tattle& operator=(const tattle& other) noexcept(Nothrow) {
    value = other.value;
    ++copy_assign_count;
    return *this;
  }
  tattle& operator=(tattle&& other) noexcept(Nothrow) {
    value = other.value;
    ++move_assign_count;
    return *this;
  }

  double value{};
};

template <bool Nothrow>
constexpr auto operator<=>(const tattle<Nothrow>& lhs,
                           const tattle<Nothrow>& rhs) {
  return lhs.value <=> rhs.value;
}

template <bool Nothrow>
constexpr auto operator==(const tattle<Nothrow>& lhs,
                          const tattle<Nothrow>& rhs) {
  return lhs.value == rhs.value;
}

using nothrow_tattle = tattle<true>;
using throwing_tattle = tattle<false>;

#endif
