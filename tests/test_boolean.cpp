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
  EXPECT_TRUE(std::swappable<type>);

  using reference = cina::new_type<struct Tag2, bool&>;
  EXPECT_FALSE(std::is_default_constructible_v<reference>);
  EXPECT_TRUE(std::is_copy_constructible_v<reference>);
  EXPECT_TRUE(std::is_move_constructible_v<reference>);
  EXPECT_TRUE(std::is_copy_assignable_v<reference>);
  EXPECT_TRUE(std::is_move_assignable_v<reference>);
  EXPECT_TRUE(std::equality_comparable<reference>);
  EXPECT_TRUE(std::swappable<reference>);
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

  struct my_boolean : public cina::new_type<my_boolean, bool> {
    using cina::new_type<my_boolean, bool>::new_type;
  };

  const my_boolean mb{true};
  EXPECT_TRUE(mb.unwrap());
}

using nttp_type = cina::new_type<struct NTTP, bool>;
template <nttp_type B> struct test {
  static constexpr bool value = B.unwrap();
};

struct my_boolean : public cina::new_type<my_boolean, bool> {
  using cina::new_type<my_boolean, bool>::new_type;
};
template <my_boolean B> struct test2 {
  static constexpr bool value = B.unwrap();
};

TEST(TestBooleanType, TestNTTP) {
  constexpr nttp_type a{true};
  EXPECT_TRUE(test<a>::value);

  constexpr my_boolean b{false};
  EXPECT_FALSE(test2<b>::value);
}

TEST(TestBooleanType, TestComparison) {
  using type = cina::new_type<struct Tag, bool>;
  const type a{true};
  const type b{false};
  EXPECT_EQ(a, a);
  EXPECT_NE(a, b);
  constexpr type ca{true};
  constexpr type cb{false};
  static_assert(ca == ca);
  static_assert(ca != cb);

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

  struct my_boolean : public cina::new_type<my_boolean, bool> {
    using cina::new_type<my_boolean, bool>::new_type;
  };

  const my_boolean mb1{true};
  const my_boolean mb2{false};
  EXPECT_EQ(mb1, mb1);
  EXPECT_NE(mb1, mb2);
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

  struct my_boolean : public cina::new_type<my_boolean, bool> {
    using cina::new_type<my_boolean, bool>::new_type;
  };
  const my_boolean mb{false};
  std::stringstream ss4;
  ss4 << std::boolalpha << mb;
  EXPECT_EQ(ss4.str(), "false");
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

  struct my_boolean : public cina::new_type<my_boolean, bool> {
    using cina::new_type<my_boolean, bool>::new_type;
  };
  my_boolean mb{cina::uninitialized};
  std::stringstream ss3("true");
  ss3 >> std::boolalpha >> mb;
  EXPECT_TRUE(mb.unwrap());
}

consteval auto test_constexpr_assignment() -> bool {
  using type = cina::new_type<struct Tag, bool>;
  type a{false};
  a = type{true};
  return a.unwrap();
}

TEST(TestBooleanType, TestAssignment) {
  using type = cina::new_type<struct Tag, bool>;
  using type2 = cina::new_type<struct Tag2, bool>;
  EXPECT_FALSE((std::is_assignable_v<type&, type2>));
  static_assert(test_constexpr_assignment());

  using reference = cina::new_type<struct Tag3, bool&>;
  using reference2 = cina::new_type<struct Tag4, bool&>;
  EXPECT_FALSE((std::is_assignable_v<reference&, reference2>));

  bool b{false};
  reference ref{b};
  ref = cina::remove_reference_t<reference>{true};
  EXPECT_TRUE(b);
  bool b2{false};
  reference ref2{b2};
  ref = ref2;
  EXPECT_FALSE(b);

  struct my_boolean : public cina::new_type<my_boolean, bool> {
    using cina::new_type<my_boolean, bool>::new_type;
  };
  my_boolean mb{false};
  mb = my_boolean{true};
  EXPECT_TRUE(mb.unwrap());
}

consteval auto test_constexpr_swap() -> bool {
  using type = cina::new_type<struct Tag, bool>;
  type b1{true};
  type b2{false};

  b2.swap(b1);
  type b3 = b2;

  swap(b1, b2);
  type b4 = b1;

  return b3.unwrap() && b4.unwrap();
}

TEST(TestBooleanType, TestSwap) {
  using type = cina::new_type<struct Tag, bool>;
  type a{true};
  type b{false};
  a.swap(b);
  EXPECT_FALSE(a.unwrap());
  EXPECT_TRUE(b.unwrap());
  EXPECT_TRUE(noexcept(a.swap(b)));
  swap(a, b);
  EXPECT_TRUE(a.unwrap());
  EXPECT_FALSE(b.unwrap());
  EXPECT_TRUE(noexcept(swap(a, b)));
  static_assert(test_constexpr_swap());

  using reference = cina::new_type<struct Tag2, bool&>;
  bool value{true};
  reference ref{value};
  bool value2{false};
  reference ref2{value2};
  ref.swap(ref2);
  EXPECT_FALSE(value);
  EXPECT_TRUE(value2);
  EXPECT_TRUE(noexcept(ref.swap(ref2)));
  swap(ref, ref2);
  EXPECT_TRUE(value);
  EXPECT_FALSE(value2);
  EXPECT_TRUE(noexcept(swap(ref, ref2)));

  struct my_boolean : public cina::new_type<my_boolean, bool> {
    using cina::new_type<my_boolean, bool>::new_type;
  };
  my_boolean mb1{true};
  my_boolean mb2{false};
  mb1.swap(mb2);
  EXPECT_FALSE(mb1.unwrap());
  EXPECT_TRUE(mb2.unwrap());
  EXPECT_TRUE(noexcept(mb1.swap(mb2)));
  swap(mb1, mb2);
  EXPECT_TRUE(mb1.unwrap());
  EXPECT_FALSE(mb2.unwrap());
  EXPECT_TRUE(noexcept(swap(mb1, mb2)));
}

TEST(TestBooleanType, TestConversion) {
  enum class DontCare { cFALSE, cTRUE };

  using type = cina::new_type<struct Tag, bool>;
  type a{true};
  const DontCare c = a ? DontCare::cTRUE : DontCare::cFALSE;
  EXPECT_EQ(c, DontCare::cTRUE);
  constexpr type ca{false};
  constexpr DontCare cc = ca ? DontCare::cTRUE : DontCare::cFALSE;
  static_assert(cc == DontCare::cFALSE);

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

  struct my_boolean : public cina::new_type<my_boolean, bool> {
    using cina::new_type<my_boolean, bool>::new_type;
  };
  const my_boolean mb{true};
  const DontCare c4 = mb ? DontCare::cTRUE : DontCare::cFALSE;
  EXPECT_EQ(c4, DontCare::cTRUE);
}

TEST(TestBooleanType, TestHash) {
  using type = cina::new_type<struct Tag, bool>;
  type a{true};
  std::hash<type> hasher;
  EXPECT_EQ(hasher(a), std::hash<bool>{}(true));

  struct my_boolean : public cina::new_type<my_boolean, bool> {
    using cina::new_type<my_boolean, bool>::new_type;
  };
  const my_boolean mb{false};
  std::hash<my_boolean> hasher2;
  EXPECT_EQ(hasher2(mb), std::hash<bool>{}(false));
}

TEST(TestBooleanType, TestFormatter) {
  using type = cina::new_type<struct Tag, bool>;
  type a{true};
  std::string formatted = std::format("{}", a);
  EXPECT_EQ(formatted, "true");

  using reference = cina::new_type<struct Tag2, bool&>;
  bool value{false};
  reference ref{value};
  std::string formatted2 = std::format("{}", ref);
  EXPECT_EQ(formatted2, "false");

  struct my_boolean : public cina::new_type<my_boolean, bool> {
    using cina::new_type<my_boolean, bool>::new_type;
  };
  const my_boolean mb{true};
  std::string formatted3 = std::format("{}", mb);
  EXPECT_EQ(formatted3, "true");
}