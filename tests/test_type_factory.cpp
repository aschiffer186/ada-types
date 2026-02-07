#include "cina.hpp"

#include <gtest/gtest.h>

TEST(TestTypeFactory, TestNoSkills) {
  using strong_type =
      cina::new_type<struct NoSkillsTag1, bool, cina::no_skills>;
  EXPECT_TRUE((std::is_same_v<strong_type,
                              cina::strong_type<struct NoSkillsTag1, bool>>));

  using strong_type2 =
      cina::new_type<struct NoSkillsTag2, int, cina::no_skills>;
  EXPECT_TRUE((std::is_same_v<strong_type2,
                              cina::strong_type<struct NoSkillsTag2, int>>));

  using strong_type3 =
      cina::new_type<struct NoSkillsTag3, double, cina::no_skills>;
  EXPECT_TRUE((std::is_same_v<strong_type3,
                              cina::strong_type<struct NoSkillsTag3, double>>));

  using strong_type4 = cina::new_type<struct NoSkillsTag4, int, cina::no_skills,
                                      cina::enable_bitwise>;
  EXPECT_TRUE((std::is_same_v<strong_type4,
                              cina::strong_type<struct NoSkillsTag4, int>>));
}

TEST(TestTypeFactory, TestBooleanType) {
  using bool_type = cina::new_type<struct BoolTag, bool>;
  EXPECT_TRUE((std::is_same_v<bool_type, cina::boolean_type<struct BoolTag>>));
}

TEST(TestTypeFactory, TestIntegralType) {
  using int_type = cina::new_type<struct IntTag, int>;
  EXPECT_TRUE(
      (std::is_same_v<int_type, cina::integral_type<struct IntTag, int>>));

  using int_type2 = cina::new_type<struct IntTag2, int, cina::enable_bitwise>;
  EXPECT_TRUE(
      (std::is_same_v<int_type2,
                      cina::bitwise_integral_type<struct IntTag2, int>>));
}

TEST(TestTypeFactory, TestFloatingPointType) {
  using float_type = cina::new_type<struct FloatTag, double>;
  EXPECT_TRUE(
      (std::is_same_v<float_type,
                      cina::floating_point_type<struct FloatTag, double>>));
}

TEST(TestTypeFactory, TestBoundedIntegralType) {
  using bounded_int_type =
      cina::new_type<struct BoundedIntTag, int, cina::range<-10, 10>>;
  EXPECT_TRUE(
      (std::is_same_v<
          bounded_int_type,
          cina::bounded_integral_type<struct BoundedIntTag, int, -10, 10>>));
}

TEST(TestTypeFactory, TestPointerType) {
  using pointer_type = cina::new_type<struct PointerTag, int*>;
  EXPECT_TRUE((std::is_same_v<pointer_type,
                              cina::pointer_type<struct PointerTag, int*>>));

  using unique_owning_type =
      cina::new_type<struct UniqueOwningTag, int*, cina::owning_pointer>;
  EXPECT_TRUE(
      (std::is_same_v<unique_owning_type,
                      cina::unique_ptr_type<struct UniqueOwningTag, int>>));
}