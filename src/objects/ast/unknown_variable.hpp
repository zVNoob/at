#pragma once

#include "ast.hpp"
#include "variable.hpp"
#include <memory>

namespace ast {
class UnknownVariable : public Ast {
public:
  std::shared_ptr<variable::Variable> var;
  std::shared_ptr<object::Object> eval() override { return var->get_value(); }
  UnknownVariable(std::shared_ptr<variable::Variable> var) : var(var) {
    this->type = var->type;
  }
};
}
