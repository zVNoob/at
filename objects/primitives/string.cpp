#include "string.hpp"
#include "callable.hpp"
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
  return std::make_shared<String>(
        static_cast<String*>(args[0].get())->value + 
        static_cast<String*>(args[1].get())->value);
}

std::shared_ptr<Object> on_find(arg_list args) {
  return std::make_shared<integer::Integer>(
        BigInt(
          static_cast<String*>(args[0].get())->value.find(static_cast<String*>(args[1].get())->value)
        ));
}

std::shared_ptr<Object> on_length(arg_list args) {
  return std::make_shared<integer::Integer>(
        BigInt(
          static_cast<String*>(args[0].get())->value.length()
        ));
}

std::shared_ptr<Object> on_int(arg_list args) {
  BigInt value(0);
  if (static_cast<String*>(args[0].get())->value.size())
    value = BigInt(static_cast<String*>(args[0].get())->value[0]);
  return std::make_shared<integer::Integer>(value);
}

int utf8_encode(char *out, uint32_t utf)
{
  if (utf <= 0x7F) {
    // Plain ASCII
    out[0] = (char) utf;
    out[1] = 0;
    return 1;
  }
  else if (utf <= 0x07FF) {
    // 2-byte unicode
    out[0] = (char) (((utf >> 6) & 0x1F) | 0xC0);
    out[1] = (char) (((utf >> 0) & 0x3F) | 0x80);
    out[2] = 0;
    return 2;
  }
  else if (utf <= 0xFFFF) {
    // 3-byte unicode
    out[0] = (char) (((utf >> 12) & 0x0F) | 0xE0);
    out[1] = (char) (((utf >>  6) & 0x3F) | 0x80);
    out[2] = (char) (((utf >>  0) & 0x3F) | 0x80);
    out[3] = 0;
    return 3;
  }
  else if (utf <= 0x10FFFF) {
    // 4-byte unicode
    out[0] = (char) (((utf >> 18) & 0x07) | 0xF0);
    out[1] = (char) (((utf >> 12) & 0x3F) | 0x80);
    out[2] = (char) (((utf >>  6) & 0x3F) | 0x80);
    out[3] = (char) (((utf >>  0) & 0x3F) | 0x80);
    out[4] = 0;
    return 4;
  }
  else { 
    // error - use replacement character
    out[0] = (char) 0xEF;  
    out[1] = (char) 0xBF;
    out[2] = (char) 0xBD;
    out[3] = 0;
    return 0;
  }
}

std::shared_ptr<Object> on_add_int(arg_list args) {
  std::string value = static_cast<String*>(args[0].get())->value;
  char out[5] = {0,0,0,0,0};
  utf8_encode(out,static_cast<integer::Integer*>(args[1].get())->value.to_long());
  value += out;
  return std::make_shared<String>(value);
}

std::shared_ptr<Object> on_get(arg_list args) {
  int index = static_cast<integer::Integer*>(args[1].get())->value.to_long();
  if (index < 0) index = static_cast<String*>(args[0].get())->value.size() + index;
  if (index < 0 || index >= static_cast<String*>(args[0].get())->value.size())
    throw error::internal_error("Index out of range");
  return std::make_shared<String>(
        std::string(1,static_cast<String*>(args[0].get())->value[index]));
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
    func_obj->push_func(std::make_shared<InternalFunction>(on_add_int),{Get_String_type(),integer::Get_Integer_type()});
    type->members["+"] = func_obj;
  }
  {
    auto func_obj = std::make_shared<TypedFunction>(Get_String_type());
    func_obj->push_func(std::make_shared<InternalFunction>(on_int),{Get_String_type()});
    type->members["-"] = func_obj;
  }
  {
    auto func_obj = std::make_shared<TypedFunction>(Get_String_type());
    func_obj->push_func(std::make_shared<InternalFunction>(on_find),{Get_String_type(),Get_String_type()});
    type->members["%"] = func_obj;
  }
  {
    auto func_obj = std::make_shared<TypedFunction>(integer::Get_Integer_type());
    func_obj->push_func(std::make_shared<InternalFunction>(on_length),{Get_String_type()});
    type->members["~"] = func_obj;
  }
  {
    auto func_obj = std::make_shared<TypedFunction>(Get_String_type());
    func_obj->push_func(std::make_shared<InternalFunction>(on_get),{Get_String_type(),integer::Get_Integer_type()});
    type->members["[]"] = func_obj;
  }
  return type;
}

String::String(std::string value) : value(value) {
  type = Get_String_type();
}

}
