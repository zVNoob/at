#pragma once

#include "object.hpp"
#include "../type/bigfraction.hpp"

namespace fraction {
  class Fraction : public object::Object {
    public:
    BigFraction value;
    Fraction(BigFraction value, parser::location loc);
    std::string to_string() const override;
};
std::shared_ptr<type::Type> Get_Fraction_type();
}
