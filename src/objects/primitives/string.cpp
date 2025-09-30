#include "string.hpp"
#include "callable.hpp"
#include "object.hpp"
#include "type.hpp"
#include "internal_func.hpp"
#include "typed_func.hpp"
#include "integer.hpp"
#include "error.hpp"
#include <memory>

namespace string {
using namespace internal_func;
using namespace callable;
using namespace object;
using namespace typed_func;

std::shared_ptr<Object> on_add(arg_list args) {
  auto& value = static_cast<String*>(args[0].get())->value;
  auto& addend = static_cast<String*>(args[1].get())->value;
  return std::make_shared<String>(value + addend);
}


std::shared_ptr<Object> on_length(arg_list args) {
  return std::make_shared<integer::Integer>(
        BigInt(
          static_cast<String*>(args[0].get())->value.size()
        ));
}

std::shared_ptr<Object> on_find(arg_list args) {
  auto& value = static_cast<String*>(args[0].get())->value;
  auto& substr = static_cast<String*>(args[1].get())->value;
  return std::make_shared<integer::Integer>(
        BigInt(
          value.find(substr)
        ));
}

std::shared_ptr<Object> on_equal(arg_list args) {
  auto arg0 = static_cast<String*>(args[0].get())->value;
  auto arg1 = static_cast<String*>(args[1].get())->value;
  return std::make_shared<integer::Integer>(
        BigInt(arg0 == arg1));
}

class StringContainerVariable : public variable::Variable {
public:
  std::shared_ptr<String> container;
  int index;
  explicit StringContainerVariable(std::shared_ptr<String> container, int index) :
    Variable("", 0), container(container), index(index) {
    type = integer::Get_Integer_type();
  };
  std::shared_ptr<object::Object> get_value() {
    if (index < 0) index = container->value.size() + index;
    if (index < 0 || index >= container->value.size())
      throw error::internal_error("Index out of range");
    return std::make_shared<integer::Integer>(
      BigInt(static_cast<unsigned char>(container->value[index]))
    );
  }
  void set_value(std::shared_ptr<Object> value) {
    if (index < 0) index = container->value.size() + index;
    if (index < 0 || index >= container->value.size())
      throw error::internal_error("Index out of range");
    container->value[index] = static_cast<integer::Integer*>(value.get())->value.to_long();
  }
};

std::shared_ptr<Object> on_get(arg_list args) {
  int index = static_cast<integer::Integer*>(args[1].get())->value.to_long();
  auto obj = std::static_pointer_cast<String>(args[0]);
  return std::make_shared<StringContainerVariable>(obj, index);
}

std::shared_ptr<Object> on_construct(arg_list args) {
  if (args.size() == 1) return std::make_shared<String>("");
  if (dynamic_cast<String*>(args[1].get())) return args[1];
  if (dynamic_cast<integer::Integer*>(args[1].get())) 
    return std::make_shared<String>(
      std::string(1,static_cast<integer::Integer*>(args[1].get())->value.to_long()));
  throw error::internal_error("Invalid argument for String constructor");
  
}

std::shared_ptr<Object> on_assign(arg_list args) {
  // 1st arg is always variable
  static_cast<variable::Variable*>(args[0].get())->set_value(args[1]);
  return args[1];
}

std::shared_ptr<type::Type> Get_String_type() {
  static std::shared_ptr<type::Type> type = 
    std::make_shared<type::Type>();
  static bool init = false;
  if (init) return type;
  init = true;
  {
    auto func_obj = std::make_shared<TypedFunction>(Get_String_type());
    func_obj->push_func(std::make_shared<InternalFunction>(on_add),{Get_String_type(),Get_String_type()});
    type->members["+"] = func_obj;
  } 
  {
    auto func_obj = std::make_shared<TypedFunction>(integer::Get_Integer_type());
    func_obj->push_func(std::make_shared<InternalFunction>(on_length),{Get_String_type()});
    type->members["~"] = func_obj;
  }
  {
    auto func_obj = std::make_shared<TypedFunction>(integer::Get_Integer_type());
    func_obj->push_func(std::make_shared<InternalFunction>(on_find),{Get_String_type(),Get_String_type()});
    type->members["%"] = func_obj;
  }
  {
    auto func_obj = std::make_shared<TypedFunction>(integer::Get_Integer_type());
    func_obj->push_func(std::make_shared<InternalFunction>(on_equal),{Get_String_type(),Get_String_type()});
    type->members["=="] = func_obj;
  }
  {
    auto func_obj = std::make_shared<TypedFunction>(Get_String_type());
    func_obj->push_func(std::make_shared<InternalFunction>(on_get),{Get_String_type(),integer::Get_Integer_type()});
    type->members["[]"] = func_obj;
  }
  {
    auto func_obj = std::make_shared<TypedFunction>(Get_String_type());
    func_obj->push_func(std::make_shared<InternalFunction>(on_assign),{Get_String_type(),Get_String_type()});
    type->members["="] = func_obj;
  }
  {
    auto func_obj = std::make_shared<TypedFunction>(Get_String_type());
    func_obj->push_func(std::make_shared<InternalFunction>(on_construct),{nullptr});
    func_obj->push_func(std::make_shared<InternalFunction>(on_construct),{nullptr,Get_String_type()});
    func_obj->push_func(std::make_shared<InternalFunction>(on_construct),{nullptr,integer::Get_Integer_type()});
    type->members[""] = func_obj;
  }
  return type;
}

String::String(std::string value) : value(value) {
  type = Get_String_type();
}

}
