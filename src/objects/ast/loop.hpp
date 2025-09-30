#pragma once

#include "ast.hpp"
#include "object.hpp"
#include "scope.hpp"
#include "integer.hpp"
#include "error.hpp"

namespace ast {
class Loop : public scope::Scope {
public:
  std::shared_ptr<object::Object> condition;
  Loop(std::shared_ptr<object::Object> condition, parser::location loc) : condition(condition) {
    if (condition->type != integer::Get_Integer_type())
      throw error::eval_error("Condition must be an integer",loc);
    this->location = loc;
    this->auto_resolve = false;
  }
  std::shared_ptr<object::Object> eval() override {
    while (true) {
      std::shared_ptr<object::Object> cond = condition;
      if (dynamic_cast<ast::Ast*>(cond.get())) 
        cond = std::static_pointer_cast<ast::Ast>(cond)->eval();
      if (std::static_pointer_cast<integer::Integer>(cond)->value == BigInt(0))
        break;
      Scope::eval();
    }
    return nullptr;
  }
};
}
