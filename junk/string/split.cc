std::vector<std::string> StringSplit(const std::string_view str,
                                     const char delimiter) {
  std::vector<std::string> result;
  std::size_t start_pos = 0;
  std::size_t end_pos;

  while ((end_pos = str.find_first_of(delimiter, start_pos)) != std::string_view::npos) {
    result.emplace_back(str.substr(start_pos, end_pos - start_pos));
    start_pos = end_pos + 1;
  }
  if (start_pos < str.size()) {
    result.emplace_back(str.substr(start_pos));
  }
  return result;
}
