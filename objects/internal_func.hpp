#pragma once

#include "callable.hpp"
#include <functional>

namespace internal_func {
  class InternalFunction : public callable::Callable {
    std::function<callable::arg_list(callable::arg_list)> func;
    public:
      InternalFunction(std::function<callable::arg_list(callable::arg_list)> func, std::string& file,int line);
      callable::arg_list on_call(callable::arg_list args) override;
      bool operator==(const Object* that) const override;
  };
}
