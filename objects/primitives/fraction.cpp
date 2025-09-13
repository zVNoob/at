#include "fraction.hpp"
#include "callable.hpp"
#include "integer.hpp"
#include "internal_func.hpp"
#include "typed_func.hpp"
#include "type.hpp"
#include "error.hpp"
#include "variable.hpp"
#include <memory>

namespace fraction {
using namespace callable;
using namespace internal_func;
using namespace typed_func;
using namespace object;

std::shared_ptr<Fraction> to_fraction(std::shared_ptr<object::Object> obj) {
  if (dynamic_cast<Fraction*>(obj.get())) return std::make_shared<Fraction>(static_cast<Fraction*>(obj.get())->value);
  return std::make_shared<Fraction>(BigFraction(static_cast<integer::Integer*>(obj.get())->value));
}

std::shared_ptr<Object> on_add(arg_list args) {
  if (args.size() == 1) return 
    std::make_shared<Fraction>(static_cast<Fraction*>(args[0].get())->value);
  auto arg0 = to_fraction(args[0]);
  auto arg1 = to_fraction(args[1]);
  return std::make_shared<Fraction>(
        arg0->value + 
        arg1->value);
}
std::shared_ptr<Object> on_sub(arg_list args) {
  if (args.size() == 1) return 
    std::make_shared<Fraction>(-static_cast<Fraction*>(args[0].get())->value);
  auto arg0 = to_fraction(args[0]);
  auto arg1 = to_fraction(args[1]);
  return std::make_shared<Fraction>(
        arg0->value - 
        arg1->value);
}
std::shared_ptr<Object> on_mul(arg_list args) {
  auto arg0 = to_fraction(args[0]);
  auto arg1 = to_fraction(args[1]);
  return std::make_shared<Fraction>(
        arg0->value * 
        arg1->value);
}

std::shared_ptr<Object> on_div(arg_list args) {
  auto arg0 = to_fraction(args[0]);
  auto arg1 = to_fraction(args[1]);
  if (arg1->value == BigFraction(BigInt(0))) throw error::internal_error("Division by zero");
  return std::make_shared<Fraction>(
        arg0->value / 
        arg1->value);
}

std::shared_ptr<Object> on_assign(arg_list args) {
  static_cast<variable::Variable*>(args[0].get())->set_value(args[1]);
  return args[1];
}

std::shared_ptr<type::Type> Get_Fraction_type() {
  static std::shared_ptr<type::Type> type = 
    std::make_shared<type::Type>();
  static bool init = false;
  if (init) return type;
  init = true;
  {
    auto func_obj = std::make_shared<TypedFunction>(Get_Fraction_type());
    func_obj->push_func(std::make_shared<InternalFunction>(on_add),{Get_Fraction_type(),Get_Fraction_type()});
    func_obj->push_func(std::make_shared<InternalFunction>(on_add),{Get_Fraction_type()});
    func_obj->push_func(std::make_shared<InternalFunction>(on_add),{Get_Fraction_type(),integer::Get_Integer_type()});
    type->members["+"] = func_obj;
    auto int_func_obj = static_cast<TypedFunction*>(integer::Get_Integer_type()->members["+"].get());
    int_func_obj->push_func(std::make_shared<InternalFunction>(on_add),{integer::Get_Integer_type(),Get_Fraction_type()});
  }
  {
    auto func_obj = std::make_shared<TypedFunction>(Get_Fraction_type());
    func_obj->push_func(std::make_shared<InternalFunction>(on_sub),{Get_Fraction_type(),Get_Fraction_type()});
    func_obj->push_func(std::make_shared<InternalFunction>(on_sub),{Get_Fraction_type()});
    func_obj->push_func(std::make_shared<InternalFunction>(on_sub),{Get_Fraction_type(),integer::Get_Integer_type()});
    type->members["-"] = func_obj;
    auto int_func_obj = static_cast<TypedFunction*>(integer::Get_Integer_type()->members["-"].get());
    int_func_obj->push_func(std::make_shared<InternalFunction>(on_sub),{integer::Get_Integer_type(),Get_Fraction_type()});
  }
  {
    auto func_obj = std::make_shared<TypedFunction>(Get_Fraction_type());
    func_obj->push_func(std::make_shared<InternalFunction>(on_mul),{Get_Fraction_type(),Get_Fraction_type()});
    func_obj->push_func(std::make_shared<InternalFunction>(on_mul),{Get_Fraction_type(),integer::Get_Integer_type()});
    type->members["*"] = func_obj;
    auto int_func_obj = static_cast<TypedFunction*>(integer::Get_Integer_type()->members["*"].get());
    int_func_obj->push_func(std::make_shared<InternalFunction>(on_mul),{integer::Get_Integer_type(),Get_Fraction_type()});
  }
  {
    auto func_obj = std::make_shared<TypedFunction>(Get_Fraction_type());
    func_obj->push_func(std::make_shared<InternalFunction>(on_div),{Get_Fraction_type(),Get_Fraction_type()});
    func_obj->push_func(std::make_shared<InternalFunction>(on_div),{Get_Fraction_type(),integer::Get_Integer_type()});
    type->members["/"] = func_obj;
    auto int_func_obj = static_cast<TypedFunction*>(integer::Get_Integer_type()->members["/"].get());
    int_func_obj->push_func(std::make_shared<InternalFunction>(on_div),{integer::Get_Integer_type(),Get_Fraction_type()});
  }
  {
    auto func_obj = std::make_shared<TypedFunction>(Get_Fraction_type());
    func_obj->push_func(std::make_shared<InternalFunction>(on_assign),{Get_Fraction_type(),Get_Fraction_type()});
    type->members["="] = func_obj;
  }
  return type;

}

Fraction::Fraction(BigFraction value) : value(value) {
  type = Get_Fraction_type();
}

}
