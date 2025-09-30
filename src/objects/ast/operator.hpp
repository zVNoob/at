#pragma once

#include "ast.hpp"
#include "object.hpp"
#include "type.hpp" // IWYU pragma: keep
#include "callable.hpp"
#include "error.hpp"
#include "typed_func.hpp"
#include "unknown_variable.hpp"
#include <memory>

namespace ast {

class Binary_Operator : public Ast {
public:
  std::string op;
  std::shared_ptr<object::Object> left;
  std::shared_ptr<object::Object> right;
  Binary_Operator(std::string op, std::shared_ptr<object::Object> left, std::shared_ptr<object::Object> right, parser::location location) 
  : op(op), left(left), right(right) {
    this->location = location;
    if (dynamic_cast<typed_func::TypedFunction*>(left->type->members[op].get()) == nullptr) 
      throw error::eval_error("This operator is not defined for that data type",location);
    this->type = std::static_pointer_cast<typed_func::TypedFunction>(left->type->members[op])->return_type;
    if (!std::static_pointer_cast<typed_func::TypedFunction>(left->type->members[op])->found_func({left,right})) 
      throw error::eval_error("No function found",location);
  }
  std::shared_ptr<object::Object> eval() override {
    std::shared_ptr<object::Object> lhs = left;
    std::shared_ptr<object::Object> rhs = right;
    if (dynamic_cast<ast::Ast*>(lhs.get())) lhs = std::static_pointer_cast<ast::Ast>(lhs)->eval();
    if (dynamic_cast<ast::Ast*>(rhs.get())) rhs = std::static_pointer_cast<ast::Ast>(rhs)->eval();
    return static_cast<callable::Callable*>(lhs->type->members[op].get())->on_call({lhs,rhs},location);
  }
};

class Assign_Operator : public Ast {
public:
  std::shared_ptr<UnknownVariable> left;
  std::shared_ptr<object::Object> right;
  Assign_Operator(std::shared_ptr<UnknownVariable> left, std::shared_ptr<object::Object> right, parser::location location) 
  : left(left), right(right) {
    this->location = location;
    if (dynamic_cast<typed_func::TypedFunction*>(left->type->members["="].get()) == nullptr) 
      throw error::eval_error("This operator is not defined for that data type",location);
    this->type = std::static_pointer_cast<typed_func::TypedFunction>(left->type->members["="])->return_type;
    if (!std::static_pointer_cast<typed_func::TypedFunction>(left->type->members["="])->found_func({left,right})) 
      throw error::eval_error("No function found",location);
  }
  std::shared_ptr<object::Object> eval() override {
    std::shared_ptr<object::Object> lhs = left->var;
    std::shared_ptr<object::Object> rhs = right;
    if (dynamic_cast<ast::Ast*>(rhs.get())) rhs = std::static_pointer_cast<ast::Ast>(rhs)->eval();
    return static_cast<callable::Callable*>(lhs->type->members["="].get())->on_call({lhs,rhs},location);
  }
};

class Unary_Operator : public Ast {
public:
  std::string op;
  std::shared_ptr<object::Object> obj;
  Unary_Operator(std::string op, std::shared_ptr<object::Object> obj, parser::location location) 
  : op(op), obj(obj) {
    this->location = location;
    if (dynamic_cast<typed_func::TypedFunction*>(obj->type->members[op].get()) == nullptr) 
      throw error::eval_error("This operator is not defined for that data type",location);
    this->type = std::static_pointer_cast<typed_func::TypedFunction>(obj->type->members[op])->return_type;
    if (!std::static_pointer_cast<typed_func::TypedFunction>(obj->type->members[op])->found_func({obj})) 
      throw error::eval_error("No function found",location);
  }
  std::shared_ptr<object::Object> eval() override {
    std::shared_ptr<object::Object> lhs = obj;
    if (dynamic_cast<ast::Ast*>(lhs.get())) lhs = std::static_pointer_cast<ast::Ast>(lhs)->eval();
    return static_cast<callable::Callable*>(lhs->type->members[op].get())->on_call({lhs},location);
  }
};


}
