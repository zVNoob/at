#pragma once

#include "object.hpp"

namespace variable {

class Variable : public object::Object {
public:
  std::string name;
  std::shared_ptr<object::Object>* value;
  bool re_declarable;
  bool readonly;
  explicit Variable(std::string name, std::shared_ptr<object::Object>* value = nullptr, bool re_declarable = true, bool readonly = false) :
    name(name), value(value), re_declarable(re_declarable), readonly(readonly) {
    if (value) type = value->get()->type;
  };
  virtual std::shared_ptr<object::Object> get_value() {
    return *value;
  };
  virtual void set_value(std::shared_ptr<object::Object> value) {
    *this->value = value;
  }
};

}
