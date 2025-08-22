#include "callable.hpp"

namespace callable {
  // TODO: Implement this after inplement array
  Callable::Callable(parser::location loc) : Object(loc) {}
  void Callable::push_arg_types(std::vector<std::shared_ptr<Type>> arg_types) {}
}
