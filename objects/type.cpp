#include "type.hpp"

namespace type {
using namespace object;
  Type::Type(parser::location loc,std::string name) : Object(loc), name(name) {}
    const std::string Type::to_string() const {
    return name;
  }
}
