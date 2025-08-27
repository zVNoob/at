#pragma once

#include "object.hpp"

namespace variable {

class Variable : public object::Object {
public:
  std::string name;
  std::shared_ptr<object::Object>& value;
  explicit Variable(std::string name, std::shared_ptr<object::Object>& value, parser::location loc) :
    Object(loc), name(name), value(value) {
    type = value->type;
  };
  std::string to_string() const override { return name + (value?("("+value->to_string()+")"):""); }
  std::shared_ptr<object::Object> get_value() const { return value; }
};

}
