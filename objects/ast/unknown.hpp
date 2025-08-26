#pragma once

#include "object.hpp"

namespace object {
class Unknown : public Object {
public:
  explicit Unknown(parser::location loc) : Object(loc) {
    static std::shared_ptr<type::Type> type = std::make_shared<type::Type>(loc,"unknown");
    this->type = type;
  }
  std::string to_string() const override { return "<unknown>"; }
};
}
