#include "tuple.hpp"

#include "object.hpp"
#include "callable.hpp"
#include <memory>
#include "parser.hpp"
#include "type.hpp" // IWYU pragma: keep
#include "typed_func.hpp"
#include "variable.hpp"
#include "internal_func.hpp"

namespace tuple {

using namespace object;
using namespace callable;
using namespace internal_func;

std::shared_ptr<Object> on_assign(arg_list args) {
  // 1st arg is always variable
  static_cast<variable::Variable*>(args[0].get())->set_value(args[1]);
  return args[1];
}

void build_Tuple_type(std::shared_ptr<type::Type>& type) {
  {
    auto func = std::make_shared<typed_func::TypedFunction>(type);
    func->push_func(std::make_shared<InternalFunction>(on_assign),{type,type});
    type->members["="] = func;
  }
}

struct tuple_type_table {
  std::shared_ptr<type::Type> type;
  std::map<std::shared_ptr<type::Type>,tuple_type_table> subtypes;
};

std::shared_ptr<type::Type> get_Tuple_type(std::vector<std::shared_ptr<type::Type>> types) {
  static tuple_type_table table;
  tuple_type_table& temp = table;
  for (auto& type : types) {
    temp = temp.subtypes[type];
  }
  if (temp.type == nullptr) {
    temp.type = std::make_shared<type::Type>();
    build_Tuple_type(temp.type);
  }
  return temp.type;
}

Tuple::Tuple(const std::vector<std::shared_ptr<Object>>& values) {
  this->values = values;
  std::vector<std::shared_ptr<type::Type>> types;
  for (auto& value : values) {
    types.push_back(value->type);
  }
  this->type = get_Tuple_type(types);
}

}
