#include "array.hpp"

#include "internal_func.hpp"
#include "callable.hpp"
#include "object.hpp"
#include "type.hpp"

namespace array {
using namespace internal_func;
using namespace callable;
using namespace object;
const std::string Array_sourcename = __FILE__;

std::shared_ptr<type::Type> Get_Array_type() {
  static std::shared_ptr<type::Type> type = 
    std::make_shared<type::Type>(parser::location(&Array_sourcename,__LINE__),"Array");
  static bool init = false;
  if (init) return type;
  init = true;
  return type;
}

Array::Array(parser::location loc) : Object(loc) {
  type = Get_Array_type();
}

std::string Array::to_string() const { 
  std::string result = "[";
  for (auto& e : elements) {
    result += e->to_string();
    result += ", ";
  }
  if (elements.size()) {
    result.pop_back();
    result.pop_back();
  }
  result += "]";
  return result;
}
}
