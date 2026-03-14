#include "cina.hpp"

#include <concepts>
#include <format>
#include <gtest/gtest.h>
#include <string>
#include <type_traits>

TEST(TestBoolean, TestCXXProperties) {
  using boolean_type = cina::boolean_type<struct Tag, bool>;

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

TEST(TestBoolean, TestCinaConcepts) {
  using boolean_type = cina::boolean_type<struct BooleanType, bool>;
  EXPECT_TRUE(cina::strong_type_like<boolean_type>);

  EXPECT_TRUE((std::same_as<cina::underlying_type<boolean_type>, bool>));

  struct Type : cina::boolean_type<Type, bool> {};
  EXPECT_TRUE(cina::strong_type_like<Type>);
  EXPECT_TRUE((std::same_as<cina::underlying_type<Type>, bool>));
}

TEST(TestBoolean, TestConstructor) {
  using boolean_type = cina::boolean_type<struct BooleanType, bool>;

  const boolean_type b1{true};
  EXPECT_TRUE(b1.unwrap());

  using boolean_type2 = cina::boolean_type<struct BooleanType2, bool>;
  EXPECT_FALSE((std::constructible_from<boolean_type, boolean_type2>));

  EXPECT_FALSE((std::convertible_to<bool, boolean_type>));
  EXPECT_FALSE((std::convertible_to<boolean_type, bool>));

  using boolean_reference = boolean_type::reference;
  bool b{true};
  boolean_reference ref{b};
  EXPECT_TRUE(ref.unwrap());

  boolean_type::const_reference ref2{b};
  EXPECT_TRUE(ref2.unwrap());

  boolean_type b3{true};
  EXPECT_TRUE(b3.unwrap());
  boolean_reference ref3{b3};
  const bool b5 = ref3.unwrap();
  EXPECT_EQ(b5, true);

  boolean_type::const_reference ref4{b3};
  const bool b6 = ref4.unwrap();
  EXPECT_EQ(b6, true);

  EXPECT_FALSE((std::constructible_from<boolean_type::reference, const bool&>));
}

TEST(TestBoolean, TestAssignment) {
  using boolean_type1 = cina::boolean_type<struct BooleanType, bool>;
  using boolean_type2 = cina::boolean_type<struct BooleanType2, bool>;

  EXPECT_FALSE((std::assignable_from<boolean_type1, boolean_type2>));

  using boolean_reference = boolean_type1::reference;
  bool b{true};
  boolean_reference ref{b};
  ref.unwrap() = false;
  EXPECT_FALSE(b);
  EXPECT_FALSE(ref.unwrap());

  boolean_type1 b1{true};
  ref = b1;
  EXPECT_TRUE(ref.unwrap());
  EXPECT_TRUE(b);
}

TEST(TestBoolean, TestFormat) {
  using boolean_type = cina::boolean_type<struct BooleanType, bool>;

  const std::string str = std::format("{}", boolean_type{true});
  EXPECT_STREQ(str.c_str(), "true");

  bool b{true};
  boolean_type::reference ref{b};
  const std::string str2 = std::format("{}", ref);
  EXPECT_STREQ(str2.c_str(), "true");

  // const boolean_type b2{true};
}

TEST(TestBoolean, TestHash) {
  using boolean_type = cina::boolean_type<struct BooleanType, bool>;

  const boolean_type b1{true};
  const boolean_type b2{false};
  const std::size_t hash1 = std::hash<boolean_type>{}(b1);
  const std::size_t hash2 = std::hash<boolean_type>{}(b2);

  EXPECT_EQ(hash1, std::hash<bool>{}(true));
  EXPECT_EQ(hash2, std::hash<bool>{}(false));
}

TEST(TestBoolean, TestEquality) {
  using boolean_type = cina::boolean_type<struct BooleanType, bool>;

  const boolean_type b1{true};
  const boolean_type b2{false};

  EXPECT_EQ(b1, b1);
  EXPECT_NE(b1, b2);
  EXPECT_NE(b2, b1);

  EXPECT_FALSE((std::equality_comparable_with<boolean_type, bool>));

  using boolean_type2 = cina::boolean_type<struct BooleanType2, bool>;
  EXPECT_FALSE((std::equality_comparable_with<boolean_type, boolean_type2>));
}

TEST(TestBoolean, TestLess) {
  using boolean_type = cina::boolean_type<struct BooleanType, bool>;

  const boolean_type b1{true};
  const boolean_type b2{false};

  EXPECT_EQ(b1 < b2, true < false);
}

TEST(TestBoolean, TestOutputStream) {
  using boolean_type = cina::boolean_type<struct BooleanType, bool>;

  const boolean_type b1{true};
  const boolean_type b2{false};

  std::ostringstream oss;
  oss << std::boolalpha << b1 << " " << b2;
  EXPECT_STREQ(oss.str().c_str(), "true false");
}

TEST(TestBoolean, TestContextualConversion) {
  using boolean_type = cina::boolean_type<struct BooleanType, bool>;

  enum DontCare { cDONT_CARE_FALSE, cDONT_CARE_TRUE };

  const boolean_type b1{true};
  const DontCare dc = b1 ? cDONT_CARE_TRUE : cDONT_CARE_FALSE;
  EXPECT_EQ(dc, cDONT_CARE_TRUE);
}

TEST(TestBoolean, TestLogicalAnd) {
  using boolean_type = cina::boolean_type<struct BooleanType, bool>;

  const boolean_type b1{true};
  const boolean_type b2{false};
  const boolean_type b3{true};

  EXPECT_EQ(b1 && b2, boolean_type{false});
  EXPECT_EQ(b1 && b3, boolean_type{true});
  EXPECT_EQ(b2 && b3, boolean_type{false});
}

TEST(TestBoolean, TestLogicalOr) {
  using boolean_type = cina::boolean_type<struct BooleanType, bool>;

  const boolean_type b1{true};
  const boolean_type b2{false};
  const boolean_type b3{true};

  EXPECT_EQ(b1 || b2, boolean_type{true});
  EXPECT_EQ(b1 || b3, boolean_type{true});
  EXPECT_EQ(b2 || b3, boolean_type{true});
}

TEST(TestBoolean, TestLogicalNot) {
  using boolean_type = cina::boolean_type<struct BooleanType, bool>;

  const boolean_type b1{true};
  const boolean_type b2{false};

  EXPECT_EQ(!b1, boolean_type{false});
  EXPECT_EQ(!b2, boolean_type{true});
}

TEST(TestBoolean, TestTypeFactory) {
  using boolean_type = cina::new_type<struct BooleanType, bool>;

  EXPECT_TRUE(
      (std::same_as<boolean_type,
                    cina::boolean_type<typename boolean_type::tag, bool>>));
  EXPECT_FALSE((std::same_as<cina::new_type<struct BooleanType, bool>,
                             cina::new_type<struct BooleanType2, bool>>));

  using boolean_type2 = cina::subtype<boolean_type>;
  EXPECT_TRUE((std::same_as<boolean_type2, boolean_type>));

  using boolean_type3 = cina::new_type<struct BooleanType3, boolean_type>;
  EXPECT_TRUE((std::same_as<boolean_type3,
                            cina::boolean_type<struct BooleanType3, bool>>));
}

namespace {
using boolean_nttp = cina::new_type<struct NTTP, bool>;

template <boolean_nttp B> struct S {
  static constexpr auto value = B;
};
} // namespace

TEST(TestBoolean, TestNTTP) {
  S<boolean_nttp{false}> s;
  EXPECT_FALSE(s.value.unwrap());
}