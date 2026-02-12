#include "cina.hpp"

#include <concepts>
#include <format>
#include <gtest/gtest.h>
#include <string>
#include <type_traits>

TEST(TestBoolean, TestCXXProperties) {
  using boolean_type = cina::boolean_type<struct Tag>;

  EXPECT_FALSE(std::is_default_constructible_v<boolean_type>);
  EXPECT_TRUE(std::is_nothrow_copy_constructible_v<boolean_type>);
  EXPECT_TRUE(std::is_nothrow_copy_assignable_v<boolean_type>);
  EXPECT_TRUE(std::is_trivially_copyable_v<boolean_type>);
  EXPECT_TRUE(std::is_nothrow_move_constructible_v<boolean_type>);
  EXPECT_TRUE(std::is_nothrow_move_assignable_v<boolean_type>);
  EXPECT_TRUE(std::is_nothrow_destructible_v<boolean_type>);
  EXPECT_TRUE(std::is_trivially_destructible_v<boolean_type>);

  EXPECT_EQ(sizeof(boolean_type), sizeof(bool));
  EXPECT_EQ(alignof(boolean_type), alignof(bool));

  EXPECT_TRUE(std::equality_comparable<boolean_type>);
  EXPECT_TRUE(std::swappable<boolean_type>);
  EXPECT_TRUE(std::copyable<boolean_type>);
}

TEST(TestBoolean, TestConstructor) {
  using boolean_type = cina::boolean_type<struct BooleanType>;

  const boolean_type b1{true};
  EXPECT_TRUE(b1.unwrap());

  using boolean_Type2 = cina::boolean_type<struct BooleanType2>;
  EXPECT_FALSE((std::constructible_from<boolean_Type2, boolean_type>));
  EXPECT_FALSE((std::constructible_from<boolean_type, boolean_Type2>));
}

TEST(TestBoolean, TestAssignment) {
  using boolean_type1 = cina::boolean_type<struct BooleanType>;
  using boolean_type2 = cina::boolean_type<struct BooleanType2>;

  EXPECT_FALSE((std::is_assignable_v<boolean_type1&, boolean_type2>));
}

TEST(TestBoolean, TestFormat) {
  using boolean_type = cina::boolean_type<struct BooleanType>;

  const std::string str = std::format("{}", boolean_type{true});
  EXPECT_STREQ(str.c_str(), "true");
}

TEST(TestBoolean, TestHash) {
  using boolean_type = cina::boolean_type<struct BooleanType>;

  const boolean_type b1{true};
  const boolean_type b2{false};
  const std::size_t hash1 = std::hash<boolean_type>{}(b1);
  const std::size_t hash2 = std::hash<boolean_type>{}(b2);

  EXPECT_EQ(hash1, std::hash<bool>{}(true));
  EXPECT_EQ(hash2, std::hash<bool>{}(false));
}

TEST(TestBoolean, TestLess) {
  using boolean_type = cina::boolean_type<struct BooleanType>;

  const boolean_type b1{true};
  const boolean_type b2{false};

  EXPECT_EQ(b1 < b2, true < false);
}

TEST(TestBoolean, TestEquality) {
  using boolean_type = cina::boolean_type<struct BooleanType>;

  const boolean_type b1{true};
  const boolean_type b2{false};

  EXPECT_EQ(b1, b1);
  EXPECT_NE(b1, b2);
  EXPECT_NE(b2, b1);
}

TEST(TestBoolean, TestOutputStream) {
  using boolean_type = cina::boolean_type<struct BooleanType>;

  const boolean_type b1{true};
  const boolean_type b2{false};

  std::ostringstream oss;
  oss << std::boolalpha << b1 << " " << b2;
  EXPECT_STREQ(oss.str().c_str(), "true false");
}

TEST(TestBoolean, TestContextualConversion) {
  using boolean_type = cina::boolean_type<struct BooleanType>;

  enum DontCare { cDONT_CARE_FALSE, cDONT_CARE_TRUE };

  const boolean_type b1{true};
  const DontCare dc = b1 ? cDONT_CARE_TRUE : cDONT_CARE_FALSE;
  EXPECT_EQ(dc, cDONT_CARE_TRUE);
}

TEST(TestBoolean, TestLogicalAnd) {
  using boolean_type = cina::boolean_type<struct BooleanType>;

  const boolean_type b1{true};
  const boolean_type b2{false};
  const boolean_type b3{true};

  EXPECT_EQ(b1 && b2, boolean_type{false});
  EXPECT_EQ(b1 && b3, boolean_type{true});
  EXPECT_EQ(b2 && b3, boolean_type{false});
}

TEST(TestBoolean, TestLogicalOr) {
  using boolean_type = cina::boolean_type<struct BooleanType>;

  const boolean_type b1{true};
  const boolean_type b2{false};
  const boolean_type b3{true};

  EXPECT_EQ(b1 || b2, boolean_type{true});
  EXPECT_EQ(b1 || b3, boolean_type{true});
  EXPECT_EQ(b2 || b3, boolean_type{true});
}

TEST(TestBoolean, TestLogicalNot) {
  using boolean_type = cina::boolean_type<struct BooleanType>;

  const boolean_type b1{true};
  const boolean_type b2{false};

  EXPECT_EQ(!b1, boolean_type{false});
  EXPECT_EQ(!b2, boolean_type{true});
}

TEST(TestBoolean, TestTypeFactory) {
  using boolean_type = cina::new_type<struct BooleanType, bool>;

  EXPECT_TRUE((std::same_as<boolean_type,
                            cina::boolean_type<typename boolean_type::tag>>));
  EXPECT_FALSE((std::same_as<cina::new_type<struct BooleanType, bool>,
                             cina::new_type<struct BooleanType2, bool>>));

  using boolean_type2 = cina::subtype<boolean_type>;
  EXPECT_TRUE((std::same_as<boolean_type2, boolean_type>));
}
