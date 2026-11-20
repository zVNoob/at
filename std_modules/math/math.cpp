#include "math.h"

#include <cstdlib>

at_int math_abs(at_int x) { return x < 0 ? -x : x; }
at_int math_add(at_int a, at_int b) { return a + b; }
at_int math_sub(at_int a, at_int b) { return a - b; }
at_int math_mul(at_int a, at_int b) { return a * b; }
at_int math_div(at_int a, at_int b) { return b == 0 ? 0 : a / b; }
at_int math_mod(at_int a, at_int b) { return b == 0 ? 0 : a % b; }
at_int math_min(at_int a, at_int b) { return a < b ? a : b; }
at_int math_max(at_int a, at_int b) { return a > b ? a : b; }
at_int math_pow2(at_int x) { return x * x; }
at_int math_is_even(at_int x) { return (x % 2 == 0) ? 1 : 0; }
at_int math_is_odd(at_int x) { return (x % 2 != 0) ? 1 : 0; }
