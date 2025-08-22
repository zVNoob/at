#pragma once

#include <map>
#include <memory>
#include <string>
#include "object.hpp"

namespace type {
class Type : public object::Object {
  std::string name;
public:
  std::map<std::string, std::shared_ptr<Object>> members;
  Type(parser::location loc,std::string name);
  bool operator==(const Object* that) const override;
  const std::string to_string() const override;
};
}
