#pragma once

#include "callable.hpp"
#include <utility>
#include <vector>

namespace typed_func {
  
class TypedFunction : public callable::Callable {
  std::vector<
    std::pair<
      std::vector<std::shared_ptr<type::Type>>, 
      std::shared_ptr<callable::Callable>>
    > func_lookup_table;
  std::shared_ptr<type::Type> return_type;
  public:
  TypedFunction(std::shared_ptr<type::Type> return_type,parser::location loc);
  callable::arg_list on_call(callable::arg_list args) override;
  void push_func(std::shared_ptr<Callable> func,const std::vector<std::shared_ptr<type::Type>>& arg_types);
  std::string to_string() const override;
};
}
