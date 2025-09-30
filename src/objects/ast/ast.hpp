#pragma once

#include "object.hpp"
#include "location.hh"

namespace ast {

class Ast : public object::Object {
public:
  parser::location location;
  Ast() : object::Object() {}
  virtual ~Ast() = default;
  virtual std::shared_ptr<object::Object> eval() = 0;
};

}
