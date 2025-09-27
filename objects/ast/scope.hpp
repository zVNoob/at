#pragma once

#include "object.hpp"
#include "type.hpp"
#include "variable.hpp"
#include "ast.hpp"
#include <list>
#include <map>
#include <utility>
#include <vector>

namespace scope {

struct scope_data {
  std::shared_ptr<object::Object> value;
  bool readonly;
};

class Scope : public ast::Ast {
  public:
  std::shared_ptr<Scope> parent;
  std::vector<std::shared_ptr<ast::Ast>> statements;
  std::map<std::string, std::list<scope_data>, std::less<>> members;
  std::map<std::string, std::shared_ptr<type::Type>, std::less<>> types;
  void add_member(std::string name, std::shared_ptr<object::Object> value, bool readonly = false);
  std::pair<variable::Variable,bool> get_variable(std::string name,bool current_scope = true);
  void add_statement(std::shared_ptr<ast::Ast> statement);
  std::shared_ptr<object::Object> eval() override;
};

}
