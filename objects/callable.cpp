#include "callable.hpp"

namespace callable {
using namespace object;
using namespace type;
  Callable::Callable(parser::location loc) : Object(loc) {}
    void Callable::push_arg_types(std::vector<std::shared_ptr<Type>> arg_types) {
  // TODO: Implement this after inplement array
  }
}
