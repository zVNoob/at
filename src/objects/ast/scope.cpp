#include "scope.hpp"
#include "variable.hpp"
#include "call.hpp"
#include <memory>

namespace scope {
void Scope::add_member(std::string name, std::shared_ptr<object::Object> value, bool readonly) {
  members[name].push_front({value, readonly});
  if (auto_resolve == false) {
    if (value->type->members["="] == nullptr) return;
    add_statement(
      std::make_shared<ast::Assign_Operator>(
        std::static_pointer_cast<typed_func::TypedFunction>(value->type->members["="]),
        std::make_shared<ast::UnknownVariable>(
          std::make_shared<variable::Variable>(get_variable(name).first)), 
        value, 
        location));
  }
}
std::pair<variable::Variable,bool> Scope::get_variable(std::string name,bool current_scope) {
  auto it = members.find(name);
  if (it == members.end()) {
    if (parent) return parent->get_variable(name, false);
    return {variable::Variable(name),false};
  }
  return {variable::Variable(name, &it->second.front().value, current_scope, members[name].front().readonly),true};
}

void Scope::add_statement(std::shared_ptr<ast::Ast> statement) {
  statements.push_back(statement);
}

std::shared_ptr<object::Object> Scope::eval() {
  for (auto& statement : statements) {
    statement->eval();
  }
  return nullptr;
}

}
