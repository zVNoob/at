#include "integer.hpp"

#include "callable.hpp"
#include "string.hpp"
#include "fraction.hpp" 
#include "typed_func.hpp"
#include "internal_func.hpp"
#include "type.hpp"
#include <memory>

namespace integer {
using namespace object;
using namespace internal_func;
using namespace callable;
using namespace typed_func;

std::shared_ptr<Object> on_add(arg_list args) {
  if (args.size() == 1) return {
    std::make_shared<Integer>(static_cast<Integer*>(args[0].get())->value)};
  return std::make_shared<Integer>(
        static_cast<Integer*>(args[0].get())->value + 
        static_cast<Integer*>(args[1].get())->value);
}

std::shared_ptr<Object> on_sub(arg_list args) {
  if (args.size() == 1) return {
    std::make_shared<Integer>(static_cast<Integer*>(args[0].get())->value)};
  return std::make_shared<Integer>(
        static_cast<Integer*>(args[0].get())->value - 
        static_cast<Integer*>(args[1].get())->value);
}

std::shared_ptr<Object> on_mul(arg_list args) {
  return std::make_shared<Integer>(
        static_cast<Integer*>(args[0].get())->value * 
        static_cast<Integer*>(args[1].get())->value);
}

std::shared_ptr<Object> on_div(arg_list args) {
  if (static_cast<Integer*>(args[1].get())->value == BigInt(0)) throw error::internal_error("Division by zero");
  return std::make_shared<Integer>(
        static_cast<Integer*>(args[0].get())->value / 
        static_cast<Integer*>(args[1].get())->value);
}

std::shared_ptr<Object> on_mod(arg_list args) {
  if (static_cast<Integer*>(args[1].get())->value == BigInt(0)) throw error::internal_error("Division by zero");
  return std::make_shared<Integer>(
        static_cast<Integer*>(args[0].get())->value % 
        static_cast<Integer*>(args[1].get())->value);
}

std::shared_ptr<Object> on_assign(arg_list args) {
  // 1st arg is always variable
  static_cast<variable::Variable*>(args[0].get())->set_value(args[1]);
  return args[1];
}

std::shared_ptr<Object> on_construct(arg_list args) {
  if (args.size() == 1) return std::make_shared<Integer>(BigInt(0));
  if (dynamic_cast<Integer*>(args[1].get())) return args[1];
  if (dynamic_cast<string::String*>(args[1].get())) return std::make_shared<Integer>(BigInt(static_cast<string::String*>(args[1].get())->value));
  if (dynamic_cast<fraction::Fraction*>(args[1].get())) {
    BigInt num = static_cast<fraction::Fraction*>(args[1].get())->value();
    return std::make_shared<Integer>(num);
  }
  throw error::internal_error("Invalid argument for Integer constructor");
}

std::shared_ptr<type::Type> Get_Integer_type() {
  static std::shared_ptr<type::Type> type = 
    std::make_shared<type::Type>();
  static bool init = false;
  if (init) return type;
  init = true;
  {
    auto func_obj = std::make_shared<TypedFunction>(Get_Integer_type());
    func_obj->push_func(std::make_shared<InternalFunction>(on_add),{Get_Integer_type(),Get_Integer_type()});
    func_obj->push_func(std::make_shared<InternalFunction>(on_add),{Get_Integer_type()});
    type->members["+"] = func_obj;
  }
  {
    auto func_obj = std::make_shared<TypedFunction>(Get_Integer_type());
    func_obj->push_func(std::make_shared<InternalFunction>(on_sub),{Get_Integer_type(),Get_Integer_type()});
    func_obj->push_func(std::make_shared<InternalFunction>(on_sub),{Get_Integer_type()});
    type->members["-"] = func_obj;
  }
  {
    auto func_obj = std::make_shared<TypedFunction>(Get_Integer_type());
    func_obj->push_func(std::make_shared<InternalFunction>(on_mul),{Get_Integer_type(),Get_Integer_type()});
    type->members["*"] = func_obj;
  }
  {
    auto func_obj = std::make_shared<TypedFunction>(Get_Integer_type());
    func_obj->push_func(std::make_shared<InternalFunction>(on_div),{Get_Integer_type(),Get_Integer_type()});
    type->members["/"] = func_obj;
  }
  {
    auto func_obj = std::make_shared<TypedFunction>(Get_Integer_type());
    func_obj->push_func(std::make_shared<InternalFunction>(on_mod),{Get_Integer_type(),Get_Integer_type()});
    type->members["%"] = func_obj;
  }
  {
    auto func_obj = std::make_shared<TypedFunction>(Get_Integer_type());
    func_obj->push_func(std::make_shared<InternalFunction>(on_assign),{Get_Integer_type(),Get_Integer_type()});
    type->members["="] = func_obj;
  }
  {
    auto func_obj = std::make_shared<TypedFunction>(Get_Integer_type());
    func_obj->push_func(std::make_shared<InternalFunction>(on_construct),{nullptr,Get_Integer_type()});
    func_obj->push_func(std::make_shared<InternalFunction>(on_construct),{nullptr,string::Get_String_type()});
    func_obj->push_func(std::make_shared<InternalFunction>(on_construct),{nullptr,fraction::Get_Fraction_type()});
    type->members[""] = func_obj;
  }
  return type;
}

Integer::Integer(BigInt value) {
  this->value = value;
  this->type = Get_Integer_type();
}

}
