#pragma once

#include "object.hpp"
#include "variable.hpp"
#include <list>
#include <map>
#include <utility>

namespace scope {

struct scope_data {
  std::shared_ptr<object::Object> value;
  bool readonly;
};

class Scope {
  public:
  std::shared_ptr<Scope> parent;
  std::map<std::string, std::list<scope_data>, std::less<>> members;
  void add_member(std::string name, std::shared_ptr<object::Object> value, bool readonly = false);
  std::pair<variable::Variable,bool> get_variable(std::string name,bool current_scope = true);
};

}
