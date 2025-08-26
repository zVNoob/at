#pragma once

#include "object.hpp"
#include <vector>

namespace array {

class Array : public object::Object {
public:
  std::vector<std::shared_ptr<object::Object>> elements;
  explicit Array(parser::location loc);
  std::string to_string() const override;
};
std::shared_ptr<type::Type> Get_Array_type();
}
