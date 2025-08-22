#pragma once

#include "object.hpp"
#include "../type/bigint.hpp"

namespace integer {
class Integer : public object::Object {
  public:
  BigInt value;
  Integer(BigInt value,parser::location loc);
  const std::string to_string() const override;
  bool operator==(const Object* that) const override;
};
std::shared_ptr<type::Type> Get_Integer_type();
}
