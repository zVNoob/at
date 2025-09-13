#pragma once

#include "object.hpp"

#include <vector>

namespace tuple {
class Tuple : public object::Object {
  public:
  bool has_identifier = false;
  bool has_const = false;
  bool assignable = true;
  bool re_declarable = true;
  std::vector<std::shared_ptr<object::Object>> values;
  explicit Tuple(const std::vector<std::shared_ptr<object::Object>>& values);
};
std::shared_ptr<type::Type> get_Tuple_type(std::vector<std::shared_ptr<type::Type>> types);
}
