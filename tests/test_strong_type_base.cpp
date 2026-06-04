#include <cina.hpp>

#include <gtest/gtest.h>

#include <concepts>
#include <type_traits>

class int_wrapper {
public:
  constexpr int_wrapper() = default;

  constexpr int_wrapper(const int value) : _m_value(value) {}

  constexpr explicit int_wrapper(const int v1, const int v2)
      : _m_value(v1 + v2) {}

  constexpr explicit int_wrapper(std::initializer_list<int> il, int init)
      : _m_value(init) {
    for (const auto& v : il) {
      _m_value += v;
    }
  }

  constexpr auto value() const noexcept -> int { return _m_value; }

  constexpr operator int() const noexcept { return _m_value; }

private:
  int _m_value{};
};

class double_wrapper {
public:
  constexpr double_wrapper() = default;

  constexpr double_wrapper(const double value) : _m_value(value) {}

  constexpr double_wrapper(int_wrapper value) : _m_value(value.value()) {}

  constexpr explicit double_wrapper(const double v1, const double v2)
      : _m_value(v1 + v2) {}

  constexpr explicit double_wrapper(std::initializer_list<double> il,
                                    double init)
      : _m_value(init) {
    for (const auto& v : il) {
      _m_value += v;
    }
  }

  constexpr auto operator=(const int_wrapper& value) noexcept
      -> double_wrapper& {
    _m_value = value.value();
    return *this;
  }

private:
  friend constexpr auto operator<=>(const double_wrapper&,
                                    const double_wrapper&) = default;

  double _m_value{};
};

TEST(StrongTypeBase, TestCXXProperties) {
  using type = cina::strong_type<struct Tag, int_wrapper>;
  EXPECT_TRUE(std::regular<type>);
  EXPECT_TRUE(std::is_trivially_copyable_v<type>);
  EXPECT_TRUE(std::is_trivially_destructible_v<type>);
  EXPECT_TRUE(std::is_trivially_copy_constructible_v<type>);
  EXPECT_TRUE(std::is_trivially_move_constructible_v<type>);
  EXPECT_TRUE(std::is_trivially_copy_assignable_v<type>);
  EXPECT_TRUE(std::is_trivially_move_assignable_v<type>);
  EXPECT_TRUE(std::is_nothrow_move_constructible_v<type>);
  EXPECT_EQ(sizeof(type), sizeof(int_wrapper));
  EXPECT_EQ(alignof(type), alignof(int_wrapper));

  using reference = cina::strong_type<struct Tag2, int_wrapper&>;
  EXPECT_FALSE(std::is_default_constructible_v<reference>);
  EXPECT_TRUE(std::is_copy_constructible_v<reference>);
  EXPECT_TRUE(std::is_move_constructible_v<reference>);
  EXPECT_TRUE(std::is_copy_assignable_v<reference>);
  EXPECT_TRUE(std::is_move_assignable_v<reference>);
  EXPECT_TRUE(std::equality_comparable<reference>);
}

TEST(StrongTypeBase, TestConstructor) {
  using type = cina::strong_type<struct Tag, int_wrapper>;
  const type a{};
  EXPECT_EQ(a.unwrap(), 0);
  constexpr type ca{};
  static_assert(ca.unwrap() == 0);
  const type b{42};
  EXPECT_EQ(b.unwrap(), 42);
  constexpr type cb{42};
  static_assert(cb.unwrap() == 42);
  const type c{std::in_place, 42, 42};
  EXPECT_EQ(c.unwrap(), 84);
  constexpr type cc{std::in_place, 42, 42};
  static_assert(cc.unwrap() == 84);
  const type d{std::in_place, {1, 2, 3}, 42};
  EXPECT_EQ(d.unwrap(), 48);
  constexpr type cd{std::in_place, {1, 2, 3}, 42};
  static_assert(cd.unwrap() == 48);

  using reference = cina::strong_type<struct Tag2, int_wrapper&>;
  int_wrapper value{42};
  reference ref{value};
  EXPECT_EQ(ref.unwrap(), 42);
  EXPECT_FALSE((std::is_constructible_v<reference, const int&>));
  EXPECT_FALSE((std::is_constructible_v<reference, int&&>));
  EXPECT_FALSE((std::is_constructible_v<reference, const int_wrapper&&>));

  using const_reference = cina::strong_type<struct Tag3, const int_wrapper&>;
  const int_wrapper const_value{42};
  const_reference const_ref{const_value};
  EXPECT_TRUE((std::is_constructible_v<const_reference, int_wrapper&>));
  EXPECT_FALSE((std::is_constructible_v<const_reference, int&&>));
  EXPECT_FALSE((std::is_constructible_v<const_reference, const int_wrapper&&>));

  using type2 = cina::strong_type<struct Tag4, int_wrapper>;
  EXPECT_FALSE((std::is_constructible_v<type2, type>));
  EXPECT_FALSE((std::is_constructible_v<reference, type>));
  EXPECT_FALSE((std::is_constructible_v<const_reference, type>));
  EXPECT_FALSE((std::is_constructible_v<reference, const_reference>));
  EXPECT_FALSE((std::is_constructible_v<const_reference, reference>));

  using type3 = cina::strong_type<struct Tag, double_wrapper>;
  const type3 f{a};
  EXPECT_EQ(f.unwrap(), double_wrapper{0.0});
  EXPECT_FALSE((std::is_constructible_v<type, type3>));
  constexpr type3 g{cb};
  static_assert(g.unwrap() == double_wrapper{42.0});
}

TEST(TestStrongType, TestAssignment) {
  using type1 = cina::strong_type<struct Tag1, int_wrapper>;
  using type2 = cina::strong_type<struct Tag2, int_wrapper>;
  using type3 = cina::strong_type<struct Tag1, double_wrapper>;

  EXPECT_FALSE((std::is_assignable_v<type1&, type2>));
  EXPECT_FALSE((std::is_assignable_v<type1&, type3>));

  const type1 a{42};
  type3 b{};
  b = a;
  EXPECT_EQ(b.unwrap(), double_wrapper{42.0});

  using reference = cina::strong_type<struct Tag3, int_wrapper&>;
  int_wrapper value{42};
  reference ref{value};
  int_wrapper value2{84};
  reference ref2{value2};
  ref = ref2;
  EXPECT_EQ(value, 84);
  ref = cina::remove_reference_t<reference>{int_wrapper{42}};
  EXPECT_EQ(value, 42);
}

TEST(TestStrongType, TestComparison) {
  using type1 = cina::strong_type<struct Tag1, int_wrapper>;
  const type1 a{42};
  const type1 b{43};
  EXPECT_EQ(a, a);
  EXPECT_NE(a, b);

  using type2 = cina::strong_type<struct Tag2, int_wrapper>;
  EXPECT_FALSE((std::equality_comparable_with<type1, type2>));

  using reference = cina::strong_type<struct Tag3, int_wrapper&>;
  int_wrapper value{42};
  reference ref{value};
  int_wrapper value2{43};
  reference ref2{value2};
  EXPECT_EQ(ref, ref);
  EXPECT_NE(ref, ref2);

  using const_reference = cina::strong_type<struct Tag4, const int_wrapper&>;
  const int_wrapper const_value{42};
  const_reference const_ref{const_value};
  const int_wrapper const_value2{43};
  const_reference const_ref2{const_value2};
  EXPECT_EQ(const_ref, const_ref);
  EXPECT_NE(const_ref, const_ref2);
}

TEST(TestStrongType, TestUnwrap) {
  using type = cina::strong_type<struct Tag, int_wrapper>;
  type a{42};
  EXPECT_TRUE((std::is_same_v<decltype(a.unwrap()), int_wrapper&>));
  EXPECT_TRUE((
      std::is_same_v<decltype(std::as_const(a).unwrap()), const int_wrapper&>));
  EXPECT_TRUE((std::is_same_v<decltype(std::move(a).unwrap()), int_wrapper&&>));
  EXPECT_TRUE((std::is_same_v<decltype(std::move(std::as_const(a)).unwrap()),
                              const int_wrapper&&>));

  using reference = cina::strong_type<struct Tag2, int_wrapper&>;
  int_wrapper value{42};
  reference ref{value};
  EXPECT_TRUE((std::is_same_v<decltype(ref.unwrap()), int_wrapper&>));
  EXPECT_TRUE(
      (std::is_same_v<decltype(std::as_const(ref).unwrap()), int_wrapper&>));
  EXPECT_TRUE(
      (std::is_same_v<decltype(std::move(ref).unwrap()), int_wrapper&>));
  EXPECT_TRUE((std::is_same_v<decltype(std::move(std::as_const(ref)).unwrap()),
                              int_wrapper&>));
}

TEST(TestStrongType, TestSwap) {
  using type = cina::strong_type<struct Tag, int_wrapper>;
  type a{42};
  type b{84};
  a.swap(b);
  EXPECT_EQ(a.unwrap(), 84);
  EXPECT_EQ(b.unwrap(), 42);
  EXPECT_TRUE(noexcept(a.swap(b)));

  swap(a, b);
  EXPECT_EQ(a.unwrap(), 42);
  EXPECT_EQ(b.unwrap(), 84);
  EXPECT_TRUE(noexcept(swap(a, b)));

  using reference = cina::strong_type<struct Tag2, int_wrapper&>;
  int_wrapper value1{42};
  int_wrapper value2{84};
  reference ref1{value1};
  reference ref2{value2};
  ref1.swap(ref2);
  EXPECT_EQ(value1, 84);
  EXPECT_EQ(value2, 42);
  EXPECT_TRUE(noexcept(ref1.swap(ref2)));

  swap(a, b);
  EXPECT_EQ(a.unwrap(), 84);
  EXPECT_EQ(b.unwrap(), 42);
  EXPECT_TRUE(noexcept(swap(ref1, ref2)));
}

TEST(TestStrongType, TestTypeFactory) {
  using type2 = cina::new_type<struct Tag2, bool, cina::no_skills>;
  EXPECT_TRUE((std::same_as<type2, cina::strong_type<struct Tag2, bool>>));

  using type3 = cina::new_type<struct Tag3, bool, cina::output_stream>;
  EXPECT_TRUE((!std::same_as<type3, cina::boolean_type<struct Tag3, bool>>));
  EXPECT_TRUE((std::derived_from<type3, cina::output_stream::skill<type3>>));
}