#pragma once

#include <memory>

namespace type { class Type; }
namespace object {
class Object {
public:
  std::shared_ptr<type::Type> type;
  virtual ~Object() = default;
};
}
