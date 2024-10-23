#include <iostream>

#include "vector.h"

auto main() -> int {
  easystl::vector vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  std::cout << "vector size: " << vec.size() << std::endl;
  std::cout << "vector elements: ";
  for (const auto &v: vec) {
    std::cout << v << ' ';
  }
  std::cout << std::endl;
  return 0;
}
