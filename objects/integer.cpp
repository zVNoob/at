#include "integer.hpp"

#include "internal_func.hpp"
#include "../error.hpp"
#include "type.hpp"
#include <sstream>


namespace integer {
using namespace internal_func;
std::string Integer_sourcename = __FILE__;

std::vector<Object*> on_add(std::vector<Object*> args) {
  if (!dynamic_cast<Integer*>(args[0])) {
    throw error::type_mismatch(args[0]);
  }
  if (args.size() == 1) return {new Integer(static_cast<Integer*>(args[0])->value,args[0]->loc)};
  if (!dynamic_cast<Integer*>(args[1])) {
    throw error::type_mismatch(args[1]);
  }
  return {new Integer(static_cast<Integer*>(args[0])->value + static_cast<Integer*>(args[1])->value,
                     args[0]->loc)};
}

std::vector<Object*> on_sub(std::vector<Object*> args) {
  if (!dynamic_cast<Integer*>(args[0])) {
    throw error::type_mismatch(args[0]);
  }
  if (args.size() == 1) return {new Integer(-static_cast<Integer*>(args[0])->value,args[0]->loc)};
  if (!dynamic_cast<Integer*>(args[1])) {
    throw error::type_mismatch(args[1]);
  }
  return {new Integer(static_cast<Integer*>(args[0])->value - static_cast<Integer*>(args[1])->value,
                     args[0]->loc)};
}

std::vector<Object*> on_mul(std::vector<Object*> args) {
  if (!dynamic_cast<Integer*>(args[0])) {
    throw error::type_mismatch(args[0]);
  }
  if (args.size() == 1) return {new Integer(static_cast<Integer*>(args[0])->value,args[0]->loc)};
  if (!dynamic_cast<Integer*>(args[1])) {
    throw error::type_mismatch(args[1]);
  }
  return {new Integer(static_cast<Integer*>(args[0])->value * static_cast<Integer*>(args[1])->value,
                     args[0]->loc)};
}

type::Type* Get_Integer_type() {
  static type::Type* type = new type::Type(parser::location(&Integer_sourcename,__LINE__),"Integer");
  static bool init = false;
  if (init) return type;
  init = true;
  type->members["+"] = new InternalFunction(on_add,{Get_Integer_type(),Get_Integer_type()},Integer_sourcename,__LINE__);
  type->members["-"] = new InternalFunction(on_sub,{Get_Integer_type(),Get_Integer_type()},Integer_sourcename,__LINE__);
  type->members["*"] = new InternalFunction(on_mul,{Get_Integer_type(),Get_Integer_type()},Integer_sourcename,__LINE__);
  return type;
}

Integer::Integer(BigInt value,parser::location loc) : Object(loc) {
  this->value = value;
  this->type = Get_Integer_type();
}

const std::string Integer::to_string() const {
  std::stringstream ss;
  ss << this->value;
  return ss.str();
}

bool Integer::operator==(const Object* that) const {
  if (typeid(*that) != typeid(*this)) return false;
  auto _that = static_cast<const Integer*>(that);
  return this->value == _that->value;
}

}
