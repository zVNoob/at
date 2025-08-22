#include "parser_helper.hpp"

#include "error.hpp"
#include "objects/callable.hpp"
#include "objects/type.hpp"
namespace parser {
  object::Object* exec_binary_op(object::Object* lhs, object::Object* rhs, std::string op) {
    if (lhs->type == nullptr) throw error::unsupported_operator(lhs,op);
    object::Object* op_obj = lhs->type->members[op];
    if (op_obj == nullptr) throw error::unsupported_operator(lhs,op);
    if (dynamic_cast<callable::Callable*>(op_obj) == nullptr) throw error::unsupported_operator(lhs,op); 
    return static_cast<callable::Callable*>(op_obj)->on_call({lhs,rhs})[0];
  }
  object::Object* exec_unary_op(object::Object* rhs, std::string op) {
    if (rhs->type == nullptr) throw error::unsupported_operator(rhs,op);
    object::Object* op_obj = rhs->type->members[op];
    if (op_obj == nullptr) throw error::unsupported_operator(rhs,op);
    if (dynamic_cast<callable::Callable*>(op_obj) == nullptr) throw error::unsupported_operator(rhs,op); 
    return static_cast<callable::Callable*>(op_obj)->on_call({rhs})[0];
  }
}
