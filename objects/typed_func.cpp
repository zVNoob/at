#include "typed_func.hpp"

#include <memory>
#include "type.hpp"
#include "../error.hpp"

namespace typed_func {
using namespace callable;
TypedFunction::TypedFunction(std::shared_ptr<type::Type> return_type,parser::location loc) :
  Callable(loc), return_type(return_type) {
}

callable::arg_list TypedFunction::on_call(callable::arg_list args) {
  for (auto& [farg,func] : func_lookup_table) {
    if (farg.size() != args.size()) continue;
    bool match = true;
    for (size_t i = 0; i < farg.size(); i++) {
      if (*(farg[i].get()) != *(args[i]->type)) {
        match = false;
        break;
      }
    }
    if (match) return func->on_call(args);
  }
  throw error::eval_error("No matching function found",loc);
}

void TypedFunction::push_func(std::shared_ptr<Callable> func,
                              const std::vector<std::shared_ptr<type::Type>>& arg_types) {
  func_lookup_table.emplace_back(arg_types,func); 
}

std::string TypedFunction::to_string() const {
  return "TypedFunction";
}
}
