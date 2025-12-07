#include "strong_types.hpp"

int main() {
  using namespace ada_types;
#ifdef TEST_INCOMPATIBLE_STRONG_TYPES
  using test_type2 = strong_type<int, "tag2">;
  using test_type1 = strong_type<int, "tag1">;
  test_type1 t1{test_type2{}};
#endif

#ifdef TEST_CHECKED_INTEGER_DEFAULT_CONSTRUCTOR1
  using test_type1 = ada_types::signed_integer<int, "tag1", -10, -1>;
  test_type1 t1;
#endif

#ifdef TEST_CHECKED_INTEGER_DEFAULT_CONSTRUCTOR2
  using test_type1 = ada_types::signed_integer<int, "tag1", 10, 20>;
  test_type1 t1;
#endif
}