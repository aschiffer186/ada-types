#include <memory>

#include "type_factory.hpp"

struct S {};

int main() {
  using type1 = ada_types::new_type<"type1", std::unique_ptr<int>>;
  using type2 = ada_types::new_type<"type2", int, ada_types::arithmetic_tag>;
  using type3 = ada_types::new_type<"type3", ada_types::range<0, 12>>;

  [[maybe_unused]] type1 t1;
  [[maybe_unused]] type2 t2;
  [[maybe_unused]] type3 t3;

  [[maybe_unused]] ada_types::natural n;
}