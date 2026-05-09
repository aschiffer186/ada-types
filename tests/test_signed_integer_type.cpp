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
  EXPECT_FALSE((std::convertible_to<signed_integer, bool>));

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

  struct integral_type2 : new_type<"SignedInteger2", int> {
    using new_type<"SignedInteger2", int>::new_type;
  };

  integral_type2 i3{42};
  integral_type2 i4{43};

  EXPECT_EQ(i3 <=> i3, std::strong_ordering::equal);
  EXPECT_EQ(i3 <=> i4, std::strong_ordering::less);
  EXPECT_EQ(i4 <=> i3, std::strong_ordering::greater);

  EXPECT_EQ(i3, i3);
  EXPECT_NE(i3, i4);
  EXPECT_LE(i3, i3);
  EXPECT_LE(i3, i4);
  EXPECT_GE(i3, i3);
  EXPECT_GE(i4, i3);
  EXPECT_LT(i3, i4);
  EXPECT_GT(i4, i3);

  EXPECT_FALSE((std::equality_comparable_with<integral_type, integral_type2>));
  EXPECT_FALSE((std::three_way_comparable_with<integral_type, integral_type2>));

  using integer_ref = new_type<"IntegerRef", int&>;
  int i5{44};
  integer_ref i6{i5};
  int i7{45};
  integer_ref i8{i7};

  EXPECT_EQ(i6 <=> i6, std::strong_ordering::equal);
  EXPECT_EQ(i6 <=> i8, std::strong_ordering::less);
  EXPECT_EQ(i8 <=> i6, std::strong_ordering::greater);
  EXPECT_EQ(i6, i6);
  EXPECT_NE(i6, i8);
  EXPECT_LE(i6, i6);
  EXPECT_LE(i6, i8);
  EXPECT_GE(i6, i6);
  EXPECT_GE(i8, i6);
  EXPECT_LT(i6, i8);
  EXPECT_GT(i8, i6);

  using const_integer_ref = new_type<"ConstIntegerRef", const int&>;
  const int i9{46};
  const_integer_ref i10{i9};
  const int i11{47};
  const_integer_ref i12{i11};

  EXPECT_EQ(i10 <=> i10, std::strong_ordering::equal);
  EXPECT_EQ(i10 <=> i12, std::strong_ordering::less);
  EXPECT_EQ(i12 <=> i10, std::strong_ordering::greater);
  EXPECT_EQ(i10, i10);
  EXPECT_NE(i10, i12);
  EXPECT_LE(i10, i10);
  EXPECT_LE(i10, i12);
  EXPECT_GE(i10, i10);
  EXPECT_GE(i12, i10);
  EXPECT_LT(i10, i12);
  EXPECT_GT(i12, i10);
}

TEST(TestSignedIntegerType, TestAddition) {
  using integer_type = cina::new_type<"Integer", int>;

  integer_type i1{42};
  integer_type i2{43};

  integer_type i3 = i1 + i2;
  EXPECT_EQ(i3.unwrap(), 85);

  i3 += i3;
  EXPECT_EQ(i3.unwrap(), 170);

  using integer_ref = cina::new_type<"IntegerRef", int&>;

  int i4{44};
  integer_ref i5{i4};
  int i6{45};
  integer_ref i7{i6};

  auto i8 = i5 + i7;
  EXPECT_EQ(i8.unwrap(), 89);

  int i9{46};
  integer_ref i10{i9};
  i7 += i10;
  EXPECT_EQ(i7.unwrap(), 91);
  EXPECT_EQ(i6, 91);
}

TEST(TestSignedIntegerType, TestSubtraction) {
  using integer_type = cina::new_type<"Integer", int>;

  integer_type i1{42};
  integer_type i2{43};

  integer_type i3 = i1 - i2;
  EXPECT_EQ(i3.unwrap(), -1);

  i3 -= i3;
  EXPECT_EQ(i3.unwrap(), 0);

  using integer_ref = cina::new_type<"IntegerRef", int&>;

  int i4{44};
  integer_ref i5{i4};
  int i6{45};
  integer_ref i7{i6};

  auto i8 = i5 - i7;
  EXPECT_EQ(i8.unwrap(), -1);

  int i9{46};
  integer_ref i10{i9};
  i7 -= i10;
  EXPECT_EQ(i7.unwrap(), -1);
  EXPECT_EQ(i6, -1);
}

TEST(TestSignedIntegerType, TestMultiplication) {
  using integer_type = cina::new_type<"Integer", int>;

  integer_type i1{42};
  integer_type i2{43};

  integer_type i3 = i1 * i2;
  EXPECT_EQ(i3.unwrap(), 1806);

  i3 *= i3;
  EXPECT_EQ(i3.unwrap(), 3261636);

  using integer_ref = cina::new_type<"IntegerRef", int&>;

  int i4{44};
  integer_ref i5{i4};
  int i6{45};
  integer_ref i7{i6};

  auto i8 = i5 * i7;
  EXPECT_EQ(i8.unwrap(), 1980);

  int i9{46};
  integer_ref i10{i9};
  i7 *= i10;
  EXPECT_EQ(i7.unwrap(), 2070);
  EXPECT_EQ(i6, 2070);
}