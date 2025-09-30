#pragma once

#include "object.hpp"
#include "bigfraction.hpp"

namespace fraction {
  class Fraction : public object::Object {
    public:
    BigFraction value;
    Fraction(BigFraction value);
};
std::shared_ptr<type::Type> Get_Fraction_type();
}
