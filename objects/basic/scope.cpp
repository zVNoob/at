#include "scope.hpp"

#include "object.hpp"
#include "type.hpp"

#include <utility>

namespace scope {
using namespace object;
void Scope::add_member(std::string name, std::shared_ptr<object::Object> obj) {
  members[name] = obj;
}

std::pair<std::shared_ptr<variable::Variable>, bool> Scope::get_member(std::string name) {
  auto it = members.find(name);
  if (it == members.end()) {
    if (!parent) return std::make_pair(
      nullptr,
      false);
    return std::make_pair(parent->get_member(name).first,false);
  }
  return std::make_pair(std::make_shared<variable::Variable>(name, it->second, loc), true);
}

Scope::Scope(std::shared_ptr<Scope> parent) : Object(parent->loc),parent(parent) {
  type = parent->type;
}

Scope::Scope(parser::location loc) : Object(loc) {
  this->type = std::make_shared<type::Type>(loc,*loc.begin.filename);
}

std::string Scope::to_string() const {
  return *loc.begin.filename;
}

}
