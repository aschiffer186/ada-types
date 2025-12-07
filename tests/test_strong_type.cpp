#include "strong_types.hpp"

#include <concepts>
#include <type_traits>

#include <gtest/gtest.h>

#include "test_types.hpp"

using namespace ada_types;

TEST(TestStrongTypes, TestBasicProperties) {
  using test_type = strong_type<int, "tag">;

  EXPECT_EQ(sizeof(test_type), sizeof(int));
  EXPECT_EQ(alignof(test_type), alignof(int));
  EXPECT_TRUE(std::regular<test_type>);
}

TEST(TestStrongTypes, TestDefaultConstructor) {
  using test_type1 = strong_type<tattle<true>, "tag">;
  using test_type2 = strong_type<tattle<false>, "tag2">;

  test_type1 t1;
  EXPECT_TRUE(std::is_default_constructible_v<test_type1>);
  EXPECT_TRUE(std::is_nothrow_default_constructible_v<test_type1>);
  EXPECT_EQ(t1.get().value, double{});

  test_type2 t2;
  EXPECT_TRUE(std::is_default_constructible_v<test_type2>);
  EXPECT_FALSE(std::is_nothrow_default_constructible_v<test_type2>);
  EXPECT_EQ(t2.get().value, double{});
}

TEST(TestStrongTypes, TestValueConstructor) {
  using test_type1 = strong_type<tattle<true>, "tag">;
  using test_type2 = strong_type<tattle<false>, "tag2">;

  auto num_copy_calls = tattle<true>::copy_ctor_count;
  auto num_move_calls = tattle<true>::move_ctor_count;

  tattle<true> t;
  t.value = 2;
  test_type1 t1{t};
  EXPECT_TRUE((std::is_nothrow_constructible_v<test_type1, tattle<true>>));
  EXPECT_EQ(t1.get().value, 2);
  EXPECT_EQ(tattle<true>::copy_ctor_count, num_copy_calls + 1);
  EXPECT_EQ(tattle<true>::move_ctor_count, num_move_calls);

  test_type1 t2(std::move(t));
  EXPECT_EQ(t2.get().value, 2);
  EXPECT_EQ(tattle<true>::copy_ctor_count, num_copy_calls + 1);
  EXPECT_EQ(tattle<true>::move_ctor_count, num_move_calls + 1);

  num_copy_calls = tattle<false>::copy_ctor_count;
  num_move_calls = tattle<false>::move_ctor_count;

  tattle<false> tattle2;
  tattle2.value = 2;
  test_type2 t3{tattle2};
  EXPECT_FALSE((std::is_nothrow_constructible_v<test_type2, tattle<false>>));
  EXPECT_EQ(t3.get().value, 2);
  EXPECT_EQ(tattle<false>::copy_ctor_count, num_copy_calls + 1);
  EXPECT_EQ(tattle<false>::move_ctor_count, num_move_calls);

  test_type2 t4(std::move(tattle2));
  EXPECT_EQ(t4.get().value, 2);
  EXPECT_EQ(tattle<false>::copy_ctor_count, num_copy_calls + 1);
  EXPECT_EQ(tattle<false>::move_ctor_count, num_move_calls + 1);
}

TEST(TestStrongTypes, TestGet) {
  using test_type = strong_type<int, "tag">;

  test_type t1;
  const test_type t2{};

  EXPECT_TRUE(std::is_lvalue_reference_v<decltype(t1.get())>);
  EXPECT_FALSE(std::is_const_v<std::remove_reference_t<decltype(t1.get())>>);

  EXPECT_TRUE(std::is_lvalue_reference_v<decltype(t2.get())>);
  EXPECT_TRUE(std::is_const_v<std::remove_reference_t<decltype(t2.get())>>);

  EXPECT_TRUE(std::is_rvalue_reference_v<decltype(test_type{}.get())>);
  EXPECT_FALSE(
      std::is_const_v<std::remove_reference_t<decltype(test_type{}.get())>>);

  using const_test_type = std::add_const_t<test_type>;
  EXPECT_TRUE(std::is_rvalue_reference_v<decltype(const_test_type{}.get())>);
  EXPECT_TRUE(std::is_const_v<
              std::remove_reference_t<decltype(const_test_type{}.get())>>);
}