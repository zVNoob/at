#include "string.hpp"
#include "type.hpp"
#include "internal_func.hpp"

#include <vector>
#include "../error.hpp"

namespace string {
using namespace object;
using namespace internal_func;
std::string String_sourcename = __FILE__;

std::vector<Object*> on_add(std::vector<Object*> args) {
  if (!dynamic_cast<String*>(args[0])) {
    throw error::type_mismatch(args[0]);
  }
  if (args.size() == 1) return {new String(static_cast<String*>(args[0])->value,args[0]->loc)};
  if (!dynamic_cast<String*>(args[1])) {
    throw error::type_mismatch(args[1]);
  }
  return {new String(static_cast<String*>(args[0])->value + 
                     static_cast<String*>(args[1])->value,args[0]->loc)};
}


type::Type* Get_String_type() {
  static type::Type* type = new type::Type(parser::location(&String_sourcename,__LINE__),"String");
  static bool init = false;
  if (init) return type;
  init = true;
  type->members["+"] = new InternalFunction(on_add,{Get_String_type(),Get_String_type()},String_sourcename,__LINE__);
  return type;
}

String::String(std::string value, parser::location loc) : Object(loc), value(value) {
  type = Get_String_type();
}

const std::string String::to_string() const { return "\"" + this->value + "\""; }

bool String::operator==(const Object* that) const { 
  if (typeid(*that) != typeid(*this)) return false;
  return this->value == static_cast<const String*>(that)->value;
}

}
