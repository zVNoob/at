#include "scope.hpp"
#include "variable.hpp"

namespace scope {
void Scope::add_member(std::string name, std::shared_ptr<object::Object> value, bool readonly) {
  members[name].push_front({value, readonly});
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
  std::shared_ptr<object::Object> result = nullptr;
  for (auto& statement : statements) {
    result = statement->eval();
  }
  return result;
}

}
