#pragma once

#include "callable.hpp"
#include <utility>
#include <vector>

namespace typed_func {
  
class TypedFunction : public callable::Callable {
public:
  std::vector<
    std::pair<
      std::vector<std::shared_ptr<type::Type>>, 
      std::shared_ptr<callable::Callable>>
    > func_lookup_table;
  std::shared_ptr<type::Type> return_type;
  TypedFunction(std::shared_ptr<type::Type> return_type);
  std::shared_ptr<object::Object> on_call(callable::arg_list args, parser::location where) override;
  bool found_func(callable::arg_list args);
  void push_func(std::shared_ptr<Callable> func,const std::vector<std::shared_ptr<type::Type>>& arg_types);
};
}
