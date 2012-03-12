#include <algorithm>
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
  std::string x = "Hola mundo Cruel";
  std::cout << count(x.begin(), x.end(), ' ') << std::endl;
  return 0;
}
