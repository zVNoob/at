#pragma once

#include <map>
#include <string>
#include "object.hpp"

namespace type {
using namespace object;
class Type : public Object {
  std::string name;
public:
  std::map<std::string, Object*> members;
  Type(parser::location loc,std::string name);
  bool operator==(const Object* that) const override;
  const std::string to_string() const override;
};

}
