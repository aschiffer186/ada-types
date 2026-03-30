#ifndef BUILD_MODULE
#include "cina.hpp"
#else
import cina;
#endif

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

  using boolean_type2 = cina::boolean_type<struct BooleanType2, bool&>;
  EXPECT_TRUE(cina::strong_type_like<boolean_type2>);
  EXPECT_TRUE((std::same_as<cina::underlying_type<boolean_type2>, bool&>));
}

TEST(TestBoolean, TestConstructor) {
  using boolean_type = cina::boolean_type<struct BooleanType, bool>;

  const boolean_type b1{true};
  EXPECT_TRUE(b1.unwrap());

  using boolean_type2 = cina::boolean_type<struct BooleanType2, bool>;
  EXPECT_FALSE((std::constructible_from<boolean_type, boolean_type2>));

  EXPECT_FALSE((std::convertible_to<bool, boolean_type>));
  EXPECT_FALSE((std::convertible_to<boolean_type, bool>));

  using boolean_reference = cina::boolean_type<struct BooleanType3, bool&>;
  bool b{true};
  boolean_reference ref{b};
  EXPECT_TRUE(ref.unwrap());

  cina::boolean_type<struct BooleanType4, const bool&> ref2{b};
  EXPECT_TRUE(ref2.unwrap());

  EXPECT_FALSE((std::constructible_from<boolean_reference, const bool&>));

  using boolean_reference2 = cina::boolean_type<struct BooleanType5, bool&>;
  EXPECT_FALSE(
      (std::constructible_from<boolean_reference, boolean_reference2>));
}

TEST(TestBoolean, TestAssignment) {
  using boolean_type1 = cina::boolean_type<struct BooleanType, bool>;
  using boolean_type2 = cina::boolean_type<struct BooleanType2, bool>;

  EXPECT_FALSE((std::assignable_from<boolean_type1, boolean_type2>));

  using boolean_reference = cina::boolean_type<struct BooleanType3, bool&>;
  bool b{true};
  boolean_reference ref{b};
  ref.unwrap() = false;
  EXPECT_FALSE(b);
  EXPECT_FALSE(ref.unwrap());

  using boolean_reference2 = cina::boolean_type<struct BooleanType4, bool&>;
  EXPECT_FALSE((std::assignable_from<boolean_reference, boolean_reference2>));
}

TEST(TestBoolean, TestFormat) {
  using boolean_type = cina::boolean_type<struct BooleanType, bool>;

  const std::string str = std::format("{}", boolean_type{true});
  EXPECT_STREQ(str.c_str(), "true");

  using boolean_reference = cina::boolean_type<struct BooleanType2, bool&>;

  bool b{true};
  boolean_reference ref{b};
  const std::string str2 = std::format("{}", ref);
  EXPECT_STREQ(str2.c_str(), "true");

  using boolean_reference2 =
      cina::boolean_type<struct BooleanType3, const bool&>;
  boolean_reference2 ref2{b};
  const std::string str3 = std::format("{}", ref2);
  EXPECT_STREQ(str3.c_str(), "true");

  struct Type : cina::boolean_type<Type, bool> {
    using cina::boolean_type<Type, bool>::boolean_type;
  };
  const std::string str4 = std::format("{}", Type{false});
  EXPECT_STREQ(str4.c_str(), "false");
}

TEST(TestBoolean, TestHash) {
  using boolean_type = cina::boolean_type<struct BooleanType, bool>;

  const boolean_type b1{true};
  const boolean_type b2{false};
  const std::size_t hash1 = std::hash<boolean_type>{}(b1);
  const std::size_t hash2 = std::hash<boolean_type>{}(b2);

  EXPECT_EQ(hash1, std::hash<bool>{}(true));
  EXPECT_EQ(hash2, std::hash<bool>{}(false));

  struct Type : cina::boolean_type<Type, bool> {
    using cina::boolean_type<Type, bool>::boolean_type;
  };
  const std::size_t hash3 = std::hash<Type>{}(Type{true});
  EXPECT_EQ(hash3, std::hash<bool>{}(true));
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

  using boolean_reference = cina::boolean_type<struct BooleanType3, bool&>;
  bool b3{true};
  boolean_reference ref{b3};

  bool b4{false};
  boolean_reference ref2{b4};
  EXPECT_EQ(ref, ref);
  EXPECT_NE(ref, ref2);

  using const_boolean_reference =
      cina::boolean_type<struct BooleanType4, const bool&>;
  const_boolean_reference cref{b3};
  const_boolean_reference cref2{b4};
  EXPECT_EQ(cref, cref);
  EXPECT_NE(cref, cref2);

  struct Type : cina::boolean_type<Type, bool> {
    using cina::boolean_type<Type, bool>::boolean_type;
  };
  const Type t1{true};
  const Type t2{false};
  EXPECT_EQ(t1, t1);
  EXPECT_NE(t1, t2);
}

TEST(TestBoolean, TestLess) {
  using boolean_type = cina::boolean_type<struct BooleanType, bool>;

  const boolean_type b1{true};
  const boolean_type b2{false};

  EXPECT_EQ(b1 < b2, true < false);

  struct Type : cina::boolean_type<Type, bool> {
    using cina::boolean_type<Type, bool>::boolean_type;
  };
  const Type t1{true};
  const Type t2{false};
  EXPECT_EQ(t1 < t2, true < false);

  using boolean_reference = cina::boolean_type<struct BooleanType3, bool&>;
  bool b3{true};
  boolean_reference ref{b3};
  bool b4{false};
  boolean_reference ref2{b4};
  EXPECT_EQ(ref < ref2, true < false);

  using const_boolean_reference =
      cina::boolean_type<struct BooleanType4, const bool&>;
  const_boolean_reference cref{b3};
  const_boolean_reference cref2{b4};
  EXPECT_EQ(cref < cref2, true < false);
}

TEST(TestBoolean, TestOutputStream) {
  using boolean_type = cina::boolean_type<struct BooleanType, bool>;

  const boolean_type b1{true};
  const boolean_type b2{false};

  std::ostringstream oss;
  oss << std::boolalpha << b1 << " " << b2;
  EXPECT_STREQ(oss.str().c_str(), "true false");

  struct Type : cina::boolean_type<Type, bool> {
    using cina::boolean_type<Type, bool>::boolean_type;
  };
  const Type t1{true};
  oss.str("");
  oss << std::boolalpha << t1;
  EXPECT_STREQ(oss.str().c_str(), "true");

  using boolean_reference = cina::boolean_type<struct BooleanType3, bool&>;
  bool b3{true};
  boolean_reference ref{b3};
  oss.str("");
  oss << std::boolalpha << ref;
  EXPECT_STREQ(oss.str().c_str(), "true");

  using const_boolean_reference =
      cina::boolean_type<struct BooleanType4, const bool&>;
  const_boolean_reference cref{b3};
  oss.str("");
  oss << std::boolalpha << cref;
  EXPECT_STREQ(oss.str().c_str(), "true");
}

TEST(TestBoolean, TestContextualConversion) {
  using boolean_type = cina::boolean_type<struct BooleanType, bool>;

  enum DontCare { cDONT_CARE_FALSE, cDONT_CARE_TRUE };

  const boolean_type b1{true};
  const DontCare dc = b1 ? cDONT_CARE_TRUE : cDONT_CARE_FALSE;
  EXPECT_EQ(dc, cDONT_CARE_TRUE);

  struct Type : cina::boolean_type<Type, bool> {
    using cina::boolean_type<Type, bool>::boolean_type;
  };
  const Type t1{false};
  const DontCare dc2 = t1 ? cDONT_CARE_TRUE : cDONT_CARE_FALSE;
  EXPECT_EQ(dc2, cDONT_CARE_FALSE);

  using boolean_reference = cina::boolean_type<struct BooleanType3, bool&>;
  bool b3{true};
  boolean_reference ref{b3};
  const DontCare dc3 = ref ? cDONT_CARE_TRUE : cDONT_CARE_FALSE;
  EXPECT_EQ(dc3, cDONT_CARE_TRUE);

  using const_boolean_reference =
      cina::boolean_type<struct BooleanType4, const bool&>;
  const_boolean_reference cref{b3};
  const DontCare dc4 = cref ? cDONT_CARE_TRUE : cDONT_CARE_FALSE;
  EXPECT_EQ(dc4, cDONT_CARE_TRUE);
}

TEST(TestBoolean, TestLogicalAnd) {
  using boolean_type = cina::boolean_type<struct BooleanType, bool>;

  const boolean_type b1{true};
  const boolean_type b2{false};
  const boolean_type b3{true};

  EXPECT_EQ(b1 && b2, boolean_type{false});
  EXPECT_EQ(b1 && b3, boolean_type{true});
  EXPECT_EQ(b2 && b3, boolean_type{false});

  struct Type : cina::boolean_type<Type, bool> {
    using cina::boolean_type<Type, bool>::boolean_type;
  };
  const Type t1{true};
  const Type t2{false};
  const Type t3{true};
  EXPECT_EQ(t1 && t2, Type{false});
  EXPECT_EQ(t1 && t3, Type{true});
  EXPECT_EQ(t2 && t3, Type{false});
}

TEST(TestBoolean, TestLogicalOr) {
  using boolean_type = cina::boolean_type<struct BooleanType, bool>;

  const boolean_type b1{true};
  const boolean_type b2{false};
  const boolean_type b3{true};

  EXPECT_EQ(b1 || b2, boolean_type{true});
  EXPECT_EQ(b1 || b3, boolean_type{true});
  EXPECT_EQ(b2 || b3, boolean_type{true});

  struct Type : cina::boolean_type<Type, bool> {
    using cina::boolean_type<Type, bool>::boolean_type;
  };
  const Type t1{true};
  const Type t2{false};
  const Type t3{true};
  EXPECT_EQ(t1 || t2, Type{true});
  EXPECT_EQ(t1 || t3, Type{true});
  EXPECT_EQ(t2 || t3, Type{true});

  using boolean_reference = cina::boolean_type<struct BooleanType3, bool&>;
  bool b4{true};
  boolean_reference ref{b4};
  bool b5{false};
  boolean_reference ref2{b5};

  using result_type = cina::boolean_type<struct BooleanType3, bool>;
  EXPECT_EQ(ref || ref2, result_type{true});
  EXPECT_EQ(ref || ref, result_type{true});
  EXPECT_EQ(ref2 || ref2, result_type{false});

  using const_boolean_reference =
      cina::boolean_type<struct BooleanType4, const bool&>;
  const_boolean_reference cref{b4};
  const_boolean_reference cref2{b5};
  using result_type2 = cina::boolean_type<struct BooleanType4, const bool>;
  EXPECT_EQ(cref || cref2, result_type2{true});
  EXPECT_EQ(cref || cref, result_type2{true});
  EXPECT_EQ(cref2 || cref2, result_type2{false});
}

TEST(TestBoolean, TestLogicalNot) {
  using boolean_type = cina::boolean_type<struct BooleanType, bool>;

  const boolean_type b1{true};
  const boolean_type b2{false};

  EXPECT_EQ(!b1, boolean_type{false});
  EXPECT_EQ(!b2, boolean_type{true});

  struct Type : cina::boolean_type<Type, bool> {
    using cina::boolean_type<Type, bool>::boolean_type;
  };
  const Type t1{true};
  const Type t2{false};
  EXPECT_EQ(!t1, Type{false});
  EXPECT_EQ(!t2, Type{true});
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