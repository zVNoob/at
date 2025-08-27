#pragma once 

#include "object.hpp"
#include "lexer.hpp"
#include <memory>
namespace parser {

std::shared_ptr<object::Object> exec_binary_op(std::shared_ptr<object::Object> lhs, 
                                               std::shared_ptr<object::Object> rhs, 
                                               const std::string& op);
std::shared_ptr<object::Object> exec_unary_op(std::shared_ptr<object::Object> rhs, 
                                              const std::string& op);
void exec_declare(const std::vector<std::string>& name, 
                  const std::vector<std::shared_ptr<object::Object>>& obj,
                  lexer::Lexer* lexer,parser::location loc);
void exec_assign(const std::vector<std::shared_ptr<variable::Variable>>& name, 
                 const std::vector<std::shared_ptr<object::Object>>& obj,
                 lexer::Lexer* lexer,parser::location loc);
}
