#include "cina.hpp"

int main() {
  const cina::boolean_type<struct Tag> b{true};
  [[maybe_unused]] auto hash = std::hash<cina::boolean_type<struct Tag>>{}(b);

  [[maybe_unused]] std::string f = std::format("Boolean value: {}", b);
}