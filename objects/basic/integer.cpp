#include "integer.hpp"

#include "callable.hpp"
#include "typed_func.hpp"
#include "internal_func.hpp"
#include "type.hpp"
#include <memory>
#include <sstream>

namespace integer {
using namespace object;
using namespace internal_func;
using namespace callable;
using namespace typed_func;
const std::string Integer_source = __FILE__;

arg_list on_add(arg_list args) {
  if (args.size() == 1) return {
    std::make_shared<Integer>(static_cast<Integer*>(args[0].get())->value,args[0]->loc)};
  return {
    std::make_shared<Integer>(
        static_cast<Integer*>(args[0].get())->value + 
        static_cast<Integer*>(args[1].get())->value,
      args[0]->loc)};
}
arg_list on_sub(arg_list args) {
  if (args.size() == 1) return {
    std::make_shared<Integer>(-static_cast<Integer*>(args[0].get())->value,args[0]->loc)};
  return {
    std::make_shared<Integer>(
        static_cast<Integer*>(args[0].get())->value - 
        static_cast<Integer*>(args[1].get())->value,
      args[0]->loc)};
}
arg_list on_mul(arg_list args) {
  return {
    std::make_shared<Integer>(
        static_cast<Integer*>(args[0].get())->value * 
        static_cast<Integer*>(args[1].get())->value,
      args[0]->loc)};
}
arg_list on_int_div(arg_list args) {
  return {
    std::make_shared<Integer>(
        static_cast<Integer*>(args[0].get())->value / 
        static_cast<Integer*>(args[1].get())->value,
      args[0]->loc)};
}
arg_list on_mod(arg_list args) {
  return {
    std::make_shared<Integer>(
        static_cast<Integer*>(args[0].get())->value % 
        static_cast<Integer*>(args[1].get())->value,
      args[0]->loc)};
}

std::shared_ptr<type::Type> Get_Integer_type() {
  static std::shared_ptr<type::Type> type = 
    std::make_shared<type::Type>(parser::location(&Integer_source,__LINE__),"Integer");
  static bool init = false;
  if (init) return type;
  init = true;
  {
    auto func_obj = std::make_shared<TypedFunction>(Get_Integer_type(),parser::location(&Integer_source,__LINE__));
    func_obj->push_func(std::make_shared<InternalFunction>(on_add),{Get_Integer_type(),Get_Integer_type()});
    func_obj->push_func(std::make_shared<InternalFunction>(on_add),{Get_Integer_type()});
    type->members["+"] = func_obj;
  }
  {
    auto func_obj = std::make_shared<TypedFunction>(Get_Integer_type(),parser::location(&Integer_source,__LINE__));
    func_obj->push_func(std::make_shared<InternalFunction>(on_sub),{Get_Integer_type(),Get_Integer_type()});
    func_obj->push_func(std::make_shared<InternalFunction>(on_sub),{Get_Integer_type()});
    type->members["-"] = func_obj;
  }
  {
    auto func_obj = std::make_shared<TypedFunction>(Get_Integer_type(),parser::location(&Integer_source,__LINE__));
    func_obj->push_func(std::make_shared<InternalFunction>(on_mul),{Get_Integer_type(),Get_Integer_type()});
    type->members["*"] = func_obj;
  }
  {
    auto func_obj = std::make_shared<TypedFunction>(Get_Integer_type(),parser::location(&Integer_source,__LINE__));
    func_obj->push_func(std::make_shared<InternalFunction>(on_int_div),{Get_Integer_type(),Get_Integer_type()});
    type->members["/"] = func_obj;
  }
  {
    auto func_obj = std::make_shared<TypedFunction>(Get_Integer_type(),parser::location(&Integer_source,__LINE__));
    func_obj->push_func(std::make_shared<InternalFunction>(on_mod),{Get_Integer_type(),Get_Integer_type()});
    type->members["%"] = func_obj;
  }
  return type;
}

Integer::Integer(BigInt value,parser::location loc) : Object(loc) {
  this->value = value;
  this->type = Get_Integer_type();
}

std::string Integer::to_string() const {
  std::stringstream ss;
  ss << this->value;
  return ss.str();
}

}
