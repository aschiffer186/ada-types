#include <cina.hpp>

#include <concepts>
#include <gtest/gtest.h>
#include <sstream>
#include <type_traits>

using namespace cina;

TEST(TestBoolean, TestCXXProperties) {
  using boolean = boolean_type<"Test", bool>;

  EXPECT_FALSE(std::default_initializable<boolean>);
  EXPECT_TRUE(std::move_constructible<boolean>);
  EXPECT_TRUE(std::copy_constructible<boolean>);
  EXPECT_TRUE(std::movable<boolean>);
  EXPECT_TRUE(std::copyable<boolean>);
  EXPECT_TRUE(std::equality_comparable<boolean>);

  EXPECT_TRUE(std::is_nothrow_copy_constructible_v<boolean>);
  EXPECT_TRUE(std::is_trivially_copy_constructible_v<boolean>);
  EXPECT_TRUE(std::is_trivially_copyable_v<boolean>);
  EXPECT_TRUE(std::is_nothrow_move_constructible_v<boolean>);
  EXPECT_TRUE(std::is_trivially_move_constructible_v<boolean>);
  EXPECT_TRUE(std::is_nothrow_copy_assignable_v<boolean>);
  EXPECT_TRUE(std::is_trivially_copy_assignable_v<boolean>);
  EXPECT_TRUE(std::is_nothrow_move_assignable_v<boolean>);
  EXPECT_TRUE(std::is_trivially_move_assignable_v<boolean>);
  EXPECT_TRUE(std::is_trivially_destructible_v<boolean>);

  EXPECT_EQ(sizeof(boolean), sizeof(bool));
  EXPECT_EQ(alignof(boolean), alignof(bool));
}

TEST(TestBoolean, TestCinaConcepts) {
  using boolean = boolean_type<"Test", bool>;

  EXPECT_TRUE(strong_type_like<boolean>);
  EXPECT_TRUE((std::same_as<underlying_type<boolean>, bool>));

  struct boolean2 : boolean_type<"Test2", bool> {
    using boolean_type<"Test2", bool>::boolean_type;
  };

  EXPECT_TRUE(strong_type_like<boolean2>);
  EXPECT_TRUE((std::same_as<underlying_type<boolean2>, bool>));
}

TEST(TestBoolean, TestTypeFactory) {
  using boolean1 = new_type<"Boolean1", bool>;
  using boolean2 = new_type<"Boolean2", const bool>;
  using boolean3 = new_type<"Boolean3", bool&>;
  using boolean4 = new_type<"Boolean4", const bool&>;

  EXPECT_TRUE((std::same_as<boolean1, boolean_type<"Boolean1", bool>>));
  EXPECT_TRUE((std::same_as<boolean2, boolean_type<"Boolean2", const bool>>));
  EXPECT_TRUE((std::same_as<boolean3, boolean_type<"Boolean3", bool&>>));
  EXPECT_TRUE((std::same_as<boolean4, boolean_type<"Boolean4", const bool&>>));
}

TEST(TestBoolean, TestConstructor) {
  using boolean1 = new_type<"Boolean1", bool>;
  boolean1 b1{true};
  EXPECT_TRUE(b1.unwrap());
  using boolean2 = new_type<"Boolean2", bool>;
  EXPECT_FALSE((std::constructible_from<boolean1, boolean2>));
  EXPECT_FALSE((std::constructible_from<boolean2, boolean1>));

  using boolean3 = new_type<"Boolean3", const bool>;
  boolean3 b2{false};
  EXPECT_FALSE(b2.unwrap());

  struct boolean4 : new_type<"Boolean4", bool> {
    using new_type<"Boolean4", bool>::new_type;
  };
  boolean4 b3{true};
  EXPECT_TRUE(b3.unwrap());

  using boolean_ref = new_type<"BooleanRef", bool&>;
  bool bool1{true};
  boolean_ref b4{bool1};
  EXPECT_TRUE(b4.unwrap());
  EXPECT_FALSE((std::constructible_from<boolean_ref, bool&&>));
  EXPECT_FALSE((std::constructible_from<boolean_ref, const bool&>));
  EXPECT_FALSE((std::constructible_from<boolean_ref, const bool&&>));

  using boolean_cref = new_type<"BooleanCRef", const bool&>;
  bool bool2{false};
  boolean_cref b5{bool2};
  EXPECT_FALSE(b5.unwrap());

  const bool bool3{true};
  boolean_cref b6{bool3};
  EXPECT_TRUE(b6.unwrap());
}

TEST(TestBoolean, TestHash) {
  using boolean1 = new_type<"Boolean1", bool>;
  boolean1 b1{true};
  boolean1 b2{true};
  boolean1 b3{false};

  std::hash<boolean1> hasher;
  EXPECT_EQ(hasher(b1), hasher(b2));
  EXPECT_NE(hasher(b1), hasher(b3));
}

TEST(TestBoolean, TestFormatter) {
  using boolean1 = new_type<"Boolean1", bool>;
  boolean1 b1{true};
  boolean1 b2{false};

  EXPECT_EQ(std::format("{}", b1), "true");
  EXPECT_EQ(std::format("{}", b2), "false");
}

TEST(TestBoolean, TestEqualityComparison) {
  using boolean1 = new_type<"Boolean1", bool>;
  boolean1 b1{true};
  boolean1 b2{true};
  boolean1 b3{false};

  EXPECT_EQ(b1, b2);
  EXPECT_EQ(b2, b1);
  EXPECT_NE(b1, b3);
  EXPECT_NE(b3, b1);

  EXPECT_TRUE(std::equality_comparable<boolean1>);

  using boolean2 = new_type<"Boolean2", bool>;
  EXPECT_FALSE((std::equality_comparable_with<boolean1, boolean2>));

  struct boolean3 : new_type<"Boolean3", bool> {
    using new_type<"Boolean3", bool>::new_type;
  };
  boolean3 b4{true};
  boolean3 b5{false};
  EXPECT_EQ(b4, b4);
  EXPECT_NE(b4, b5);

  using boolean_ref = new_type<"BooleanRef", bool&>;
  bool bool1{true};
  bool bool2{false};
  boolean_ref b6{bool1};
  boolean_ref b7{bool2};

  EXPECT_EQ(b6, b6);
  EXPECT_NE(b6, b7);

  using boolean_cref = new_type<"BooleanCRef", const bool&>;
  const bool bool3{true};
  const bool bool4{false};
  boolean_cref b8{bool3};
  boolean_cref b9{bool4};
  EXPECT_EQ(b8, b8);
  EXPECT_NE(b8, b9);
}

TEST(TestBoolean, TestOutputOperator) {
  using boolean1 = new_type<"Boolean1", bool>;

  std::stringstream ss;
  ss << std::boolalpha << boolean1{true};

  EXPECT_STREQ(ss.str().c_str(), "true");
}

TEST(TestBoolean, TestInputOperator) {
  using boolean1 = new_type<"Boolean1", bool>;

  std::stringstream ss("false");
  boolean1 b{true};
  ss >> std::boolalpha >> b;

  EXPECT_FALSE(b.unwrap());
}

TEST(TestBoolean, TestAssignment) {
  using boolean1 = new_type<"Boolean1", bool>;
  using boolean2 = new_type<"Boolean2", bool>;

  EXPECT_FALSE((std::assignable_from<boolean1&, boolean2>));
  EXPECT_FALSE((std::assignable_from<boolean2&, boolean1>));

  using boolean_ref = new_type<"BooleanRef", bool&>;
  bool bool1{true};
  boolean_ref b1{bool1};

  bool bool2{false};
  boolean_ref b2{bool2};
  b1 = b2;
  EXPECT_FALSE(b1.unwrap());
}

TEST(TestBoolean, TestConversionToBool) {
  using boolean1 = new_type<"Boolean1", bool>;

  enum dont_care { cTRUE, cFALSE };

  boolean1 b1{true};

  const dont_care dc = b1 ? cTRUE : cFALSE;
  EXPECT_EQ(dc, cTRUE);

  using boolean_ref = new_type<"BooleanRef", bool&>;

  bool bool1{true};
  boolean_ref b2{bool1};
  const dont_care dc2 = b2 ? cTRUE : cFALSE;
  EXPECT_EQ(dc2, cTRUE);

  using boolean_cref = new_type<"BooleanCRef", const bool&>;
  const bool bool2{false};
  boolean_cref b3{bool2};
  const dont_care dc3 = b3 ? cTRUE : cFALSE;
  EXPECT_EQ(dc3, cFALSE);

  struct boolean4 : new_type<"Boolean4", bool> {
    using new_type<"Boolean4", bool>::new_type;
  };
  boolean4 b4{true};
  const dont_care dc4 = b4 ? cTRUE : cFALSE;
  EXPECT_EQ(dc4, cTRUE);
}

TEST(TestBoolean, TestLogicalNotOperator) {
  using boolean1 = new_type<"Boolean1", bool>;

  boolean1 b1{true};
  boolean1 b2{false};

  EXPECT_FALSE(!b1);
  EXPECT_TRUE(!b2);

  struct boolean2 : new_type<"Boolean2", bool> {
    using new_type<"Boolean2", bool>::new_type;
  };

  boolean2 b3{true};
  EXPECT_FALSE(!b3);
}