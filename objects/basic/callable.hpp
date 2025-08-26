#pragma once

#include <vector>
#include <memory>

#include "object.hpp"

namespace callable {
using arg_list = std::vector<std::shared_ptr<object::Object>>;
class Callable : public object::Object {
public:
  explicit Callable(parser::location loc);
  virtual arg_list on_call(arg_list args) = 0;
};
}
