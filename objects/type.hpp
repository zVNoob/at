#pragma once

#include <map>
#include <memory>
#include <string>
#include "object.hpp"

namespace type {
class Type : public object::Object {
  std::string name;
public:
  std::map<std::string, std::shared_ptr<Object>, std::less<>> members;
  explicit Type(parser::location loc,std::string name);
  std::string to_string() const override;
};
}
