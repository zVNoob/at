#include "parser_helper.hpp"

#include "parser.hpp"

#include "error.hpp"
#include "callable.hpp"
#include "type.hpp" // IWYU pragma: keep
#include <memory>

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
  void exec_declare(const std::vector<std::string>& name, 
                    const std::vector<std::shared_ptr<object::Object>>& obj,
                    lexer::Lexer* lexer,parser::location loc) {
    if (name.size() > obj.size()) throw error::eval_error("Invalid declaration",loc);
    for (size_t i = 0; i < name.size(); i++) {
      lexer->scope->members[name[i]] = obj[i];
    }
  }
  void exec_assign(const std::vector<std::shared_ptr<variable::Variable>>& name, 
                   const std::vector<std::shared_ptr<object::Object>>& obj,
                   lexer::Lexer* lexer,parser::location loc) {
    if (name.size() > obj.size()) throw error::eval_error("Invalid assignment",loc);
    for (size_t i = 0; i < name.size(); i++) {
      exec_binary_op(std::static_pointer_cast<object::Object>(name[i]), obj[i],"=");
    }
  }
}
