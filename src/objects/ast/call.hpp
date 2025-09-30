#pragma once

#include "ast.hpp"
#include "object.hpp"
#include "typed_func.hpp"
#include "variable.hpp"
#include "unknown_variable.hpp" 
#include <memory>
#include <vector>

namespace ast {

class Call : public Ast {
public:
  std::shared_ptr<typed_func::TypedFunction> callee;
  std::vector<std::shared_ptr<object::Object>> args;
  Call(std::shared_ptr<typed_func::TypedFunction> callee, std::vector<std::shared_ptr<object::Object>> args, parser::location location) : callee(callee), args(args) {
    this->location = location;
    this->type = callee->return_type;
  }
  std::shared_ptr<object::Object> eval() override { 
    std::vector<std::shared_ptr<object::Object>> new_args = args;
    for (auto& arg : new_args) {
      if (dynamic_cast<ast::Ast*>(arg.get())) {
        arg = std::static_pointer_cast<ast::Ast>(arg)->eval();
      }
      if (dynamic_cast<variable::Variable*>(arg.get())) 
        arg = std::static_pointer_cast<variable::Variable>(arg)->get_value();
    }
    return callee->on_call(new_args,location);
  }
};

class Assign_Operator : public Ast {
public:
  std::shared_ptr<typed_func::TypedFunction> callee;
  std::shared_ptr<ast::UnknownVariable> var;
  std::shared_ptr<object::Object> value;
  Assign_Operator(std::shared_ptr<typed_func::TypedFunction> callee,
                  std::shared_ptr<ast::UnknownVariable> var, 
                  std::shared_ptr<object::Object> value, parser::location location) : callee(callee), var(var), value(value) {
    this->location = location;
    this->type = var->type;
  }
  std::shared_ptr<object::Object> eval() override {
    auto new_value = value;
    if (dynamic_cast<ast::Ast*>(new_value.get())) {
      new_value = std::static_pointer_cast<ast::Ast>(new_value)->eval();
    }
    if (dynamic_cast<variable::Variable*>(new_value.get())) 
      new_value = std::static_pointer_cast<variable::Variable>(new_value)->get_value();
    callee->on_call({var->var,new_value}, location);
    return nullptr;
  }
};

}
