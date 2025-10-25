#include "ada_types.hpp"

using int1 = ada_types::new_type<"int1", ada_types::range<0, 32>>;
using int2 = ada_types::new_type<"int2", ada_types::integer>;
using int_ref = ada_types::new_type<"int1", int&, ada_types::range<0, 32>>;

constexpr int foo() {
  int1 i{10};
  return 0;
}

int main() {
  constexpr int i = foo();

  return 0;
}