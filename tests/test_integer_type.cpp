#ifndef BUILD_MODULE
#include "cina.hpp"
#else
import cina;
#endif

#include <concepts>
#include <gtest/gtest.h>

TEST(TestIntegerType, TestCXXProperties) {
  using int_type = cina::signed_integral_type<struct IntType, int>;

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

TEST(TestIntegerType, TestConstructor) {
  using int_type = cina::signed_integral_type<struct IntType, int>;

  const int_type i1{42};
  EXPECT_EQ(i1.unwrap(), 42);

  EXPECT_FALSE((std::constructible_from<int_type, std::int64_t>));
  EXPECT_FALSE((std::constructible_from<int_type, double>));
  EXPECT_FALSE((std::constructible_from<int_type, std::uint8_t>));
  EXPECT_FALSE((std::constructible_from<int_type, bool>));
  EXPECT_FALSE((std::constructible_from<int_type, char16_t>));
  EXPECT_FALSE((std::constructible_from<int_type, char32_t>));
  EXPECT_FALSE((std::constructible_from<int_type, wchar_t>));

  using int_type2 = cina::signed_integral_type<struct IntType2, int>;
  EXPECT_FALSE((std::constructible_from<int_type, int_type2>));

  EXPECT_FALSE((std::convertible_to<int, int_type>));
  EXPECT_FALSE((std::convertible_to<int_type, int>));

  int i{42};
  cina::signed_integral_type<struct IntType2, int&> ref{i};
  int& i2 = ref.unwrap();
  EXPECT_EQ(i2, 42);
}

TEST(TestIntegerType, TestAssignment) {
  using int_type1 = cina::signed_integral_type<struct IntType, int>;
  using int_type2 = cina::signed_integral_type<struct IntType2, int>;

  EXPECT_FALSE((std::assignable_from<int_type1, int_type2>));
  EXPECT_FALSE((std::assignable_from<int_type1, int>));

  using int_type3 = cina::signed_integral_type<struct IntType3, int&>;
  int i{42};
  int_type3 ref{i};
  ref.unwrap() = 10;
  EXPECT_EQ(i, 10);
  EXPECT_EQ(ref.unwrap(), 10);
}

TEST(TestIntegerType, TestCinaConcepts) {
  using int_type = cina::signed_integral_type<struct IntType, int>;
  EXPECT_TRUE(cina::strong_type_like<int_type>);
  EXPECT_TRUE(cina::integral<int_type>);
  EXPECT_TRUE(cina::arithmetic<int_type>);

  EXPECT_TRUE((std::same_as<cina::underlying_type<int_type>, int>));

  struct Type : cina::signed_integral_type<Type, int> {};
  EXPECT_TRUE(cina::strong_type_like<Type>);
  EXPECT_TRUE(cina::integral<Type>);
  EXPECT_TRUE(cina::arithmetic<Type>);
  EXPECT_TRUE((std::same_as<cina::underlying_type<Type>, int>));

  using int_reference = cina::signed_integral_type<struct Tag, int&>;
  EXPECT_TRUE(cina::strong_type_like<int_reference>);
  EXPECT_TRUE(cina::integral<int_reference>);
  EXPECT_TRUE(cina::arithmetic<int_reference>);
  EXPECT_TRUE((std::same_as<cina::underlying_type<int_reference>, int&>));

  using const_int_reference =
      cina::signed_integral_type<struct Tag2, const int&>;
  EXPECT_TRUE(cina::strong_type_like<const_int_reference>);
  EXPECT_TRUE(cina::integral<int_reference>);
  EXPECT_TRUE(cina::arithmetic<int_reference>);
  EXPECT_TRUE(
      (std::same_as<cina::underlying_type<const_int_reference>, const int&>));
}

TEST(TestIntegerType, TestFormat) {
  using int_type = cina::signed_integral_type<struct IntType, std::int8_t>;

  const std::string str =
      std::format("{}", int_type{static_cast<std::int8_t>(42)});
  EXPECT_STREQ(str.c_str(), "42");

  using int_reference = cina::signed_integral_type<struct Tag, int&>;
  int i{42};
  const std::string str2 = std::format("{}", int_reference{i});
  EXPECT_STREQ(str2.c_str(), "42");

  using const_int_reference =
      cina::signed_integral_type<struct Tag2, const int&>;
  const std::string str3 = std::format("{}", const_int_reference{i});
  EXPECT_STREQ(str3.c_str(), "42");
}

TEST(TestIntegerType, TestHash) {
  using int_type = cina::signed_integral_type<struct IntType, int>;

  const int_type i1{42};

  auto hash = std::hash<int_type>{}(i1);
  EXPECT_EQ(std::hash<int>{}(i1.unwrap()), hash);
}

TEST(TestIntegerType, TestEquality) {
  using int_type = cina::signed_integral_type<struct IntType, int>;

  const int_type i1{42};
  const int_type i2{42};
  const int_type i3{43};

  EXPECT_EQ(i1, i2);
  EXPECT_NE(i1, i3);
  EXPECT_NE(i2, i3);

  EXPECT_FALSE((std::equality_comparable_with<int_type, int>));

  using int_type2 = cina::signed_integral_type<struct IntType2, int>;
  EXPECT_FALSE((std::equality_comparable_with<int_type, int_type2>));

  using int_reference = cina::signed_integral_type<struct IntType2, int&>;
  int val{42};
  int val2{43};
  int_reference ref{val};
  int_reference ref2{val2};

  EXPECT_EQ(ref, ref);
  EXPECT_NE(ref, ref2);

  using const_int_reference =
      cina::signed_integral_type<struct IntType3, const int&>;
  const_int_reference cref{val};
  const_int_reference cref2{val2};
  EXPECT_EQ(cref, cref);
  EXPECT_NE(cref, cref2);
}

TEST(TestIntegerType, TestComparison) {
  using int_type = cina::signed_integral_type<struct IntType, int>;

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

  using int_type2 = cina::signed_integral_type<struct IntType2, int>;
  EXPECT_FALSE((std::totally_ordered_with<int_type, int_type2>));
  EXPECT_FALSE((std::three_way_comparable_with<int_type, int_type2>));
}

TEST(TestIntegerType, TestOutputStream) {
  using int_type = cina::signed_integral_type<struct IntType, std::int8_t>;

  const int_type i1{static_cast<std::int8_t>(42)};

  std::ostringstream oss;
  oss << i1;
  EXPECT_STREQ(oss.str().c_str(), "42");
}

TEST(TestIntegerType, TestAddition) {
  using int_type = cina::signed_integral_type<struct IntType, int>;

  int_type i1{42};
  const int_type i2{10};

  const int_type i3 = i1 += i2;
  EXPECT_EQ(i1.unwrap(), 52);
  EXPECT_EQ(i3.unwrap(), 52);

  const int_type i4 = i1 + i2;
  EXPECT_EQ(i4.unwrap(), 62);
}

TEST(TestIntegerType, TestSubtraction) {
  using int_type = cina::signed_integral_type<struct IntType, int>;

  int_type i1{42};
  const int_type i2{10};

  const int_type i3 = i1 -= i2;
  EXPECT_EQ(i1.unwrap(), 32);
  EXPECT_EQ(i3.unwrap(), 32);

  const int_type i4 = i1 - i2;
  EXPECT_EQ(i4.unwrap(), 22);
}

TEST(TestIntegerType, TestMultiplication) {
  using int_type = cina::signed_integral_type<struct IntType, int>;

  int_type i1{42};
  const int_type i2{10};

  const int_type i3 = i1 *= i2;
  EXPECT_EQ(i1.unwrap(), 420);
  EXPECT_EQ(i3.unwrap(), 420);

  const int_type i4 = i1 * i2;
  EXPECT_EQ(i4.unwrap(), 4200);
}

TEST(TestIntegerType, TestDivision) {
  using int_type = cina::signed_integral_type<struct IntType, int>;

  int_type i1{42};
  const int_type i2{10};

  const int_type i3 = i1 /= i2;
  EXPECT_EQ(i1.unwrap(), 4);
  EXPECT_EQ(i3.unwrap(), 4);

  const int_type i4 = i1 / i2;
  EXPECT_EQ(i4.unwrap(), 0);
}

TEST(TestIntegerType, TestModulo) {
  using int_type = cina::signed_integral_type<struct IntType, int>;

  int_type i1{42};
  const int_type i2{10};

  const int_type i3 = i1 %= i2;
  EXPECT_EQ(i1.unwrap(), 2);
  EXPECT_EQ(i3.unwrap(), 2);

  const int_type i4 = i1 % i2;
  EXPECT_EQ(i4.unwrap(), 2);
}

TEST(TestIntegerType, TestNegation) {
  using int_type = cina::signed_integral_type<struct IntType, int>;

  const int_type i1{42};
  const int_type i2 = -i1;
  EXPECT_EQ(i2.unwrap(), -42);
}

TEST(TestIntegerType, TestIncrement) {
  using int_type = cina::signed_integral_type<struct IntType, int>;

  int_type i1{42};

  const int_type i2 = ++i1;
  EXPECT_EQ(i1.unwrap(), 43);
  EXPECT_EQ(i2.unwrap(), 43);

  const int_type i3 = i1++;
  EXPECT_EQ(i1.unwrap(), 44);
  EXPECT_EQ(i3.unwrap(), 43);
}

TEST(TestIntegerType, TestDecrement) {
  using int_type = cina::signed_integral_type<struct IntType, int>;

  int_type i1{42};

  const int_type i2 = --i1;
  EXPECT_EQ(i1.unwrap(), 41);
  EXPECT_EQ(i2.unwrap(), 41);

  const int_type i3 = i1--;
  EXPECT_EQ(i1.unwrap(), 40);
  EXPECT_EQ(i3.unwrap(), 41);
}

TEST(TestIntegerType, TestTypeFactory) {
  using int_type1 = cina::new_type<struct IntType1, std::int8_t>;

  EXPECT_TRUE(
      (std::same_as<int_type1, cina::signed_integral_type<
                                   typename int_type1::tag, std::int8_t>>));

  using int_type2 = cina::new_type<struct IntType2, std::int8_t>;
  EXPECT_FALSE((std::same_as<int_type1, int_type2>));

  using int_type3 = cina::subtype<int_type1>;
  EXPECT_TRUE((std::same_as<int_type1, int_type3>));

  using int_type4 = cina::new_type<struct IntType4, int_type1>;
  EXPECT_TRUE(
      (std::same_as<int_type4,
                    cina::signed_integral_type<struct IntType4, std::int8_t>>));
}

namespace {
using nttp_type = cina::new_type<struct NTTP, int>;

template <nttp_type I> struct S {
  static constexpr nttp_type value = I;
};
} // namespace

TEST(TestIntegerType, TestNTTP) {
  S<nttp_type{1}> s;
  EXPECT_EQ(s.value.unwrap(), 1);
}