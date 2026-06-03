#include <cina.hpp>

#include <gtest/gtest.h>
#include <type_traits>

TEST(TestBooleanType, TestTypeFactory) {
  using type = cina::new_type<struct Tag, bool>;
  EXPECT_TRUE((std::same_as<type, cina::boolean_type<struct Tag, bool>>));

  using type2 = cina::new_type<struct Tag2, const bool>;
  EXPECT_TRUE(
      (std::same_as<type2, cina::boolean_type<struct Tag2, const bool>>));

  using type3 = cina::new_type<struct Tag3, volatile bool>;
  EXPECT_TRUE(
      (std::same_as<type3, cina::boolean_type<struct Tag3, volatile bool>>));

  using type4 = cina::new_type<struct Tag4, const volatile bool>;
  EXPECT_TRUE(
      (std::same_as<type4,
                    cina::boolean_type<struct Tag4, const volatile bool>>));

  using type5 = cina::new_type<struct Tag5, bool&>;
  EXPECT_TRUE((std::same_as<type5, cina::boolean_type<struct Tag5, bool&>>));

  using type6 = cina::new_type<struct Tag6, const bool&>;
  EXPECT_TRUE(
      (std::same_as<type6, cina::boolean_type<struct Tag6, const bool&>>));
}

TEST(TestBooleanType, TestCXXProperties) {
  using type = cina::new_type<struct Tag, bool>;
  EXPECT_FALSE(std::is_default_constructible_v<type>);
  EXPECT_TRUE(std::is_trivially_copyable_v<type>);
  EXPECT_TRUE(std::is_trivially_destructible_v<type>);
  EXPECT_TRUE(std::is_trivially_copy_constructible_v<type>);
  EXPECT_TRUE(std::is_trivially_move_constructible_v<type>);
  EXPECT_TRUE(std::is_trivially_copy_assignable_v<type>);
  EXPECT_TRUE(std::is_trivially_move_assignable_v<type>);
  EXPECT_TRUE(std::is_nothrow_move_constructible_v<type>);
  EXPECT_EQ(sizeof(type), sizeof(bool));
  EXPECT_EQ(alignof(type), alignof(bool));
  EXPECT_FALSE((std::convertible_to<type, int>));
  EXPECT_FALSE((std::convertible_to<type, bool>));

  using reference = cina::new_type<struct Tag2, bool&>;
  EXPECT_FALSE(std::is_default_constructible_v<reference>);
  EXPECT_TRUE(std::is_copy_constructible_v<reference>);
  EXPECT_TRUE(std::is_move_constructible_v<reference>);
  EXPECT_TRUE(std::is_copy_assignable_v<reference>);
  EXPECT_TRUE(std::is_move_assignable_v<reference>);
  EXPECT_TRUE(std::equality_comparable<reference>);
}

TEST(TestBooleanType, TestConstructor) {
  using type = cina::new_type<struct Tag, bool>;
  using type2 = cina::new_type<struct Tag2, bool>;
  EXPECT_FALSE((std::is_constructible_v<type, type2>));
  const type a{true};
  EXPECT_TRUE(a.unwrap());
  constexpr type b{true};
  static_assert(b.unwrap());

  using reference = cina::new_type<struct Tag3, bool&>;
  bool value{false};
  reference ref{value};
  EXPECT_FALSE(ref.unwrap());
  EXPECT_FALSE((std::is_constructible_v<reference, const bool&>));
  EXPECT_FALSE((std::is_constructible_v<reference, bool&&>));
  EXPECT_FALSE((std::is_constructible_v<reference, const bool&&>));

  using const_reference = cina::new_type<struct Tag4, const bool&>;
  bool value2{true};
  const_reference const_ref{value2};
  EXPECT_TRUE(const_ref.unwrap());
  EXPECT_TRUE((std::is_constructible_v<const_reference, bool&>));
  EXPECT_FALSE((std::is_constructible_v<const_reference, bool&&>));
  EXPECT_FALSE((std::is_constructible_v<const_reference, const bool&&>));
}

using nttp_type = cina::new_type<struct NTTP, bool>;
template <nttp_type B> struct test {
  static constexpr bool value = B.unwrap();
};

TEST(TestBooleanType, TestNTTP) {
  constexpr nttp_type a{true};
  EXPECT_TRUE(test<a>::value);
}

TEST(TestBooleanType, TestComparison) {
  using type = cina::new_type<struct Tag, bool>;
  const type a{true};
  const type b{false};
  EXPECT_EQ(a, a);
  EXPECT_NE(a, b);

  using reference = cina::new_type<struct Tag2, bool&>;
  bool value{true};
  reference ref{value};
  bool value2{false};
  reference ref2{value2};
  EXPECT_EQ(ref, ref);
  EXPECT_NE(ref, ref2);

  using const_reference = cina::new_type<struct Tag3, const bool&>;
  bool value3{true};
  const_reference const_ref{value3};
  bool value4{false};
  const_reference const_ref2{value4};
  EXPECT_EQ(const_ref, const_ref);
  EXPECT_NE(const_ref, const_ref2);
}

TEST(TestBooleanType, TestOutputStream) {
  using type = cina::new_type<struct Tag, bool>;
  type a{true};
  std::stringstream ss;
  ss << std::boolalpha << a;
  EXPECT_EQ(ss.str(), "true");

  using reference = cina::new_type<struct Tag2, bool&>;
  bool value{false};
  reference ref{value};
  std::stringstream ss2;
  ss2 << std::boolalpha << ref;
  EXPECT_EQ(ss2.str(), "false");

  using const_reference = cina::new_type<struct Tag3, const bool&>;
  bool value2{true};
  const_reference const_ref{value2};
  std::stringstream ss3;
  ss3 << std::boolalpha << const_ref;
  EXPECT_EQ(ss3.str(), "true");
}

TEST(TestBooleanType, TestInputStream) {
  using type = cina::new_type<struct Tag, bool>;
  type a{false};
  std::stringstream ss("true");
  ss >> std::boolalpha >> a;
  EXPECT_TRUE(a.unwrap());

  using reference = cina::new_type<struct Tag2, bool&>;
  bool value{false};
  reference ref{value};
  std::stringstream ss2("true");
  ss2 >> std::boolalpha >> ref;
  EXPECT_TRUE(value);
}

TEST(TestBooleanType, TestAssignment) {
  using type = cina::new_type<struct Tag, bool>;
  type b{true};
  b = false;
  EXPECT_FALSE(b.unwrap());

  using type2 = cina::new_type<struct Tag2, bool>;
  EXPECT_FALSE((std::is_assignable_v<type&, type2>));

  using reference = cina::new_type<struct Tag3, bool&>;
  bool b2{false};
  reference ref{b2};
  bool b3{true};
  reference ref2{b3};
  ref = ref2;
  EXPECT_TRUE(b2);
  ref = false;
  EXPECT_FALSE(b2);
}

TEST(TestBooleanType, TestConversion) {
  enum class DontCare { cFALSE, cTRUE };

  using type = cina::new_type<struct Tag, bool>;
  type a{true};
  const DontCare c = a ? DontCare::cTRUE : DontCare::cFALSE;
  EXPECT_EQ(c, DontCare::cTRUE);

  using reference = cina::new_type<struct Tag2, bool&>;
  bool value{true};
  reference ref{value};
  const DontCare c2 = ref ? DontCare::cTRUE : DontCare::cFALSE;
  EXPECT_EQ(c2, DontCare::cTRUE);

  using const_reference = cina::new_type<struct Tag3, const bool&>;
  bool value2{false};
  const_reference const_ref{value2};
  const DontCare c3 = const_ref ? DontCare::cTRUE : DontCare::cFALSE;
  EXPECT_EQ(c3, DontCare::cFALSE);
}

TEST(TestBooleanType, TestHash) {
  using type = cina::new_type<struct Tag, bool>;
  type a{true};
  std::hash<type> hasher;
  EXPECT_EQ(hasher(a), std::hash<bool>{}(true));
}

TEST(TestBooleanType, TestFormatter) {
  using type = cina::new_type<struct Tag, bool>;
  type a{true};
  std::string formatted = std::format("{}", a);
  EXPECT_EQ(formatted, "true");
}