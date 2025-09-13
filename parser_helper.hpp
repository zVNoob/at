#pragma once

#include <memory>
#include <vector>

#include "lexer.hpp"
#include "object.hpp"
#include "location.hh"

namespace parser {
std::shared_ptr<object::Object> exec_binary_op(std::shared_ptr<object::Object> lhs, 
                                               std::shared_ptr<object::Object> rhs, 
                                               const std::string& op,parser::location loc);
std::shared_ptr<object::Object> exec_unary_op(std::shared_ptr<object::Object> rhs, 
                                              const std::string& op,parser::location loc);

std::shared_ptr<object::Object> exec_call(std::shared_ptr<object::Object> obj, 
                                         std::vector<std::shared_ptr<object::Object>> args, 
                                         std::string method,parser::location loc);

std::shared_ptr<object::Object> exec_assign(std::vector<std::shared_ptr<object::Object>> vars, 
                 std::shared_ptr<object::Object> rhs, 
                 parser::location loc);
std::shared_ptr<object::Object> exec_assign_single(std::shared_ptr<object::Object> lhs,
                        std::shared_ptr<object::Object> rhs,
                        parser::location loc);

std::shared_ptr<object::Object> exec_declare(std::vector<std::shared_ptr<object::Object>> vars,
                  std::shared_ptr<object::Object> rhs, lexer::Lexer* lexer,parser::location loc);
std::shared_ptr<object::Object> exec_build_array(std::vector<std::shared_ptr<object::Object>> elements,parser::location loc);

}
