#include "string.hpp"
#include "callable.hpp"
#include "type.hpp"
#include "internal_func.hpp"
#include "typed_func.hpp"
#include "integer.hpp"
#include <memory>

namespace string {
using namespace internal_func;
using namespace callable;
using namespace object;
using namespace typed_func;
const std::string String_sourcename = __FILE__;

arg_list on_add(arg_list args) {
  return {
    std::make_shared<String>(
        static_cast<String*>(args[0].get())->value + 
        static_cast<String*>(args[1].get())->value,
      args[0]->loc)};
}

arg_list on_find(arg_list args) {
  return {
    std::make_shared<integer::Integer>(
        BigInt(
          static_cast<String*>(args[0].get())->value.find(static_cast<String*>(args[1].get())->value)
        ),
      args[0]->loc)};
}


std::shared_ptr<type::Type> Get_String_type() {
  static std::shared_ptr<type::Type> type = 
    std::make_shared<type::Type>(parser::location(&String_sourcename,__LINE__),"String");
  static bool init = false;
  if (init) return type;
  init = true;
  {
    auto func_obj = std::make_shared<TypedFunction>(Get_String_type(),parser::location(&String_sourcename,__LINE__));
    func_obj->push_func(std::make_shared<InternalFunction>(on_add),{Get_String_type(),Get_String_type()});
    type->members["+"] = func_obj;
  }
  {
    auto func_obj = std::make_shared<TypedFunction>(Get_String_type(),parser::location(&String_sourcename,__LINE__));
    func_obj->push_func(std::make_shared<InternalFunction>(on_find),{Get_String_type(),Get_String_type()});
    type->members["%"] = func_obj;
  }
  return type;
}

String::String(std::string value, parser::location loc) : Object(loc), value(value) {
  type = Get_String_type();
}

std::string String::to_string() const { return "\"" + this->value + "\""; }

}
