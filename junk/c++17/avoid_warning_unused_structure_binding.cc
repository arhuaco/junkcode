#include <iostream>
#include <map>

// Anothr way.
// template<class T> void _unused(T &v) {do { (void)(v); } while (0); }

int main(int argc, char *argv[]) {
  std::map<char, int> my_map = {{'b', 2}, {'a', 1}, {'c', 3}};
  // Avoid warning for unused 'v'. Not pretty.
  // Print keys in order.
  for ([[maybe_unused]] const auto &[key, v] : my_map) {
    std::cerr << "key: " << key << std::endl;
  }
  return 0;
}
