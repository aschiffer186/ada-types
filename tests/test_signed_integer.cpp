#include "exceptions.hpp"
#include "strong_types.hpp"

#include <gtest/gtest.h>
#include <type_traits>

using namespace ada_types;

TEST(TestSignedInteger, TestDefaultConstructor) {
  using test_type = ada_types::signed_integer<int, "tag", -10, 10>;

  test_type t1;
  EXPECT_EQ(t1.get(), 0);
  EXPECT_TRUE(std::is_nothrow_default_constructible_v<test_type>);

  constexpr test_type t2;
  static_assert(t2.get() == 0);
}

TEST(TestSigndInteger, TestValueConstructor) {
  using test_type = ada_types::signed_integer<int, "tag", 1, 100>;

  test_type t1{1};
  EXPECT_EQ(t1.get(), 1);

  test_type t2{100};
  EXPECT_EQ(t2.get(), 100);

  EXPECT_THROW(test_type{0}, input_out_of_range);
  EXPECT_THROW(test_type{101}, input_out_of_range);

  constexpr test_type t3{2};
  static_assert(t3.get() == 2);
}

TEST(SignedInteger, TestAdition) {
  using test_type = ada_types::signed_integer<int, "tag", 0, 100>;

  test_type t1;
  t1 += test_type{20};

  EXPECT_EQ(t1.get(), 20);

  test_type t2{20};
  EXPECT_THROW(t2 += test_type{81}, input_out_of_range);

  test_type t3{25};
  test_type t4{25};
  test_type t5{1};

  t1 = t3 + t4 + t5;
  EXPECT_EQ(t1.get(), 71);

  EXPECT_NO_THROW(t3 + t4 + t5 + test_type{100});
  EXPECT_THROW(t1 = t3 + t4 + t5 + test_type{100}, input_out_of_range);
}