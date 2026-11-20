#pragma once

#include <cstdint>
#include <cstddef>

#include "../abi.h"

#ifdef __cplusplus
extern "C" {
#endif

/* String operations. All string args/returns are const char* (int64_t).
 * Integer args/returns are int64_t.
 */
int64_t string_length(const char* s);
const char* string_substring(const char* s, int64_t start, int64_t len);
const char* string_concat(const char* s1, const char* s2);
int64_t string_char_at(const char* s, int64_t i);
int64_t string_find(const char* s, const char* sub);
int64_t string_parse_int(const char* s);
const char* string_format_int(int64_t i);
int64_t string_compare(const char* s1, const char* s2);

#ifdef __cplusplus
}
#endif
