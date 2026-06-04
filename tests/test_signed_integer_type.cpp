#include <cina.hpp>

#include <compare>
#include <gtest/gtest.h>

TEST(TestSignedIntegerType, TestTypeFactory) {
  using type = cina::new_type<struct Tag, int>;
  EXPECT_TRUE((std::same_as<type, cina::signed_integer_type<struct Tag, int>>));

  using type2 = cina::new_type<struct Tag2, const int>;
  EXPECT_TRUE(
      (std::same_as<type2, cina::signed_integer_type<struct Tag2, const int>>));

  using type3 = cina::new_type<struct Tag3, int&>;
  EXPECT_TRUE(
      (std::same_as<type3, cina::signed_integer_type<struct Tag3, int&>>));

  using type4 = cina::new_type<struct Tag4, const int&>;
  EXPECT_TRUE((
      std::same_as<type4, cina::signed_integer_type<struct Tag4, const int&>>));
}

TEST(TestSignedIntegerType, TestCXXProperties) {
  using type = cina::new_type<struct Tag, int>;
  EXPECT_FALSE(std::is_default_constructible_v<type>);
  EXPECT_TRUE(std::is_trivially_copyable_v<type>);
  EXPECT_TRUE(std::is_trivially_destructible_v<type>);
  EXPECT_TRUE(std::is_trivially_copy_constructible_v<type>);
  EXPECT_TRUE(std::is_trivially_move_constructible_v<type>);
  EXPECT_TRUE(std::is_trivially_copy_assignable_v<type>);
  EXPECT_TRUE(std::is_trivially_move_assignable_v<type>);
  EXPECT_TRUE(std::is_nothrow_move_constructible_v<type>);
  EXPECT_EQ(sizeof(type), sizeof(int));
  EXPECT_EQ(alignof(type), alignof(int));
  EXPECT_FALSE((std::convertible_to<type, int>));
  EXPECT_FALSE((std::convertible_to<type, bool>));
  EXPECT_TRUE(std::swappable<type>);
  EXPECT_TRUE((std::three_way_comparable<type, std::strong_ordering>));

  using reference = cina::new_type<struct Tag2, int&>;
  EXPECT_FALSE(std::is_default_constructible_v<reference>);
  EXPECT_TRUE(std::is_copy_constructible_v<reference>);
  EXPECT_TRUE(std::is_move_constructible_v<reference>);
  EXPECT_TRUE(std::is_copy_assignable_v<reference>);
  EXPECT_TRUE(std::is_move_assignable_v<reference>);
  EXPECT_TRUE(std::equality_comparable<reference>);
  EXPECT_TRUE(std::swappable<reference>);
  EXPECT_TRUE((std::three_way_comparable<reference, std::strong_ordering>));
}

TEST(TestSignedIntegerType, TestConstructor) {
  using type = cina::new_type<struct Tag, int>;
  EXPECT_FALSE((std::is_constructible_v<type, long long>));
  EXPECT_FALSE((std::is_constructible_v<type, unsigned int>));
  type a{42};
  EXPECT_EQ(a.unwrap(), 42);
  constexpr type ca{42};
  static_assert(ca.unwrap() == 42);

  using reference = cina::new_type<struct Tag, int&>;
  int value{42};
  reference ref{value};
  EXPECT_EQ(ref.unwrap(), 42);
  EXPECT_FALSE((std::is_constructible_v<reference, const int&>));
  EXPECT_FALSE((std::is_constructible_v<reference, int&&>));
  EXPECT_FALSE((std::is_constructible_v<reference, const int&&>));

  using const_reference = cina::new_type<struct Tag, const int&>;
  const int const_value{42};
  const_reference const_ref{const_value};
  EXPECT_TRUE((std::is_constructible_v<const_reference, int&>));
  EXPECT_FALSE((std::is_constructible_v<const_reference, int&&>));
  EXPECT_FALSE((std::is_constructible_v<const_reference, const int&&>));
}

TEST(TestSignedIntegerType, TestComparison) {
  using type = cina::new_type<struct Tag, int>;
  const type a{42};
  const type b{43};
  EXPECT_EQ(a, a);
  EXPECT_NE(a, b);
  EXPECT_LT(a, b);
  EXPECT_LE(a, b);
  EXPECT_GT(b, a);
  EXPECT_GE(b, a);

  using reference = cina::new_type<struct Tag2, int&>;
  int value{42};
  reference ref{value};
  int value2{43};
  reference ref2{value2};
  EXPECT_EQ(ref, ref);
  EXPECT_NE(ref, ref2);
  EXPECT_LT(ref, ref2);
  EXPECT_LE(ref, ref2);
  EXPECT_GT(ref2, ref);
  EXPECT_GE(ref2, ref);

  using const_reference = cina::new_type<struct Tag3, const int&>;
  const int const_value{42};
  const_reference const_ref{const_value};
  const int const_value2{43};
  const_reference const_ref2{const_value2};
  EXPECT_EQ(const_ref, const_ref);
  EXPECT_NE(const_ref, const_ref2);
  EXPECT_LT(const_ref, const_ref2);
  EXPECT_LE(const_ref, const_ref2);
  EXPECT_GT(const_ref2, const_ref);
  EXPECT_GE(const_ref2, const_ref);
}

TEST(TestSignedIntegerType, TestOutputStream) {
  using type = cina::new_type<struct Tag, int>;
  type a{42};
  std::stringstream ss;
  ss << a;
  EXPECT_EQ(ss.str(), "42");

  using reference = cina::new_type<struct Tag2, int&>;
  int value{42};
  reference ref{value};
  std::stringstream ss2;
  ss2 << ref;
  EXPECT_EQ(ss2.str(), "42");

  using const_reference = cina::new_type<struct Tag3, const int&>;
  const int const_value{42};
  const_reference const_ref{const_value};
  std::stringstream ss3;
  ss3 << const_ref;
  EXPECT_EQ(ss3.str(), "42");
}

TEST(TestSignedIntegerType, TestInputStream) {
  using type = cina::new_type<struct Tag, int>;
  type a{0};
  std::stringstream ss("42");
  ss >> a;
  EXPECT_EQ(a.unwrap(), 42);

  using reference = cina::new_type<struct Tag2, int&>;
  int value{0};
  reference ref{value};
  std::stringstream ss2("42");
  ss2 >> ref;
  EXPECT_EQ(value, 42);
}

TEST(TestSignedIntegerType, TestAddition) {
  using type = cina::new_type<struct Tag, int>;
  type a{42};
  type b{84};
  type c = a + b;
  EXPECT_TRUE((std::same_as<decltype(c), type>));
  EXPECT_EQ(c.unwrap(), 126);

  using reference = cina::new_type<struct Tag2, int&>;
  int value{42};
  reference ref{value};
  int value2{84};
  reference ref2{value2};
  cina::remove_reference_t<reference> sum = ref + ref2;
  EXPECT_TRUE(!std::is_reference_v<cina::underlying_type_t<decltype(sum)>>);
  EXPECT_EQ(sum.unwrap(), 126);

  struct my_type : cina::new_type<my_type, int> {
    using cina::new_type<my_type, int>::new_type;
  };

  my_type d{42};
  auto e = d + d;
  EXPECT_EQ(e.unwrap(), 84);
}