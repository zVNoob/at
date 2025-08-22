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
  explicit Object(parser::location loc) : loc(loc) {}
  virtual std::string to_string() const { return "None"; }
  virtual ~Object() = default;
};
}
