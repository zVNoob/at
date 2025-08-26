#include "parser_helper.hpp"

#include "parser.hpp"

#include "error.hpp"
#include "callable.hpp"
#include "type.hpp" // IWYU pragma: keep

namespace parser {
  std::shared_ptr<object::Object> exec_binary_op(std::shared_ptr<object::Object> lhs, 
                                                 std::shared_ptr<object::Object> rhs, 
                                                 const std::string& op) {
    if (lhs->type == nullptr) throw error::unsupported_operator(lhs,op);
    object::Object* op_obj = lhs->type->members[op].get();
    if (op_obj == nullptr) throw error::unsupported_operator(lhs,op);
    if (dynamic_cast<callable::Callable*>(op_obj) == nullptr) throw error::unsupported_operator(lhs,op); 
    return static_cast<callable::Callable*>(op_obj)->on_call({lhs,rhs})[0];
  }
  std::shared_ptr<object::Object> exec_unary_op(std::shared_ptr<object::Object> rhs, 
                                                const std::string& op) {
    if (rhs->type == nullptr) throw error::unsupported_operator(rhs,op);
    object::Object* op_obj = rhs->type->members[op].get();
    if (op_obj == nullptr) throw error::unsupported_operator(rhs,op);
    if (dynamic_cast<callable::Callable*>(op_obj) == nullptr) throw error::unsupported_operator(rhs,op); 
    return static_cast<callable::Callable*>(op_obj)->on_call({rhs})[0];
  }
  void exec_declare(const std::string& name, std::shared_ptr<object::Object> obj,lexer::Lexer* lexer) {
    lexer->scope->add_member(name, obj);
  }
}
