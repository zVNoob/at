#pragma once

#include "object.hpp"
#include "bigint.hpp"

#include <memory>

namespace integer {
class Integer : public object::Object {
  public:
  BigInt value;
  explicit Integer(BigInt value);
};
std::shared_ptr<type::Type> Get_Integer_type();
}
