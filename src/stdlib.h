#pragma once

#include "interpreter.h"

#include <string>

namespace at {

// Register built-in functions into the global environment.
void registerStdLib(EnvironmentPtr env);

// Built-in function names available in the language.
// print(value)  - print value without trailing newline
// println(value) - print value with trailing newline
// len(value)    - length of string or array
// abs(x)        - absolute value (int or decimal)
// min(a, b)     - minimum of two values
// max(a, b)     - maximum of two values

} // namespace at
