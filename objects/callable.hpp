#pragma once

#include <vector>
#include <memory>

#include "object.hpp"

namespace callable {
using namespace object;
using namespace type;
typedef std::vector<std::shared_ptr<Object>> arg_list;
class Callable : public Object {
public:
  Callable(parser::location loc);
  void push_arg_types(std::vector<std::shared_ptr<Type>> arg_types);
  virtual arg_list on_call(arg_list args) = 0;
};
}
