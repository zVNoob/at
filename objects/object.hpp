#pragma once

#include <string>
#include <memory>
#include "../location.hh"

namespace type { class Type; }
namespace object {
class Object {
public:
  parser::location loc;
  std::shared_ptr<type::Type> type;
  Object(parser::location loc) : loc(loc) {}
  virtual const std::string to_string() const { return "None"; }
  virtual bool operator==(const Object* that) const { return typeid(*this) == typeid(*that); }
  bool operator!=(const Object* that) const { return !(*this == that); }
  ~Object() {}
};
}
