#pragma once

#include <map>
#include <memory>
#include <string>
#include "object.hpp"

namespace type {
class Type : public object::Object {
public:
  std::map<std::string, std::shared_ptr<object::Object>, std::less<>> members;
};
}
