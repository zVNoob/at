#pragma once

#include "object.hpp"

namespace ast {

class Ast : public object::Object {
public:
  Ast() : object::Object() {}
  virtual ~Ast() = default;
  virtual std::shared_ptr<object::Object> eval() = 0;
};

}
