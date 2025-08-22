#include "string.hpp"
#include "callable.hpp"
#include "type.hpp"
#include "internal_func.hpp"

#include <memory>
#include "../error.hpp"

namespace string {
using namespace internal_func;
std::string String_sourcename = __FILE__;

arg_list on_add(arg_list args) {
  return {
    std::make_shared<String>(
        static_cast<String*>(args[0].get())->value + 
        static_cast<String*>(args[1].get())->value,
      args[0]->loc)};
}


std::shared_ptr<type::Type> Get_String_type() {
  static std::shared_ptr<type::Type> type = 
    std::make_shared<type::Type>(parser::location(&String_sourcename,__LINE__),"String");
  static bool init = false;
  if (init) return type;
  init = true;
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
