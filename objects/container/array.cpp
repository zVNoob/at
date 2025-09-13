#include "array.hpp"
#include <memory>
#include <map>
#include "callable.hpp"
#include "typed_func.hpp"
#include "internal_func.hpp"
#include "type.hpp" // IWYU pragma: keep
#include "integer.hpp"

namespace array {
using namespace object;
using namespace callable;
using namespace typed_func;
using namespace internal_func;

std::shared_ptr<Object> on_add(arg_list args) {
  auto elements = std::static_pointer_cast<Array>(args[0])->elements;
  auto& addend = std::static_pointer_cast<Array>(args[1])->elements;
  elements.insert(elements.end(), addend.begin(), addend.end());
  auto result = std::make_shared<Array>(args[0]->type);
  result->elements = elements;
  return result;
}

std::shared_ptr<Object> on_length(arg_list args) {
  return std::make_shared<integer::Integer>(
        BigInt(
          static_cast<Array*>(args[0].get())->elements.size()
        ));
}

class ArrayContainerVariable : public variable::Variable {
public:
  std::shared_ptr<Array> container;
  int index;
  explicit ArrayContainerVariable(std::shared_ptr<Array> container, int index) :
    Variable("", 0), container(container), index(index) {
    type = integer::Get_Integer_type();
  };
  std::shared_ptr<object::Object> get_value() {
    if (index < 0 || index >= container->elements.size())
      throw error::internal_error("Index out of range");
    return container->elements[index];
  }
  void set_value(std::shared_ptr<Object> value) {
    if (index < 0 || index >= container->elements.size())
      throw error::internal_error("Index out of range");
    container->elements[index] = value;
  }
};

std::shared_ptr<Object> on_get(arg_list args) {
  int index = static_cast<integer::Integer*>(args[1].get())->value.to_long();
  auto obj = std::static_pointer_cast<Array>(args[0]);
  return std::make_shared<ArrayContainerVariable>(obj, index);
}


std::shared_ptr<Object> on_assign(arg_list args) {
  // 1st arg is always variable
  static_cast<variable::Variable*>(args[0].get())->set_value(args[1]);
  return args[1];
}

void build_Array_type(std::shared_ptr<type::Type>& type,std::shared_ptr<type::Type> element_type) {
  {
    auto func_obj = std::make_shared<TypedFunction>(type);
    func_obj->push_func(std::make_shared<InternalFunction>(on_add),{type,type});
    type->members["+"] = func_obj;
  }
  {
    auto func_obj = std::make_shared<TypedFunction>(integer::Get_Integer_type());
    func_obj->push_func(std::make_shared<InternalFunction>(on_length),{type});
    type->members["~"] = func_obj;
  }
  {
    auto func_obj = std::make_shared<TypedFunction>(element_type);
    func_obj->push_func(std::make_shared<InternalFunction>(on_get),{type,integer::Get_Integer_type()});
    type->members["[]"] = func_obj;
  }
  {
    auto func_obj = std::make_shared<TypedFunction>(type);
    func_obj->push_func(std::make_shared<InternalFunction>(on_assign),{type,type});
    type->members["="] = func_obj;
  }
}

std::shared_ptr<type::Type> get_Array_type(std::shared_ptr<type::Type> element_type) {
  static std::map<std::shared_ptr<type::Type>,std::shared_ptr<type::Type>> table;
  if (table[element_type] == nullptr) {
    table[element_type] = std::make_shared<type::Type>();
    build_Array_type(table[element_type],element_type);
  }
  return table[element_type];
}

Array::Array(std::shared_ptr<type::Type> type) {
  this->type = get_Array_type(type);
}

}
