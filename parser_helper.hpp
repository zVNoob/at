#pragma once 

#include "objects/object.hpp"

#include <memory>
namespace parser {

std::shared_ptr<object::Object> exec_binary_op(std::shared_ptr<object::Object> lhs, 
                                               std::shared_ptr<object::Object> rhs, 
                                               std::string op);
std::shared_ptr<object::Object> exec_unary_op(std::shared_ptr<object::Object> rhs, 
                                              std::string op);

}
