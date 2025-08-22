#pragma once

#include <vector>
#include <memory>

#include "object.hpp"

namespace callable {
typedef std::vector<std::shared_ptr<object::Object>> arg_list;
class Callable : public object::Object {
public:
  explicit Callable(parser::location loc);
  void push_arg_types(const std::vector<std::shared_ptr<type::Type>>& arg_types);
  virtual arg_list on_call(arg_list args) = 0;
};
}
