#include "cina.hpp"

#include <concepts>
#include <gtest/gtest.h>

TEST(TestIntegerType, TestCXXProperties) {
  using int_type = cina::integral_type<struct IntType, int>;

  EXPECT_FALSE(std::is_default_constructible_v<int_type>);
  EXPECT_TRUE(std::is_nothrow_copy_constructible_v<int_type>);
  EXPECT_TRUE(std::is_nothrow_copy_assignable_v<int_type>);
  EXPECT_TRUE(std::is_trivially_copyable_v<int_type>);
  EXPECT_TRUE(std::is_nothrow_move_constructible_v<int_type>);
  EXPECT_TRUE(std::is_nothrow_move_assignable_v<int_type>);
  EXPECT_TRUE(std::is_nothrow_destructible_v<int_type>);
  EXPECT_TRUE(std::is_trivially_destructible_v<int_type>);

  EXPECT_EQ(sizeof(int_type), sizeof(int));
  EXPECT_EQ(alignof(int_type), alignof(int));

  EXPECT_TRUE(std::equality_comparable<int_type>);
  EXPECT_TRUE(std::swappable<int_type>);
  EXPECT_TRUE(std::copyable<int_type>);
  EXPECT_TRUE(std::totally_ordered<int_type>);
  EXPECT_TRUE(std::three_way_comparable<int_type>);
}

TEST(TestIntegerType, TestConstructor) {
  using int_type = cina::integral_type<struct IntType, int>;

  const int_type i1{42};
  EXPECT_EQ(i1.unwrap(), 42);

  EXPECT_FALSE((std::is_constructible_v<int_type, std::int64_t>));
  EXPECT_FALSE((std::is_constructible_v<int_type, double>));
  EXPECT_FALSE((std::is_constructible_v<int_type, std::uint8_t>));
}

TEST(TestIntegerType, TestFormat) {
  using int_type = cina::integral_type<struct IntType, std::int8_t>;

  const std::string str =
      std::format("{}", int_type{static_cast<std::int8_t>(42)});
  EXPECT_STREQ(str.c_str(), "42");
}

TEST(TestIntegerType, TestHash) {
  using int_type = cina::integral_type<struct IntType, int>;

  const int_type i1{42};

  auto hash = std::hash<int_type>{}(i1);
  EXPECT_EQ(std::hash<int>{}(i1.unwrap()), hash);
}

TEST(TestIntegerType, TestEquality) {
  using int_type = cina::integral_type<struct IntType, int>;

  const int_type i1{42};
  const int_type i2{42};
  const int_type i3{43};

  EXPECT_EQ(i1, i2);
  EXPECT_NE(i1, i3);
  EXPECT_NE(i2, i3);
}

TEST(TestIntegerType, TestComparison) {
  using int_type = cina::integral_type<struct IntType, int>;

  const int_type i1{42};
  const int_type i2{43};

  EXPECT_LT(i1, i2);
  EXPECT_GT(i2, i1);
  EXPECT_LE(i1, i2);
  EXPECT_GE(i2, i1);

  EXPECT_EQ(i1 <=> i2, std::strong_ordering::less);
  EXPECT_EQ(i2 <=> i1, std::strong_ordering::greater);
  EXPECT_EQ(i1 <=> i1, std::strong_ordering::equal);
}

TEST(TestIntegerType, TestOutputStream) {
  using int_type = cina::integral_type<struct IntType, std::int8_t>;

  const int_type i1{static_cast<std::int8_t>(42)};

  std::ostringstream oss;
  oss << i1;
  EXPECT_STREQ(oss.str().c_str(), "42");
}

TEST(TestIntegerType, TestTypeFactory) {
  using int_type1 = cina::new_type<struct IntType1, std::int8_t>;

  EXPECT_TRUE((
      std::same_as<int_type1,
                   cina::integral_type<typename int_type1::tag, std::int8_t>>));

  using int_type2 = cina::new_type<struct IntType2, std::int8_t>;
  EXPECT_FALSE((std::same_as<int_type1, int_type2>));
}