#pragma once

#include "object.hpp"
#include <list>
#include <map>

namespace scope {

struct scope_data {
  std::shared_ptr<object::Object> value;
  bool readonly;
};

class Scope {
  public:
  std::shared_ptr<Scope> parent;
  std::map<std::string, std::list<scope_data>, std::less<>> members;
};

}
