#pragma once

#include "ast.h"
#include <string>

namespace at {
AstPtr parseSource(const std::string& source);
}
