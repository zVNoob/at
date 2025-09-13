#pragma once

#include "object.hpp"
#include <vector>

namespace array {
class Array : public object::Object {
public:
  std::vector<std::shared_ptr<object::Object>> elements;
  Array(std::shared_ptr<type::Type> type);
};
std::shared_ptr<type::Type> get_Array_type(std::shared_ptr<type::Type> element_type);
}
