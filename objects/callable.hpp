#pragma once

#include <vector>

#include "object.hpp"

namespace callable {
using namespace object;
using namespace type;
class Callable : public Object {
public:
  Callable(parser::location loc, std::vector<Type*> arg_types);
  virtual std::vector<Object*> on_call(std::vector<Object*> args) = 0;
};
}
