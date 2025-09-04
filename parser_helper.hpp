#pragma once

#include <memory>

#include "object.hpp"
#include "location.hh"

namespace parser {
std::shared_ptr<object::Object> exec_binary_op(std::shared_ptr<object::Object> lhs, 
                                               std::shared_ptr<object::Object> rhs, 
                                               const std::string& op,parser::location loc);
std::shared_ptr<object::Object> exec_unary_op(std::shared_ptr<object::Object> rhs, 
                                              const std::string& op,parser::location loc);
}
