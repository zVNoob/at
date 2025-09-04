#include "typed_func.hpp"

#include <memory>
#include "type.hpp"
#include "error.hpp"

namespace typed_func {
using namespace callable;
TypedFunction::TypedFunction(std::shared_ptr<type::Type> return_type) :
  return_type(return_type) {
}

std::shared_ptr<object::Object> TypedFunction::on_call(callable::arg_list args, parser::location where) {
  for (auto& [farg,func] : func_lookup_table) {
    if (farg.size() != args.size()) continue;
    bool match = true;
    for (size_t i = 0; i < farg.size(); i++) {
      if (farg[i] != args[i]->type) {
        match = false;
        break;
      }
    }
    if (match) return func->on_call(args,where);
  }
  throw error::eval_error("No matching function found",where);
}

void TypedFunction::push_func(std::shared_ptr<Callable> func,
                              const std::vector<std::shared_ptr<type::Type>>& arg_types) {
  func_lookup_table.emplace_back(arg_types,func); 
}

}
