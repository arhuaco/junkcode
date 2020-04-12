#include <iostream>
#include <map>
#include <string>

int main() {
  // Using a lambda for comparisons.
  auto comparator = [](const std::string& c1, const std::string& c2) -> bool {
    return c1 < c2;
  };
  std::map<std::string, int, decltype(comparator)> mymap(comparator);
  mymap = {{"1", 0}, {"9", 1}, {"4", 2}, {"3", 3}};
  for (const auto& [key, value] : mymap)
    std::cout << key << " " << value << std::endl;
  return 0;
}
