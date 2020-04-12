#include <iostream>
#include <map>
#include <string>

int main() {
  // Using a lambda for comparisons.
  auto my_gt = [](const std::string& c1, const std::string& c2) -> bool {
    return std::stod(c1) > std::stod(c2);
  };
  // Note decltype and constructor call.
  std::map<std::string, int, decltype(my_gt)> mymap(my_gt);
  mymap = {{"2", 0}, {"9", 1}, {"00", 100}, {"0001", 1}, {"0001000", -1}};
  // Not the same if you sort in lexicographic order.
  for (const auto& [key, value] : mymap)
    std::cerr << key << " " << value << std::endl;
  return 0;
}
