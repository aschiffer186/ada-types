#ifndef BUILD_MODULE
#include "cina.hpp"
#else
import cina;
#endif

#include <concepts>
#include <gtest/gtest.h>
#include <type_traits>

TEST(TestPointerType, TestCXXConcept) {
  using ptr_type = cina::pointer_type<struct PtrTag, int*>;

  EXPECT_FALSE(std::is_default_constructible_v<ptr_type>);
  EXPECT_TRUE(std::is_nothrow_copy_constructible_v<ptr_type>);
  EXPECT_TRUE(std::is_nothrow_copy_assignable_v<ptr_type>);
  EXPECT_TRUE(std::is_nothrow_move_constructible_v<ptr_type>);
  EXPECT_TRUE(std::is_nothrow_move_assignable_v<ptr_type>);
  EXPECT_TRUE(std::is_nothrow_destructible_v<ptr_type>);
  EXPECT_TRUE(std::is_trivially_destructible_v<ptr_type>);

  EXPECT_EQ(sizeof(ptr_type), sizeof(int*));
  EXPECT_EQ(alignof(ptr_type), alignof(int*));

  EXPECT_TRUE(std::equality_comparable<ptr_type>);
  EXPECT_TRUE(std::swappable<ptr_type>);
}

TEST(TestPointerType, TestCinaConcepts) {
  using ptr_type = cina::pointer_type<struct PtrTag, int*>;

  EXPECT_TRUE(cina::strong_type_like<ptr_type>);

  EXPECT_TRUE((std::same_as<cina::underlying_type<ptr_type>, int*>));
}

TEST(TestPointerType, TestConstructor) {
  using ptr_type = cina::pointer_type<struct PtrTag, int*>;

  const ptr_type p1{nullptr};
  EXPECT_EQ(p1.unwrap(), nullptr);

  int i{42};
  const ptr_type p2{&i};
  EXPECT_EQ(p2.unwrap(), &i);

  EXPECT_FALSE((std::constructible_from<ptr_type, const int*>));

  struct Base {};
  struct Derived : Base {};

  using ptr_type2 = cina::pointer_type<struct PtrTag2, Base*>;

  Derived d;
  const ptr_type2 p3{&d};
  EXPECT_EQ(p3.unwrap(), &d);

  using ptr_type3 = cina::pointer_type<struct PtrTag3, int*>;
  EXPECT_FALSE((std::constructible_from<ptr_type, ptr_type3>));

  using ptr_type4 = cina::pointer_type<struct PtrTag4, const int*>;
  int i2{42};
  const ptr_type4 p4{&i2};
  EXPECT_EQ(p4.unwrap(), &i2);
}

TEST(TestPointerType, TestAssignment) {
  using ptr_type = cina::pointer_type<struct PtrTag, int*>;

  int i{42};
  ptr_type p{&i};
  p = nullptr;
  EXPECT_EQ(p.unwrap(), nullptr);
}

TEST(TestPointerType, TestComparison) {
  using ptr_type = cina::pointer_type<struct PtrTag, int*>;

  int i{42};
  const ptr_type p1{&i};
  const ptr_type p2{&i};
  const ptr_type p3{nullptr};

  EXPECT_EQ(p1, p2);
  EXPECT_NE(p1, p3);
  EXPECT_NE(p2, p3);
  EXPECT_EQ(p3, nullptr);
  EXPECT_EQ(nullptr, p3);

  EXPECT_TRUE((std::equality_comparable_with<ptr_type, std::nullptr_t>));
}

TEST(TestPointerType, TestTextualConversion) {
  enum dont_care { cFALSE, cTRUE };

  using ptr_type = cina::pointer_type<struct PtrTag, int*>;

  int i{42};
  const ptr_type p1{&i};

  const dont_care result = p1 ? cTRUE : cFALSE;
  EXPECT_EQ(result, cTRUE);

  const ptr_type p2{nullptr};
  const dont_care result2 = p2 ? cTRUE : cFALSE;
  EXPECT_EQ(result2, cFALSE);
}

TEST(TestPointerType, TestDereference) {
  using ptr_type = cina::pointer_type<struct PtrTag, int*>;

  int i{42};
  ptr_type p{&i};

  EXPECT_EQ(*p, 42);

  EXPECT_TRUE((std::same_as<decltype(*p), int&>));

  using ptr_type2 = cina::pointer_type<struct PtrTag2, const int*>;

  EXPECT_TRUE((std::same_as<decltype(*std::declval<ptr_type2>()), const int&>));
}

TEST(TestPointerType, TestTypeFactory) {
  using ptr_type = cina::new_type<struct PtrTag, int*>;

  EXPECT_TRUE((std::same_as<ptr_type,
                            cina::pointer_type<typename ptr_type::tag, int*>>));

  using ptr_type2 = cina::new_type<struct PtrTag2, int*>;
  EXPECT_FALSE((std::same_as<ptr_type, ptr_type2>));

  using ptr_type3 = cina::subtype<ptr_type>;
  EXPECT_TRUE((std::same_as<ptr_type3, ptr_type>));

  using ptr_type4 = cina::new_type<struct PtrTag4, ptr_type>;
  EXPECT_TRUE(
      (std::same_as<ptr_type4, cina::pointer_type<struct PtrTag4, int*>>));
}
