#include "cina.hpp"

#include <concepts>
#include <gtest/gtest.h>
#include <type_traits>

TEST(TestBitwiseIntegralType, TestCXXProperties) {
  using int_type = cina::bitwise_signed_integral_type<struct IntegerType, int>;

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

  EXPECT_FALSE((std::is_convertible_v<int_type, bool>));
}

TEST(TestBitwiseIntegralType, TestConstructor) {
  using int_type = cina::bitwise_signed_integral_type<struct IntType, int>;

  const int_type i{42};
  EXPECT_EQ(i.unwrap(), 42);

  EXPECT_FALSE((std::constructible_from<int_type, std::int64_t>));
  EXPECT_FALSE((std::constructible_from<int_type, double>));
  EXPECT_FALSE((std::constructible_from<int_type, std::uint8_t>));

  using int_type2 = cina::bitwise_signed_integral_type<struct IntType2, int>;
  EXPECT_FALSE((std::constructible_from<int_type, int_type2>));
}

TEST(TestBitwiseIntegralType, TestAssignment) {
  using int_type1 = cina::bitwise_signed_integral_type<struct IntType1, int>;
  using int_type2 = cina::bitwise_signed_integral_type<struct IntType2, int>;

  EXPECT_FALSE((std::assignable_from<int_type1, int_type2>));
  EXPECT_FALSE((std::assignable_from<int_type1, int>));
}

TEST(TestBitwiseIntegralType, TestCinaConcepts) {
  using int_type1 = cina::bitwise_signed_integral_type<struct IntType1, int>;

  EXPECT_TRUE(cina::strong_type_like<int_type1>);
  EXPECT_TRUE(cina::integral<int_type1>);
  EXPECT_TRUE(cina::arithmetic<int_type1>);

  EXPECT_TRUE((std::same_as<cina::underlying_type<int_type1>, int>));
}

TEST(TestBitwiseIntegralType, TestFormat) {
  using int_type = cina::bitwise_signed_integral_type<struct IntType1, int>;

  const std::string str =
      std::format("{}", int_type{static_cast<std::int8_t>(42)});
  EXPECT_STREQ(str.c_str(), "42");
}

TEST(TestBitwiseIntegralType, TestHash) {
  using int_type = cina::bitwise_signed_integral_type<struct IntType1, int>;

  const int_type i1{42};

  auto hash = std::hash<int_type>{}(i1);
  EXPECT_EQ(std::hash<int>{}(i1.unwrap()), hash);
}

TEST(TestBitwiseIntegralType, TestEquality) {
  using int_type = cina::bitwise_signed_integral_type<struct IntType, int>;

  const int_type i1{42};
  const int_type i2{42};
  const int_type i3{43};

  EXPECT_EQ(i1, i2);
  EXPECT_NE(i1, i3);
  EXPECT_NE(i2, i3);

  EXPECT_FALSE((std::equality_comparable_with<int_type, int>));

  using int_type2 = cina::bitwise_signed_integral_type<struct IntType2, int>;
  EXPECT_FALSE((std::equality_comparable_with<int_type, int_type2>));
}

TEST(TestBitwiseIntegralType, TestComparison) {
  using int_type = cina::bitwise_signed_integral_type<struct IntType, int>;

  const int_type i1{42};
  const int_type i2{43};

  EXPECT_LT(i1, i2);
  EXPECT_GT(i2, i1);
  EXPECT_LE(i1, i2);
  EXPECT_GE(i2, i1);

  EXPECT_EQ(i1 <=> i2, std::strong_ordering::less);
  EXPECT_EQ(i2 <=> i1, std::strong_ordering::greater);
  EXPECT_EQ(i1 <=> i1, std::strong_ordering::equal);

  EXPECT_FALSE((std::totally_ordered_with<int_type, int>));
  EXPECT_FALSE((std::three_way_comparable_with<int_type, int>));

  using int_type2 = cina::bitwise_signed_integral_type<struct IntType2, int>;
  EXPECT_FALSE((std::totally_ordered_with<int_type, int_type2>));
  EXPECT_FALSE((std::three_way_comparable_with<int_type, int_type2>));
}

TEST(TestBitwiseIntegralType, TestOutputStream) {
  using int_type =
      cina::bitwise_signed_integral_type<struct IntType, std::int8_t>;

  const int_type i1{static_cast<std::int8_t>(42)};

  std::ostringstream oss;
  oss << i1;
  EXPECT_STREQ(oss.str().c_str(), "42");
}

TEST(TestBitwiseIntegralType, TestAddition) {
  using int_type = cina::bitwise_signed_integral_type<struct IntType, int>;

  int_type i1{42};
  const int_type i2{10};

  const int_type i3 = i1 += i2;
  EXPECT_EQ(i1.unwrap(), 52);
  EXPECT_EQ(i3.unwrap(), 52);

  const int_type i4 = i1 + i2;
  EXPECT_EQ(i4.unwrap(), 62);
}

TEST(TestBitwiseIntegralType, TestSubtraction) {
  using int_type = cina::bitwise_signed_integral_type<struct IntType, int>;

  int_type i1{42};
  const int_type i2{10};

  const int_type i3 = i1 -= i2;
  EXPECT_EQ(i1.unwrap(), 32);
  EXPECT_EQ(i3.unwrap(), 32);

  const int_type i4 = i1 - i2;
  EXPECT_EQ(i4.unwrap(), 22);
}

TEST(TestBitwiseIntegralType, TestMultiplication) {
  using int_type = cina::bitwise_signed_integral_type<struct IntType, int>;

  int_type i1{42};
  const int_type i2{10};

  const int_type i3 = i1 *= i2;
  EXPECT_EQ(i1.unwrap(), 420);
  EXPECT_EQ(i3.unwrap(), 420);

  const int_type i4 = i1 * i2;
  EXPECT_EQ(i4.unwrap(), 4200);
}

TEST(TestBitwiseIntegralType, TestDivision) {
  using int_type = cina::bitwise_signed_integral_type<struct IntType, int>;

  int_type i1{42};
  const int_type i2{10};

  const int_type i3 = i1 /= i2;
  EXPECT_EQ(i1.unwrap(), 4);
  EXPECT_EQ(i3.unwrap(), 4);

  const int_type i4 = i1 / i2;
  EXPECT_EQ(i4.unwrap(), 0);
}

TEST(TestBitwiseIntegralType, TestModulo) {
  using int_type = cina::bitwise_signed_integral_type<struct IntType, int>;

  int_type i1{42};
  const int_type i2{10};

  const int_type i3 = i1 %= i2;
  EXPECT_EQ(i1.unwrap(), 2);
  EXPECT_EQ(i3.unwrap(), 2);

  const int_type i4 = i1 % i2;
  EXPECT_EQ(i4.unwrap(), 2);
}

TEST(TestBitwiseIntegralType, TestNegation) {
  using int_type = cina::bitwise_signed_integral_type<struct IntType, int>;

  const int_type i1{42};
  const int_type i2 = -i1;
  EXPECT_EQ(i2.unwrap(), -42);
}

TEST(TestBitwiseIntegralType, TestIncrement) {
  using int_type = cina::bitwise_signed_integral_type<struct IntType, int>;

  int_type i1{42};

  const int_type i2 = ++i1;
  EXPECT_EQ(i1.unwrap(), 43);
  EXPECT_EQ(i2.unwrap(), 43);

  const int_type i3 = i1++;
  EXPECT_EQ(i1.unwrap(), 44);
  EXPECT_EQ(i3.unwrap(), 43);
}

TEST(TestBitwiseIntegralType, TestDecrement) {
  using int_type = cina::bitwise_signed_integral_type<struct IntType, int>;

  int_type i1{42};

  const int_type i2 = --i1;
  EXPECT_EQ(i1.unwrap(), 41);
  EXPECT_EQ(i2.unwrap(), 41);

  const int_type i3 = i1--;
  EXPECT_EQ(i1.unwrap(), 40);
  EXPECT_EQ(i3.unwrap(), 41);
}

TEST(TestBitwiseIntegralType, TestBitwiseAnd) {
  using int_type = cina::bitwise_signed_integral_type<struct IntType, int>;

  const int_type i1{10};
  const int_type i2{20};
  EXPECT_EQ((i1 & i2).unwrap(), (10 & 20));

  int_type i3{10};
  i3 &= i2;
  EXPECT_EQ(i3.unwrap(), (10 & 20));
}

TEST(TestBitwiseIntegralType, TestBitwiseOr) {
  using int_type = cina::bitwise_signed_integral_type<struct IntType, int>;

  const int_type i1{10};
  const int_type i2{20};
  EXPECT_EQ((i1 | i2).unwrap(), (10 | 20));

  int_type i3{10};
  i3 |= i2;
  EXPECT_EQ(i3.unwrap(), (10 | 20));
}

TEST(TestBitwiseIntegralType, TestBitwiseXor) {
  using int_type = cina::bitwise_signed_integral_type<struct IntType, int>;

  const int_type i1{10};
  const int_type i2{20};
  EXPECT_EQ((i1 ^ i2).unwrap(), (10 ^ 20));

  int_type i3{10};
  i3 ^= i2;
  EXPECT_EQ(i3.unwrap(), (10 ^ 20));
}

TEST(TestBitwiseIntegralType, TestBitwiseNot) {
  using int_type = cina::bitwise_signed_integral_type<struct IntType, int>;

  const int_type i1{10};
  EXPECT_EQ(~i1.unwrap(), ~10);
}

TEST(TestBitwiseIntegralType, TestBitwiseShift) {
  using int_type = cina::bitwise_signed_integral_type<struct IntType, int>;

  const int_type i1{1};
  const int_type i2 = i1 << int_type{2};
  EXPECT_EQ(i2.unwrap(), 1 << 2);

  const int_type i3{1 << 2};
  const int_type i4 = i3 >> int_type{2};
  EXPECT_EQ(i4.unwrap(), ((1 << 2)) >> 2);

  int_type i5{1};
  i5 <<= int_type{2};
  EXPECT_EQ(i5.unwrap(), 1 << 2);

  int_type i6{1 << 2};
  i6 >>= int_type{2};
  EXPECT_EQ(i6.unwrap(), 1);
}

TEST(TestBitwiseIntegralType, TestTypeFactory) {
  using int_type1 =
      cina::new_type<struct IntType1, std::int8_t, cina::enable_bitwise>;

  EXPECT_TRUE(
      (std::same_as<int_type1, cina::bitwise_signed_integral_type<
                                   typename int_type1::tag, std::int8_t>>));

  using int_type2 =
      cina::new_type<struct IntType2, std::int8_t, cina::enable_bitwise>;
  EXPECT_FALSE((std::same_as<int_type1, int_type2>));

  using int_type3 = cina::subtype<int_type1>;
  EXPECT_TRUE((std::same_as<int_type1, int_type3>));

  using int_type4 = cina::new_type<struct IntType3, int_type1>;
  EXPECT_TRUE((std::same_as<int_type4, cina::bitwise_signed_integral_type<
                                           struct IntType3, std::int8_t>>));
}

namespace {
using nttp_type = cina::new_type<struct NTTP, int, cina::enable_bitwise>;

template <nttp_type I> struct S {
  static constexpr nttp_type value = I;
};
} // namespace

TEST(TestBitwiseIntegralType, TestNTTP) {
  S<nttp_type{1}> s;
  EXPECT_EQ(s.value.unwrap(), 1);
}