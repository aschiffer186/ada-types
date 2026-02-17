#include "cina.hpp"

#include <compare>
#include <gtest/gtest.h>

#include <concepts>
#include <type_traits>

TEST(TestStrongTypeBase, TestCXXProperties) {
  using type = cina::strong_type<struct Tag, int>;

  EXPECT_TRUE(std::is_default_constructible_v<type>);
  EXPECT_TRUE(std::copyable<type>);
  EXPECT_TRUE(std::is_nothrow_destructible_v<type>);

  EXPECT_EQ(sizeof(type), sizeof(int));
  EXPECT_EQ(alignof(type), alignof(int));
  EXPECT_TRUE(std::equality_comparable<type>);
  EXPECT_FALSE(std::three_way_comparable<type>);
}

TEST(TestStrongTypeBase, TestConstructor) {}

TEST(TestStrongTypeBase, TestUnderlyingType) {
  using type = cina::strong_type<struct Tag, int>;
  EXPECT_TRUE((std::same_as<cina::underlying_type<type>, int>));
}

TEST(TestStongTypeBase, TestTypeFactory) {
  struct TestType {};

  using type1 = cina::new_type<struct Type1, TestType>;
  EXPECT_TRUE((std::same_as<type1, cina::strong_type<struct Type1, TestType>>));

  using type2 = cina::new_type<struct Type2, int, cina::no_skills>;
  EXPECT_TRUE((std::same_as<type2, cina::strong_type<struct Type2, int>>));
  using type3 = cina::new_type<struct Type3, double, cina::no_skills>;
  EXPECT_TRUE((std::same_as<type3, cina::strong_type<struct Type3, double>>));
}