#pragma once

#include "object.hpp"
#include "variable.hpp"

#include <map>

namespace scope {

class Scope : public object::Object {
public:
  std::shared_ptr<Scope> parent;
  std::map<std::string, std::shared_ptr<object::Object>> members;
  void add_member(std::string name, std::shared_ptr<object::Object> obj);
  std::pair<std::shared_ptr<variable::Variable>, bool> get_member(std::string name);
  explicit Scope(std::shared_ptr<Scope> parent);
  explicit Scope(parser::location loc);
  std::string to_string() const override;
};
}
