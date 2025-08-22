#pragma once

#include "object.hpp"
#include <memory>

namespace string {
using namespace object;
class String : public Object {
public:
  std::string value;
  String(std::string value, parser::location loc);
  const std::string to_string() const override;
  bool operator==(const Object* that) const override;
};
std::shared_ptr<type::Type> Get_String_type();
}
