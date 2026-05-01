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

  using signed_integer5 = new_type<"SignedInteger5", std::int8_t>;
  using singed_integer6 = new_type<"SignedInteger6", std::int16_t>;
  using signed_integer7 = new_type<"SignedInteger7", std::int32_t>;
  using signed_integer8 = new_type<"SignedInteger8", std::int64_t>;
  using signed_integer9 = new_type<"SignedInteger9", std::intmax_t>;

  EXPECT_TRUE(
      (std::same_as<signed_integer5,
                    signed_integral_type<"SignedInteger5", std::int8_t>>));
  EXPECT_TRUE(
      (std::same_as<singed_integer6,
                    signed_integral_type<"SignedInteger6", std::int16_t>>));
  EXPECT_TRUE(
      (std::same_as<signed_integer7,
                    signed_integral_type<"SignedInteger7", std::int32_t>>));
  EXPECT_TRUE(
      (std::same_as<signed_integer8,
                    signed_integral_type<"SignedInteger8", std::int64_t>>));
  EXPECT_TRUE(
      (std::same_as<signed_integer9,
                    signed_integral_type<"SignedInteger9", std::intmax_t>>));
}

TEST(TestSignedIntegerType, TestConstructor) {
  using signed_integer = new_type<"SignedInteger", int>;

  signed_integer i1{42};
  EXPECT_EQ(i1.unwrap(), 42);

  using signed_integer2 = new_type<"SignedInteger2", int>;
  EXPECT_FALSE((std::constructible_from<signed_integer, signed_integer2>));
  EXPECT_FALSE((std::constructible_from<signed_integer2, signed_integer>));

  struct signed_integer3 : new_type<"SignedInteger2", int> {
    using new_type<"SignedInteger2", int>::new_type;
  };
  signed_integer3 i2{43};
  EXPECT_EQ(i2.unwrap(), 43);

  using signed_integer_ref = new_type<"SignedIntegerRef", int&>;
  int i3{44};
  signed_integer_ref i4{i3};
  EXPECT_EQ(i4.unwrap(), 44);
  EXPECT_FALSE((std::constructible_from<signed_integer_ref, int&&>));
  EXPECT_FALSE((std::constructible_from<signed_integer_ref, const int&>));
  EXPECT_FALSE((std::constructible_from<signed_integer_ref, const int&&>));

  using const_signed_integer_ref =
      new_type<"ConstSignedIntegerRef", const int&>;
  const int i5{45};
  const_signed_integer_ref i6{i5};
  EXPECT_EQ(i6.unwrap(), 45);
  EXPECT_TRUE((std::constructible_from<const_signed_integer_ref, int&>));
  EXPECT_FALSE((std::constructible_from<const_signed_integer_ref, int&&>));
}

TEST(TestSignedIntegerType, TestComparisons) {
  using integral_type = new_type<"SignedInteger", int>;

  integral_type i1{42};
  integral_type i2{43};

  EXPECT_EQ(i1 <=> i1, std::strong_ordering::equal);
  EXPECT_EQ(i1 <=> i2, std::strong_ordering::less);
  EXPECT_EQ(i2 <=> i1, std::strong_ordering::greater);

  EXPECT_EQ(i1, i1);
  EXPECT_NE(i1, i2);
  EXPECT_LE(i1, i1);
  EXPECT_LE(i1, i2);
  EXPECT_GE(i1, i1);
  EXPECT_GE(i2, i1);
  EXPECT_LT(i1, i2);
  EXPECT_GT(i2, i1);
}