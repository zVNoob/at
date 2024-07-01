#pragma once

#include <stdexcept>
#include <string>
#include <vector>

class StringList {
private:
  std::vector<std::string> list;

public:
  void add(const std::string &str) {
    size_t str_begin = 0;
    for (size_t str_end = 0; str_end < str.size(); ++str_end) {
      if (str[str_end] == '\n') {
        list.push_back(str.substr(str_begin, str_end - str_begin));
        str_begin = str_end + 1;
      }
    }
    list.push_back(str.substr(str_begin, str.size() - str_begin));
  }
  void add_line(const std::string &str) { list.push_back(str); }
  std::string &operator[](size_t index) {
    if (index >= list.size()) {
      throw std::out_of_range("Index out of range");
    }
    return list[index];
  }
  size_t size() { return list.size(); }
};
