#include "parser_helper.hpp"
#include "callable.hpp"
#include "error.hpp"
#include "object.hpp"
#include "type.hpp" // IWYU pragma: keep

namespace parser {
std::shared_ptr<object::Object> exec_binary_op(std::shared_ptr<object::Object> lhs, 
                         std::shared_ptr<object::Object> rhs, 
                         const std::string& op,parser::location loc) {
  if (lhs->type == nullptr) throw error::eval_error("No type found",loc);
  object::Object* obj = lhs->type->members[op].get();
  if (dynamic_cast<callable::Callable*>(obj) == nullptr) 
    throw error::eval_error("This operator is not defined for that data type",loc);
  return static_cast<callable::Callable*>(obj)->on_call({lhs,rhs},loc);
}

std::shared_ptr<object::Object> exec_unary_op(std::shared_ptr<object::Object> rhs, 
                                              const std::string& op,parser::location loc) {
  if (rhs->type == nullptr) throw error::eval_error("No type found",loc);
  object::Object* obj = rhs->type->members[op].get();
  if (dynamic_cast<callable::Callable*>(obj) == nullptr) 
    throw error::eval_error("This operator is not defined for that data type",loc);
  return static_cast<callable::Callable*>(obj)->on_call({rhs},loc);
}
}
