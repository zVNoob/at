#pragma once 

#include "objects/object.hpp"

namespace parser {

object::Object* exec_binary_op(object::Object* lhs, object::Object* rhs, std::string op);
object::Object* exec_unary_op(object::Object* rhs, std::string op);

}
