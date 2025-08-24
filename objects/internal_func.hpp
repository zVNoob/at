#pragma once

#include "callable.hpp"
#include <functional>
#include <source_location>
#include <string>

namespace internal_func {
  class InternalFunction : public callable::Callable {
    std::function<callable::arg_list(callable::arg_list)> func;
    std::string source_name;
    std::source_location sloc;
    public:
    explicit InternalFunction(std::function<callable::arg_list(callable::arg_list)> func, std::source_location loc = std::source_location::current());
    callable::arg_list on_call(callable::arg_list args) override;
    std::string to_string() const override;
  };
}
