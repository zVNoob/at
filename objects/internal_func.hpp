#pragma once

#include "callable.hpp"
#include "type.hpp"
#include <functional>

namespace internal_func {
using namespace callable;
using namespace type;
  class InternalFunction : public Callable {
    std::function<std::vector<Object*>(std::vector<Object*>)> func;
    public:
      InternalFunction(std::function<std::vector<Object*>(std::vector<Object*>)> func, std::vector<Type*> arg_types,std::string& file,int line);
      std::vector<Object*> on_call(std::vector<Object*> args) override;
      bool operator==(const Object* that) const override;
  };
}
