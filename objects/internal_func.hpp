#pragma once

#include "callable.hpp"
#include "type.hpp"
#include <functional>

namespace internal_func {
using namespace callable;
using namespace type;
  class InternalFunction : public Callable {
    std::function<arg_list(arg_list)> func;
    public:
      InternalFunction(std::function<arg_list(arg_list)> func, std::string& file,int line);
      arg_list on_call(arg_list args) override;
      bool operator==(const Object* that) const override;
  };
}
