#include "ada_types.hpp"

#include <compare>
#include <concepts>
#include <gtest/gtest.h>
#include <stdexcept>
#include <type_traits>

using namespace ada_types;

template <auto V> constexpr auto value = V;

TEST(TestSignedInteger, TestCXXProperties) {
  using test_type = signed_integer<int, 0, 32, "tag">;

  EXPECT_TRUE(std::is_nothrow_default_constructible_v<test_type>);
  EXPECT_TRUE(std::is_nothrow_copy_constructible_v<test_type>);
  EXPECT_TRUE(std::is_nothrow_move_constructible_v<test_type>);
  EXPECT_TRUE(std::is_nothrow_copy_assignable_v<test_type>);
  EXPECT_TRUE(std::is_nothrow_move_assignable_v<test_type>);
  EXPECT_TRUE(std::equality_comparable<test_type>);
  EXPECT_TRUE(std::three_way_comparable<test_type>);

  EXPECT_EQ(sizeof(test_type), sizeof(int));
  EXPECT_EQ(alignof(test_type), alignof(int));
}

TEST(TestSignedInteger, TestAdaProperties) {
  using test_type = signed_integer<int, 0, 32, "tag">;

  EXPECT_TRUE((std::same_as<test_type::underlying_type, int>));
  EXPECT_EQ(test_type::first, 0);
  EXPECT_EQ(test_type::last, 32);
}

TEST(TestSignedInteger, TestDefaultConstructor) {
  using test_type = signed_integer<int, 0, 32, "tag">;

  const test_type t1;
  EXPECT_EQ(t1.get_value(), 0);

  constexpr test_type t2;
  constexpr int actual_value = t2.get_value();
  EXPECT_EQ(actual_value, 0);
}

TEST(TestSignedInteger, TestExplicitConstructor) {
  using test_type = signed_integer<int, 0, 32, "tag">;

  const test_type t1{0};
  EXPECT_EQ(t1.get_value(), 0);

  const test_type t2{32};
  EXPECT_EQ(t2.get_value(), 32);

  EXPECT_THROW(test_type{-1}, std::out_of_range);
  EXPECT_THROW(test_type{33}, std::out_of_range);

  constexpr test_type t3{0};
  EXPECT_EQ(t3.get_value(), 0);

  constexpr test_type t4{32};
  EXPECT_EQ(t4.get_value(), 32);
}

TEST(TestSignedInteger, TestConvertingConstructor) {
  using test_type1 = signed_integer<int, 0, 32, "tag">;
  using test_type2 = signed_integer<int, 1, 31, "tag">;
  using test_type3 = signed_integer<int, 0, 35, "tag">;

  const test_type2 t1{20};
  const test_type1 t2{t1};

  EXPECT_EQ(t2.get_value(), 20);

  const test_type3 t3{35};
  EXPECT_THROW(test_type1{t3}, std::out_of_range);
}

TEST(TestSignedInteger, TestIncrement) {
  using test_type = signed_integer<int, 0, 32, "tag">;

  test_type t1{10};
  auto t2 = ++t1;
  EXPECT_EQ(t1.get_value(), 11);
  EXPECT_EQ(t2.get_value(), 11);

  auto t3 = t1++;
  EXPECT_EQ(t1.get_value(), 12);
  EXPECT_EQ(t3.get_value(), 11);

  test_type t4{32};
  EXPECT_THROW(++t4, std::out_of_range);

  test_type t5{32};
  EXPECT_THROW(t5++, std::out_of_range);
}

TEST(TestSigndInteger, TestAddition) {
  using test_type = signed_integer<int, 0, 32, "tag">;
  using test_type2 = signed_integer<int, 1, 31, "tag">;

  test_type t1{10};
  t1 += test_type{20};
  EXPECT_EQ(t1.get_value(), 30);

  EXPECT_THROW(t1 += test_type{20}, std::out_of_range);

  test_type t2{10};
  t2 += test_type2{20};
  EXPECT_EQ(t2.get_value(), 30);

  EXPECT_THROW(t2 += test_type2{20}, std::out_of_range);
}

TEST(TestSignedInteger, TestTypeFactory) {
  using test_type1 = new_type<"int1", range<0, 32>>;

  EXPECT_TRUE((std::same_as<test_type1::underlying_type, std::int8_t>));
  EXPECT_EQ(test_type1::first, 0);
  EXPECT_EQ(test_type1::last, 32);
  EXPECT_EQ(test_type1::tag, type_tag{"int1"});

  using subtype1 = sub_type<test_type1>;
  EXPECT_TRUE((std::same_as<subtype1::underlying_type, std::int8_t>));
  EXPECT_EQ(subtype1::first, 0);
  EXPECT_EQ(subtype1::last, 32);
  EXPECT_EQ(subtype1::tag, type_tag{"int1"});

  using subtype2 = sub_type<test_type1, range<1, 31>>;
  EXPECT_TRUE((std::same_as<subtype2::underlying_type, std::int8_t>));
  EXPECT_EQ(subtype2::first, 1);
  EXPECT_EQ(subtype2::last, 31);
  EXPECT_EQ(subtype2::tag, type_tag{"int1"});
}

TEST(TestSignedInteger, TestValue) {
  using test_type = new_type<"int", range<0, 32>>;

  const auto v = test_type::value("10");
  EXPECT_EQ(v.get_value(), 10);

  EXPECT_THROW(test_type::value("33"), std::out_of_range);
  EXPECT_THROW(test_type::value("-1"), std::out_of_range);
  EXPECT_THROW(test_type::value("-1asdg"), std::invalid_argument);
}

TEST(TestSignedInteger, TestComparison) {
  using test_type = new_type<"int", range<0, 32>>;
  test_type t1{1};
  test_type t2{2};

  EXPECT_EQ(t1, t1);
  EXPECT_NE(t1, t2);
  EXPECT_LT(t1, t2);
  EXPECT_GT(t2, t1);
  EXPECT_LE(t1, t1);
  EXPECT_LE(t1, t2);
  EXPECT_GE(t2, t2);
  EXPECT_GE(t2, t1);
}