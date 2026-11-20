#include "string.h"

#include <cstdlib>
#include <cstring>
#include <cstdio>

int64_t string_length(const char* s) {
    if (!s) return 0;
    return static_cast<int64_t>(strlen(s));
}

const char* string_substring(const char* s, int64_t start, int64_t len) {
    if (!s) return "";
    size_t slen = strlen(s);
    if (start < 0) start = 0;
    if (start >= static_cast<int64_t>(slen)) return "";
    if (len < 0) len = static_cast<int64_t>(slen) - start;
    size_t end = static_cast<size_t>(start) + static_cast<size_t>(len);
    if (end > slen) end = slen;
    size_t clen = end - static_cast<size_t>(start);
    char* result = static_cast<char*>(malloc(clen + 1));
    if (!result) return "";
    memcpy(result, s + start, clen);
    result[clen] = '\0';
    return result;
}

const char* string_concat(const char* s1, const char* s2) {
    if (!s1) s1 = "";
    if (!s2) s2 = "";
    size_t l1 = strlen(s1);
    size_t l2 = strlen(s2);
    char* result = static_cast<char*>(malloc(l1 + l2 + 1));
    if (!result) return "";
    memcpy(result, s1, l1);
    memcpy(result + l1, s2, l2);
    result[l1 + l2] = '\0';
    return result;
}

int64_t string_char_at(const char* s, int64_t i) {
    if (!s) return -1;
    size_t slen = strlen(s);
    if (i < 0 || i >= static_cast<int64_t>(slen)) return -1;
    return static_cast<int64_t>(static_cast<unsigned char>(s[i]));
}

int64_t string_find(const char* s, const char* sub) {
    if (!s || !sub) return -1;
    const char* found = strstr(s, sub);
    if (!found) return -1;
    return static_cast<int64_t>(found - s);
}

int64_t string_parse_int(const char* s) {
    if (!s) return 0;
    return static_cast<int64_t>(strtoll(s, nullptr, 10));
}

const char* string_format_int(int64_t i) {
    char* result = static_cast<char*>(malloc(32));
    if (!result) return "";
    snprintf(result, 32, "%lld", static_cast<long long>(i));
    return result;
}

int64_t string_compare(const char* s1, const char* s2) {
    if (!s1) s1 = "";
    if (!s2) s2 = "";
    return static_cast<int64_t>(strcmp(s1, s2));
}
