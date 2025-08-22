#include "type.hpp"

namespace type {
  Type::Type(parser::location loc,std::string name) : Object(loc), name(name) {}
  bool Type::operator==(const Object* that) const {
    auto _that = static_cast<const Type*>(that);
    for (auto& item : members) {
      auto that_obj = _that->members.find(item.first);
      if (that_obj == _that->members.end()) return false;
      if (*(that_obj->second.get()) != item.second.get()) return false;
    }
    return true;
  }
  const std::string Type::to_string() const {
    return name;
  }
}
