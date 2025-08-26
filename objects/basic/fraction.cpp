#include "fraction.hpp"
#include "callable.hpp"
#include "integer.hpp"
#include "internal_func.hpp"
#include "typed_func.hpp"
#include "type.hpp"
#include <memory>
#include <sstream>

namespace fraction {
const std::string Fraction_sourcename = __FILE__;
using namespace callable;
using namespace internal_func;
using namespace typed_func;

std::shared_ptr<Fraction> to_fraction(std::shared_ptr<object::Object> obj) {
  if (dynamic_cast<Fraction*>(obj.get())) return std::make_shared<Fraction>(static_cast<Fraction*>(obj.get())->value,obj->loc);
  return std::make_shared<Fraction>(BigFraction(static_cast<integer::Integer*>(obj.get())->value),obj->loc);
}

arg_list on_add(arg_list args) {
  if (args.size() == 1) return {
    std::make_shared<Fraction>(static_cast<Fraction*>(args[0].get())->value,args[0]->loc)};
  auto arg0 = to_fraction(args[0]);
  auto arg1 = to_fraction(args[1]);
  return {
    std::make_shared<Fraction>(
        arg0->value + 
        arg1->value,
      args[0]->loc)};
}
arg_list on_sub(arg_list args) {
  if (args.size() == 1) return {
    std::make_shared<Fraction>(-static_cast<Fraction*>(args[0].get())->value,args[0]->loc)};
  auto arg0 = to_fraction(args[0]);
  auto arg1 = to_fraction(args[1]);
  return {
    std::make_shared<Fraction>(
        arg0->value - 
        arg1->value,
      args[0]->loc)};
}
arg_list on_mul(arg_list args) {
  auto arg0 = to_fraction(args[0]);
  auto arg1 = to_fraction(args[1]);
  return {
    std::make_shared<Fraction>(
        arg0->value * 
        arg1->value,
      args[0]->loc)};
}

std::shared_ptr<type::Type> Get_Fraction_type() {
  static std::shared_ptr<type::Type> type = 
    std::make_shared<type::Type>(parser::location(&Fraction_sourcename,__LINE__),"Fraction");
  static bool init = false;
  if (init) return type;
  init = true;
  {
    auto func_obj = std::make_shared<TypedFunction>(Get_Fraction_type(),parser::location(&Fraction_sourcename,__LINE__));
    func_obj->push_func(std::make_shared<InternalFunction>(on_add),{Get_Fraction_type(),Get_Fraction_type()});
    func_obj->push_func(std::make_shared<InternalFunction>(on_add),{Get_Fraction_type()});
    func_obj->push_func(std::make_shared<InternalFunction>(on_add),{Get_Fraction_type(),integer::Get_Integer_type()});
    type->members["+"] = func_obj;
    auto int_func_obj = static_cast<TypedFunction*>(integer::Get_Integer_type()->members["+"].get());
    int_func_obj->push_func(std::make_shared<InternalFunction>(on_add),{integer::Get_Integer_type(),Get_Fraction_type()});
  }
  {
    auto func_obj = std::make_shared<TypedFunction>(Get_Fraction_type(),parser::location(&Fraction_sourcename,__LINE__));
    func_obj->push_func(std::make_shared<InternalFunction>(on_sub),{Get_Fraction_type(),Get_Fraction_type()});
    func_obj->push_func(std::make_shared<InternalFunction>(on_sub),{Get_Fraction_type()});
    func_obj->push_func(std::make_shared<InternalFunction>(on_sub),{Get_Fraction_type(),integer::Get_Integer_type()});
    type->members["-"] = func_obj;
    auto int_func_obj = static_cast<TypedFunction*>(integer::Get_Integer_type()->members["-"].get());
    int_func_obj->push_func(std::make_shared<InternalFunction>(on_sub),{integer::Get_Integer_type(),Get_Fraction_type()});
  }
  {
    auto func_obj = std::make_shared<TypedFunction>(Get_Fraction_type(),parser::location(&Fraction_sourcename,__LINE__));
    func_obj->push_func(std::make_shared<InternalFunction>(on_mul),{Get_Fraction_type(),Get_Fraction_type()});
    func_obj->push_func(std::make_shared<InternalFunction>(on_mul),{Get_Fraction_type(),integer::Get_Integer_type()});
    type->members["*"] = func_obj;
    auto int_func_obj = static_cast<TypedFunction*>(integer::Get_Integer_type()->members["*"].get());
    int_func_obj->push_func(std::make_shared<InternalFunction>(on_mul),{integer::Get_Integer_type(),Get_Fraction_type()});
  }
  return type;

}  

std::string Fraction::to_string() const {
  std::stringstream ss;
  ss << value;
  return ss.str();
}

Fraction::Fraction(BigFraction value, parser::location loc) : Object(loc) , value(value) {
  type = Get_Fraction_type();
}

}
