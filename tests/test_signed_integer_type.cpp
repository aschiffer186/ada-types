#include <cina.hpp>

#include <compare>
#include <gtest/gtest.h>

TEST(TestSignedIntegerType, TestTypeFactory) {
  using type = cina::new_type<struct Tag, int>;
  EXPECT_TRUE((std::same_as<type, cina::signed_integer_type<struct Tag, int>>));

  using type2 = cina::new_type<struct Tag2, const int>;
  EXPECT_TRUE(
      (std::same_as<type2, cina::signed_integer_type<struct Tag2, const int>>));

  using type3 = cina::new_type<struct Tag3, int&>;
  EXPECT_TRUE(
      (std::same_as<type3, cina::signed_integer_type<struct Tag3, int&>>));

  using type4 = cina::new_type<struct Tag4, const int&>;
  EXPECT_TRUE((
      std::same_as<type4, cina::signed_integer_type<struct Tag4, const int&>>));
}

TEST(TestSignedIntegerType, TestCXXProperties) {
  using type = cina::new_type<struct Tag, int>;
  EXPECT_FALSE(std::is_default_constructible_v<type>);
  EXPECT_TRUE(std::is_trivially_copyable_v<type>);
  EXPECT_TRUE(std::is_trivially_destructible_v<type>);
  EXPECT_TRUE(std::is_trivially_copy_constructible_v<type>);
  EXPECT_TRUE(std::is_trivially_move_constructible_v<type>);
  EXPECT_TRUE(std::is_trivially_copy_assignable_v<type>);
  EXPECT_TRUE(std::is_trivially_move_assignable_v<type>);
  EXPECT_TRUE(std::is_nothrow_move_constructible_v<type>);
  EXPECT_EQ(sizeof(type), sizeof(int));
  EXPECT_EQ(alignof(type), alignof(int));
  EXPECT_FALSE((std::convertible_to<type, int>));
  EXPECT_FALSE((std::convertible_to<type, bool>));
  EXPECT_TRUE(std::swappable<type>);
  EXPECT_TRUE((std::three_way_comparable<type, std::strong_ordering>));

  using reference = cina::new_type<struct Tag2, int&>;
  EXPECT_FALSE(std::is_default_constructible_v<reference>);
  EXPECT_TRUE(std::is_copy_constructible_v<reference>);
  EXPECT_TRUE(std::is_move_constructible_v<reference>);
  EXPECT_TRUE(std::is_copy_assignable_v<reference>);
  EXPECT_TRUE(std::is_move_assignable_v<reference>);
  EXPECT_TRUE(std::equality_comparable<reference>);
  EXPECT_TRUE(std::swappable<reference>);
  EXPECT_TRUE((std::three_way_comparable<reference, std::strong_ordering>));
}

TEST(TestSignedIntegerType, TestConstructor) {
  using type = cina::new_type<struct Tag, int>;
  EXPECT_FALSE((std::is_constructible_v<type, long long>));
  EXPECT_FALSE((std::is_constructible_v<type, unsigned int>));
  type a{42};
  EXPECT_EQ(a.unwrap(), 42);
  constexpr type ca{42};
  static_assert(ca.unwrap() == 42);

  using reference = cina::new_type<struct Tag, int&>;
  int value{42};
  reference ref{value};
  EXPECT_EQ(ref.unwrap(), 42);
  EXPECT_FALSE((std::is_constructible_v<reference, const int&>));
  EXPECT_FALSE((std::is_constructible_v<reference, int&&>));
  EXPECT_FALSE((std::is_constructible_v<reference, const int&&>));

  using const_reference = cina::new_type<struct Tag, const int&>;
  const int const_value{42};
  const_reference const_ref{const_value};
  EXPECT_TRUE((std::is_constructible_v<const_reference, int&>));
  EXPECT_FALSE((std::is_constructible_v<const_reference, int&&>));
  EXPECT_FALSE((std::is_constructible_v<const_reference, const int&&>));

  struct my_integer : cina::new_type<my_integer, int> {
    using cina::new_type<my_integer, int>::new_type;
  };
  const my_integer mi{42};
  EXPECT_EQ(mi.unwrap(), 42);
  constexpr my_integer cmi{42};
  static_assert(cmi.unwrap() == 42);
}

using nntp_type = cina::new_type<struct NTTPTag, int>;
template <nntp_type N> struct test {
  static constexpr int value = N.unwrap();
};

struct my_nttp_type : cina::new_type<my_nttp_type, int> {
  using cina::new_type<my_nttp_type, int>::new_type;
};
template <my_nttp_type N> struct test2 {
  static constexpr int value = N.unwrap();
};

TEST(TestSignedIntegerType, TestNTTP) {
  constexpr nntp_type a{42};
  EXPECT_EQ(test<a>::value, 42);

  constexpr my_nttp_type b{84};
  EXPECT_EQ(test2<b>::value, 84);
}

TEST(TestSignedIntegerType, TestComparison) {
  using type = cina::new_type<struct Tag, int>;
  const type a{42};
  const type b{43};
  EXPECT_EQ(a, a);
  EXPECT_NE(a, b);
  EXPECT_LT(a, b);
  EXPECT_LE(a, b);
  EXPECT_GT(b, a);
  EXPECT_GE(b, a);

  constexpr type ca{42};
  constexpr type cb{43};
  static_assert(ca == ca);
  static_assert(ca != cb);
  static_assert(ca < cb);
  static_assert(ca <= cb);
  static_assert(cb > ca);
  static_assert(cb >= ca);

  using reference = cina::new_type<struct Tag2, int&>;
  int value{42};
  reference ref{value};
  int value2{43};
  reference ref2{value2};
  EXPECT_EQ(ref, ref);
  EXPECT_NE(ref, ref2);
  EXPECT_LT(ref, ref2);
  EXPECT_LE(ref, ref2);
  EXPECT_GT(ref2, ref);
  EXPECT_GE(ref2, ref);

  using const_reference = cina::new_type<struct Tag3, const int&>;
  const int const_value{42};
  const_reference const_ref{const_value};
  const int const_value2{43};
  const_reference const_ref2{const_value2};
  EXPECT_EQ(const_ref, const_ref);
  EXPECT_NE(const_ref, const_ref2);
  EXPECT_LT(const_ref, const_ref2);
  EXPECT_LE(const_ref, const_ref2);
  EXPECT_GT(const_ref2, const_ref);
  EXPECT_GE(const_ref2, const_ref);

  struct my_integer : cina::new_type<my_integer, int> {
    using cina::new_type<my_integer, int>::new_type;
  };
  const my_integer mi1{42};
  const my_integer mi2{43};
  EXPECT_EQ(mi1, mi1);
  EXPECT_NE(mi1, mi2);
  EXPECT_LT(mi1, mi2);
  EXPECT_LE(mi1, mi2);
  EXPECT_GT(mi2, mi1);
  EXPECT_GE(mi2, mi1);
}

TEST(TestSignedIntegerType, TestOutputStream) {
  using type = cina::new_type<struct Tag, int>;
  type a{42};
  std::stringstream ss;
  ss << a;
  EXPECT_EQ(ss.str(), "42");

  using type2 = cina::new_type<struct Tag, std::int8_t>;
  type2 b{static_cast<std::int8_t>(42)};
  std::stringstream ss2;
  ss2 << b;
  EXPECT_EQ(ss2.str(), "42");

  using reference = cina::new_type<struct Tag2, int&>;
  int value{42};
  reference ref{value};
  std::stringstream ss3;
  ss3 << ref;
  EXPECT_EQ(ss3.str(), "42");

  using const_reference = cina::new_type<struct Tag3, const int&>;
  const int const_value{42};
  const_reference const_ref{const_value};
  std::stringstream ss4;
  ss4 << const_ref;
  EXPECT_EQ(ss4.str(), "42");

  struct my_integer : cina::new_type<my_integer, int> {
    using cina::new_type<my_integer, int>::new_type;
  };

  const my_integer mi{42};
  std::stringstream ss5;
  ss5 << mi;
  EXPECT_EQ(ss5.str(), "42");
}

TEST(TestSignedIntegerType, TestInputStream) {
  using type = cina::new_type<struct Tag, int>;
  type a{cina::uninitialized};
  std::stringstream ss("42");
  ss >> a;
  EXPECT_EQ(a.unwrap(), 42);

  using reference = cina::new_type<struct Tag2, int&>;
  int value{0};
  reference ref{value};
  std::stringstream ss2("42");
  ss2 >> ref;
  EXPECT_EQ(value, 42);

  struct my_integer : cina::new_type<my_integer, int> {
    using cina::new_type<my_integer, int>::new_type;
  };
  my_integer mi{cina::uninitialized};
  std::stringstream ss3("42");
  ss3 >> mi;
  EXPECT_EQ(mi.unwrap(), 42);
}

consteval auto test_constexpr_addition_assignment() -> bool {
  using type = cina::new_type<struct Tag, int>;
  type a{42};
  a += type{84};
  return a.unwrap() == 126;
}

TEST(TestSignedIntegerType, TestAddition) {
  using type = cina::new_type<struct Tag, int>;
  type a{42};
  type b{84};
  type c = a + b;
  EXPECT_TRUE((std::same_as<decltype(c), type>));
  EXPECT_EQ(c.unwrap(), 126);
  constexpr type ca{42};
  constexpr type cb{42};
  constexpr type cc = ca + cb;
  static_assert(cc.unwrap() == 84);
  a += b;
  EXPECT_EQ(a.unwrap(), 126);
  static_assert(test_constexpr_addition_assignment());

  using reference = cina::new_type<struct Tag2, int&>;
  int value{42};
  reference ref{value};
  int value2{84};
  reference ref2{value2};
  cina::remove_reference_t<reference> sum = ref + ref2;
  EXPECT_TRUE(!std::is_reference_v<cina::underlying_type_t<decltype(sum)>>);
  EXPECT_EQ(sum.unwrap(), 126);
  ref += ref2;
  EXPECT_EQ(value, 126);

  struct my_type : cina::new_type<my_type, int> {
    using cina::new_type<my_type, int>::new_type;
  };

  my_type d{42};
  auto e = d + d;
  EXPECT_EQ(e.unwrap(), 84);
  constexpr my_type cd{42};
  constexpr auto ce = cd + cd;
  static_assert(ce.unwrap() == 84);
  d += d;
  EXPECT_EQ(d.unwrap(), 84);
}

consteval auto test_constexpr_subtraction_assignment() -> bool {
  using type = cina::new_type<struct Tag, int>;
  type a{84};
  a -= type{42};
  return a.unwrap() == 42;
};

TEST(TestSignedIntegerType, TestSubtraction) {
  using type = cina::new_type<struct Tag, int>;
  type a{84};
  type b{42};
  type c = a - b;
  EXPECT_TRUE((std::same_as<decltype(c), type>));
  EXPECT_EQ(c.unwrap(), 42);
  constexpr type ca{84};
  constexpr type cb{42};
  constexpr type cc = ca - cb;
  static_assert(cc.unwrap() == 42);
  a -= b;
  EXPECT_EQ(a.unwrap(), 42);
  static_assert(test_constexpr_subtraction_assignment());

  using reference = cina::new_type<struct Tag2, int&>;
  int value{84};
  reference ref{value};
  int value2{42};
  reference ref2{value2};
  cina::remove_reference_t<reference> difference = ref - ref2;
  EXPECT_TRUE(
      !std::is_reference_v<cina::underlying_type_t<decltype(difference)>>);
  EXPECT_EQ(difference.unwrap(), 42);
  ref -= ref2;
  EXPECT_EQ(value, 42);

  struct my_type : cina::new_type<my_type, int> {
    using cina::new_type<my_type, int>::new_type;
  };

  my_type d{84};
  auto e = d - d;
  EXPECT_EQ(e.unwrap(), 0);
  constexpr my_type cd{84};
  constexpr auto ce = cd - cd;
  static_assert(ce.unwrap() == 0);
  d -= d;
  EXPECT_EQ(d.unwrap(), 0);
}

consteval auto test_constexpr_multiplication_assignment() -> bool {
  using type = cina::new_type<struct Tag, int>;
  type a{42};
  a *= type{2};
  return a.unwrap() == 84;
};

TEST(TestSignedInteger, TestMultiplication) {
  using type = cina::new_type<struct Tag, int>;
  type a{42};
  type b{2};
  type c = a * b;
  EXPECT_TRUE((std::same_as<decltype(c), type>));
  EXPECT_EQ(c.unwrap(), 84);
  constexpr type ca{42};
  constexpr type cb{2};
  constexpr type cc = ca * cb;
  static_assert(cc.unwrap() == 84);
  a *= b;
  EXPECT_EQ(a.unwrap(), 84);
  static_assert(test_constexpr_multiplication_assignment());

  using reference = cina::new_type<struct Tag2, int&>;
  int value{42};
  reference ref{value};
  int value2{2};
  reference ref2{value2};
  cina::remove_reference_t<reference> product = ref * ref2;
  EXPECT_TRUE(!std::is_reference_v<cina::underlying_type_t<decltype(product)>>);
  EXPECT_EQ(product.unwrap(), 84);

  struct my_type : cina::new_type<my_type, int> {
    using cina::new_type<my_type, int>::new_type;
  };

  my_type d{42};
  auto e = d * my_type{2};
  EXPECT_EQ(e.unwrap(), 84);
  constexpr my_type cd{42};
  constexpr auto ce = cd * my_type{2};
  static_assert(ce.unwrap() == 84);
  d *= my_type{2};
  EXPECT_EQ(d.unwrap(), 84);
}

consteval auto test_constexpr_division_assignment() -> bool {
  using type = cina::new_type<struct Tag, int>;
  type a{84};
  a /= type{2};
  return a.unwrap() == 42;
};

TEST(TestSignedInteger, TestDivision) {
  using type = cina::new_type<struct Tag, int>;
  type a{84};
  type b{2};
  type c = a / b;
  EXPECT_TRUE((std::same_as<decltype(c), type>));
  EXPECT_EQ(c.unwrap(), 42);
  constexpr type ca{84};
  constexpr type cb{2};
  constexpr type cc = ca / cb;
  static_assert(cc.unwrap() == 42);
  a /= b;
  EXPECT_EQ(a.unwrap(), 42);
  static_assert(test_constexpr_division_assignment());

  using reference = cina::new_type<struct Tag2, int&>;
  int value{84};
  reference ref{value};
  int value2{2};
  reference ref2{value2};
  cina::remove_reference_t<reference> quotient = ref / ref2;
  EXPECT_TRUE(
      !std::is_reference_v<cina::underlying_type_t<decltype(quotient)>>);
  EXPECT_EQ(quotient.unwrap(), 42);
  ref /= ref2;
  EXPECT_EQ(value, 42);

  struct my_type : cina::new_type<my_type, int> {
    using cina::new_type<my_type, int>::new_type;
  };

  my_type d{84};
  auto e = d / my_type{2};
  EXPECT_EQ(e.unwrap(), 42);
  constexpr my_type cd{84};
  constexpr auto ce = cd / my_type{2};
  static_assert(ce.unwrap() == 42);
}

consteval auto test_modulo_assignment() -> bool {
  using type = cina::new_type<struct Tag, int>;
  type a{85};
  a %= type{42};
  return a.unwrap() == 1;
};

TEST(TestSignedInteger, TestModulo) {
  using type = cina::new_type<struct Tag, int>;
  type a{85};
  type b{42};
  type c = a % b;
  EXPECT_TRUE((std::same_as<decltype(c), type>));
  EXPECT_EQ(c.unwrap(), 1);
  constexpr type ca{85};
  constexpr type cb{42};
  constexpr type cc = ca % cb;
  static_assert(cc.unwrap() == 1);
  a %= b;
  EXPECT_EQ(a.unwrap(), 1);
  static_assert(test_modulo_assignment());

  using reference = cina::new_type<struct Tag2, int&>;
  int value{85};
  reference ref{value};
  int value2{42};
  reference ref2{value2};
  cina::remove_reference_t<reference> remainder = ref % ref2;
  EXPECT_TRUE(
      !std::is_reference_v<cina::underlying_type_t<decltype(remainder)>>);
  EXPECT_EQ(remainder.unwrap(), 1);
  ref %= ref2;
  EXPECT_EQ(value, 1);

  struct my_type : cina::new_type<my_type, int> {
    using cina::new_type<my_type, int>::new_type;
  };

  my_type d{85};
  auto e = d % my_type{42};
  EXPECT_EQ(e.unwrap(), 1);
  constexpr my_type cd{85};
  constexpr auto ce = cd % my_type{42};
  static_assert(ce.unwrap() == 1);
  d %= my_type{42};
  EXPECT_EQ(d.unwrap(), 1);
}

TEST(TestSignedInteger, TestNegation) {
  using type = cina::new_type<struct Tag, int>;
  type a{42};
  type b = -a;
  EXPECT_TRUE((std::same_as<decltype(b), type>));
  EXPECT_EQ(b.unwrap(), -42);
  constexpr type ca{42};
  constexpr type cb = -ca;
  static_assert(cb.unwrap() == -42);

  using reference = cina::new_type<struct Tag2, int&>;
  int value{42};
  reference ref{value};
  cina::remove_reference_t<reference> negation = -ref;
  EXPECT_TRUE(
      !std::is_reference_v<cina::underlying_type_t<decltype(negation)>>);
  EXPECT_EQ(negation.unwrap(), -42);

  struct my_type : cina::new_type<my_type, int> {
    using cina::new_type<my_type, int>::new_type;
  };

  my_type d{42};
  auto e = -d;
  EXPECT_EQ(e.unwrap(), -42);
  constexpr my_type cd{42};
  constexpr auto ce = -cd;
  static_assert(ce.unwrap() == -42);
}

consteval auto test_constexpr_increment() -> bool {
  using type = cina::new_type<struct Tag, int>;
  type a{41};
  type& b = ++a;
  if (a.unwrap() != 42) {
    return false;
  }
  if (b.unwrap() != 42) {
    return false;
  }
  type c = a++;
  if (c.unwrap() != 42) {
    return false;
  }
  return a.unwrap() == 43;
};

TEST(TestSignedInteger, TestIncrement) {
  using type = cina::new_type<struct Tag, int>;
  type a{41};
  type& b = ++a;
  EXPECT_EQ(a.unwrap(), 42);
  EXPECT_EQ(b.unwrap(), 42);
  type c = a++;
  EXPECT_EQ(a.unwrap(), 43);
  EXPECT_EQ(c.unwrap(), 42);
  static_assert(test_constexpr_increment());

  using reference = cina::new_type<struct Tag2, int&>;
  int value{41};
  reference ref{value};
  ++ref;
  EXPECT_EQ(value, 42);
  ref++;
  EXPECT_EQ(value, 43);

  struct my_type : cina::new_type<my_type, int> {
    using cina::new_type<my_type, int>::new_type;
  };

  my_type d{41};
  ++d;
  EXPECT_EQ(d.unwrap(), 42);
  d++;
  EXPECT_EQ(d.unwrap(), 43);
}

consteval auto test_constexpr_decrement() -> bool {
  using type = cina::new_type<struct Tag, int>;
  type a{43};
  type& b = --a;
  if (a.unwrap() != 42) {
    return false;
  }
  if (b.unwrap() != 42) {
    return false;
  }
  type c = a--;
  if (c.unwrap() != 42) {
    return false;
  }
  return a.unwrap() == 41;
};

TEST(TestSignedInteger, TestDecrement) {
  using type = cina::new_type<struct Tag, int>;
  type a{43};
  type& b = --a;
  EXPECT_EQ(a.unwrap(), 42);
  EXPECT_EQ(b.unwrap(), 42);
  type c = a--;
  EXPECT_EQ(a.unwrap(), 41);
  EXPECT_EQ(c.unwrap(), 42);
  static_assert(test_constexpr_decrement());

  using reference = cina::new_type<struct Tag2, int&>;
  int value{43};
  reference ref{value};
  --ref;
  EXPECT_EQ(value, 42);
  ref--;
  EXPECT_EQ(value, 41);

  struct my_type : cina::new_type<my_type, int> {
    using cina::new_type<my_type, int>::new_type;
  };

  my_type d{43};
  --d;
  EXPECT_EQ(d.unwrap(), 42);
  d--;
  EXPECT_EQ(d.unwrap(), 41);
}

TEST(TestSignedInteger, TestAssignment) {
  using type = cina::new_type<struct Tag, int>;
  using type2 = cina::new_type<struct Tag2, int>;
  EXPECT_FALSE((std::is_assignable_v<type, type2>));

  using reference = cina::new_type<struct Tag3, int&>;
  int i{42};
  reference ref{i};
  ref = cina::remove_reference_t<reference>{84};
  EXPECT_EQ(i, 84);
  reference ref2{i};
  ref2 = ref;
  EXPECT_EQ(i, 84);

  struct my_integer : cina::new_type<my_integer, int> {
    using cina::new_type<my_integer, int>::new_type;
  };
  my_integer a{42};
  a = my_integer{84};
  EXPECT_EQ(a.unwrap(), 84);
}

TEST(TestSignedInteger, TestHash) {
  using type = cina::new_type<struct Tag, int>;
  type a{42};
  std::hash<type> hasher;
  EXPECT_EQ(hasher(a), std::hash<int>{}(42));

  struct my_integer : cina::new_type<my_integer, int> {
    using cina::new_type<my_integer, int>::new_type;
  };
  my_integer mi{42};
  std::hash<my_integer> mi_hasher;
  EXPECT_EQ(mi_hasher(mi), std::hash<int>{}(42));
}

TEST(TestSignedInteger, TestFormat) {
  using type = cina::new_type<struct Tag, int>;
  type a{42};
  EXPECT_EQ(std::format("{}", a), "42");

  using reference = cina::new_type<struct Tag2, int&>;
  int value{42};
  reference ref{value};
  EXPECT_EQ(std::format("{}", ref), "42");

  using const_reference = cina::new_type<struct Tag3, const int&>;
  const int const_value{42};
  const_reference const_ref{const_value};
  EXPECT_EQ(std::format("{}", const_ref), "42");

  struct my_integer : cina::new_type<my_integer, int> {
    using cina::new_type<my_integer, int>::new_type;
  };
  my_integer mi{42};
  EXPECT_EQ(std::format("{}", mi), "42");
}
