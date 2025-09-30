#pragma once

#include <vector>
#include <memory>

#include "object.hpp"
#include "location.hh"

namespace callable {
using arg_list = std::vector<std::shared_ptr<object::Object>>;
class Callable : public object::Object {
public:
  virtual std::shared_ptr<object::Object> on_call(arg_list args, parser::location where = parser::location()) = 0;
};
}
