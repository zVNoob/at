#pragma once

#include "object.hpp"
#include "../type/bigint.hpp"

namespace integer {
class Integer : public object::Object {
  public:
  BigInt value;
  explicit Integer(BigInt value,parser::location loc);
  std::string to_string() const override;
};
std::shared_ptr<type::Type> Get_Integer_type();
}
