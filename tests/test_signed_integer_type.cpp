#include <cina.hpp>

#include <concepts>
#include <gtest/gtest.h>

#include <compare>

using namespace cina;

TEST(TestSignedIntegerType, TestCXXProperties) {
  using signed_integer = signed_integral_type<"Test", int>;

  EXPECT_FALSE(std::default_initializable<signed_integer>);
  EXPECT_TRUE(std::move_constructible<signed_integer>);
  EXPECT_TRUE(std::copy_constructible<signed_integer>);
  EXPECT_TRUE(std::movable<signed_integer>);
  EXPECT_TRUE(std::copyable<signed_integer>);
  EXPECT_TRUE(std::equality_comparable<signed_integer>);
  EXPECT_TRUE(std::totally_ordered<signed_integer>);
  EXPECT_TRUE(
      (std::three_way_comparable<signed_integer, std::strong_ordering>));

  EXPECT_TRUE(std::is_nothrow_copy_constructible_v<signed_integer>);
  EXPECT_TRUE(std::is_trivially_copy_constructible_v<signed_integer>);
  EXPECT_TRUE(std::is_trivially_copyable_v<signed_integer>);
  EXPECT_TRUE(std::is_nothrow_move_constructible_v<signed_integer>);
  EXPECT_TRUE(std::is_trivially_move_constructible_v<signed_integer>);
  EXPECT_TRUE(std::is_nothrow_copy_assignable_v<signed_integer>);
  EXPECT_TRUE(std::is_trivially_copy_assignable_v<signed_integer>);
  EXPECT_TRUE(std::is_nothrow_move_assignable_v<signed_integer>);
  EXPECT_TRUE(std::is_trivially_move_assignable_v<signed_integer>);
  EXPECT_TRUE(std::is_trivially_destructible_v<signed_integer>);

  EXPECT_EQ(sizeof(signed_integer), sizeof(int));
  EXPECT_EQ(alignof(signed_integer), alignof(int));
}

TEST(TestSignedIntegerType, TestCinaConcepts) {
  using signed_integer = signed_integral_type<"Test", int>;

  EXPECT_TRUE(strong_type_like<signed_integer>);
  EXPECT_TRUE(signed_integral<signed_integer>);
  EXPECT_TRUE(integral<signed_integer>);
  EXPECT_TRUE((std::same_as<underlying_type<signed_integer>, int>));

  struct signed_integer2 : signed_integral_type<"Test2", int> {
    using signed_integral_type<"Test2", int>::signed_integral_type;
  };

  EXPECT_TRUE(strong_type_like<signed_integer2>);
  EXPECT_TRUE(signed_integral<signed_integer2>);
  EXPECT_TRUE(integral<signed_integer2>);
  EXPECT_TRUE((std::same_as<underlying_type<signed_integer2>, int>));
}

TEST(TestSignedIntegerType, TestTypeFactory) {
  using signed_integer1 = new_type<"SignedInteger1", int>;
  using signed_integer2 = new_type<"SignedInteger2", const int>;
  using signed_integer3 = new_type<"SignedInteger3", int&>;
  using signed_integer4 = new_type<"SignedInteger4", const int&>;

  EXPECT_TRUE((std::same_as<signed_integer1,
                            signed_integral_type<"SignedInteger1", int>>));
  EXPECT_TRUE(
      (std::same_as<signed_integer2,
                    signed_integral_type<"SignedInteger2", const int>>));
  EXPECT_TRUE((std::same_as<signed_integer3,
                            signed_integral_type<"SignedInteger3", int&>>));
  EXPECT_TRUE(
      (std::same_as<signed_integer4,
                    signed_integral_type<"SignedInteger4", const int&>>));
}