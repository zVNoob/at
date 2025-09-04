#include "scope.hpp"

namespace scope {
std::shared_ptr<Scope> global_scope = std::make_shared<Scope>();
}
