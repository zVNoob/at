#pragma once

#include <cstdint>
#include <cstddef>

#include "../abi.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Integer math operations. All args and return are int64_t (at_int). */
at_int math_abs(at_int x);
at_int math_add(at_int a, at_int b);
at_int math_sub(at_int a, at_int b);
at_int math_mul(at_int a, at_int b);
at_int math_div(at_int a, at_int b);
at_int math_mod(at_int a, at_int b);
at_int math_min(at_int a, at_int b);
at_int math_max(at_int a, at_int b);
at_int math_pow2(at_int x);
at_int math_is_even(at_int x);
at_int math_is_odd(at_int x);

#ifdef __cplusplus
}
#endif
