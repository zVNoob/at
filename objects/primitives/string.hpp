#pragma once

#include "object.hpp"
#include <memory>

namespace string {
class String : public object::Object {
public:
  std::string value;
  String(std::string value);
};
std::shared_ptr<type::Type> Get_String_type();
}
