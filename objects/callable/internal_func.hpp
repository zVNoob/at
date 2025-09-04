#pragma once

#include "callable.hpp"
#include "error.hpp"
#include <functional>
#include <sstream>

namespace internal_func {
  class InternalFunction : public callable::Callable {
    std::function<std::shared_ptr<object::Object>(callable::arg_list)> func;
    public:
    explicit InternalFunction(std::function<std::shared_ptr<object::Object>(callable::arg_list)> func) :
      func(func) {};
    std::shared_ptr<object::Object> on_call(callable::arg_list args, parser::location where) override {
      try {
        return func(args);
      } catch (error::internal_error& e) {
        std::stringstream ss;
        ss << "Error from " << e.location << ": " << e.what();
        throw error::eval_error(ss.str(),where);
      }
    }
  };
}
