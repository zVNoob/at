/* math_sigs.cpp — .at.sigs binary section for the math std module.
 *
 * Uses constexpr builders from at_sigs.h for human-readable signature
 * declarations. Everything is packed into one unnamed struct placed in
 * the .at.sigs ELF section via ATSG_SECTION.
 *
 * Compile together with math.cpp into math.o:
 *   g++ -std=c++20 -c math.cpp math_sigs.cpp -o math.o
 */

#include "../at_sigs.h"

#include <cstddef>
#include <cstdint>

// String table entries (order determines byte offsets).
constexpr const char* math_strings[] = {
    "math_abs",
    "math_add",
    "math_sub",
    "math_mul",
    "math_div",
    "math_mod",
    "math_min",
    "math_max",
    "math_pow2",
    "math_is_even",
    "math_is_odd",
    "int",
    "void",
};

// Compute byte offset of each string in the concatenated null-terminated table.
constexpr size_t string_offset(size_t index) noexcept {
    size_t off = 0;
    for (size_t i = 0; i < index; ++i) {
        off += __builtin_strlen(math_strings[i]) + 1;
    }
    return off;
}

constexpr uint16_t STR_math_abs = static_cast<uint16_t>(string_offset(0));
constexpr uint16_t STR_math_add = static_cast<uint16_t>(string_offset(1));
constexpr uint16_t STR_math_sub = static_cast<uint16_t>(string_offset(2));
constexpr uint16_t STR_math_mul = static_cast<uint16_t>(string_offset(3));
constexpr uint16_t STR_math_div = static_cast<uint16_t>(string_offset(4));
constexpr uint16_t STR_math_mod = static_cast<uint16_t>(string_offset(5));
constexpr uint16_t STR_math_min = static_cast<uint16_t>(string_offset(6));
constexpr uint16_t STR_math_max = static_cast<uint16_t>(string_offset(7));
constexpr uint16_t STR_math_pow2 = static_cast<uint16_t>(string_offset(8));
constexpr uint16_t STR_math_is_even = static_cast<uint16_t>(string_offset(9));
constexpr uint16_t STR_math_is_odd = static_cast<uint16_t>(string_offset(10));
constexpr uint16_t STR_int = static_cast<uint16_t>(string_offset(11));
constexpr uint16_t STR_void = static_cast<uint16_t>(string_offset(12));

// Total string table size (including final NUL from string literal concatenation).
constexpr size_t string_table_size() noexcept {
    size_t total = 0;
    for (const char* s : math_strings) {
        total += __builtin_strlen(s) + 1;
    }
    return total;
}

// Type table indices.
constexpr uint16_t TYP_int = 0;
constexpr uint16_t TYP_void = 1;

// Type table.
constexpr ATSigType math_types[] = {
    at_sigs_type_prim(ATSG_P_INT),   // TYP_int
    at_sigs_type_prim(ATSG_P_VOID),  // TYP_void
};

// Declaration table.
constexpr ATSigDecl math_decls[] = {
    // math_abs(int) -> int
    at_sigs_fn(STR_math_abs, TYP_int, {TYP_int}),
    // math_add(int, int) -> int
    at_sigs_fn(STR_math_add, TYP_int, {TYP_int, TYP_int}),
    // math_sub(int, int) -> int
    at_sigs_fn(STR_math_sub, TYP_int, {TYP_int, TYP_int}),
    // math_mul(int, int) -> int
    at_sigs_fn(STR_math_mul, TYP_int, {TYP_int, TYP_int}),
    // math_div(int, int) -> int
    at_sigs_fn(STR_math_div, TYP_int, {TYP_int, TYP_int}),
    // math_mod(int, int) -> int
    at_sigs_fn(STR_math_mod, TYP_int, {TYP_int, TYP_int}),
    // math_min(int, int) -> int
    at_sigs_fn(STR_math_min, TYP_int, {TYP_int, TYP_int}),
    // math_max(int, int) -> int
    at_sigs_fn(STR_math_max, TYP_int, {TYP_int, TYP_int}),
    // math_pow2(int) -> int
    at_sigs_fn(STR_math_pow2, TYP_int, {TYP_int}),
    // math_is_even(int) -> int
    at_sigs_fn(STR_math_is_even, TYP_int, {TYP_int}),
    // math_is_odd(int) -> int
    at_sigs_fn(STR_math_is_odd, TYP_int, {TYP_int}),
};

// Single unnamed struct placed in the .at.sigs section.
// Layout: header + types + decls + strings (all packed, contiguous).
ATSG_SECTION __attribute__((used))
struct {
    ATSigSection header;
    ATSigType types[2];
    ATSigDecl decls[11];
    char strings[string_table_size() + 1];
} math_at_sigs_section = {
    // Header
    {
        ATSG_MAGIC,
        ATSG_VERSION,
        2,   // num_types
        11,  // num_decls
    },
    // Type table
    {
        at_sigs_type_prim(ATSG_P_INT),
        at_sigs_type_prim(ATSG_P_VOID),
    },
    // Declaration table
    {
        at_sigs_fn(STR_math_abs, TYP_int, {TYP_int}),
        at_sigs_fn(STR_math_add, TYP_int, {TYP_int, TYP_int}),
        at_sigs_fn(STR_math_sub, TYP_int, {TYP_int, TYP_int}),
        at_sigs_fn(STR_math_mul, TYP_int, {TYP_int, TYP_int}),
        at_sigs_fn(STR_math_div, TYP_int, {TYP_int, TYP_int}),
        at_sigs_fn(STR_math_mod, TYP_int, {TYP_int, TYP_int}),
        at_sigs_fn(STR_math_min, TYP_int, {TYP_int, TYP_int}),
        at_sigs_fn(STR_math_max, TYP_int, {TYP_int, TYP_int}),
        at_sigs_fn(STR_math_pow2, TYP_int, {TYP_int}),
        at_sigs_fn(STR_math_is_even, TYP_int, {TYP_int}),
        at_sigs_fn(STR_math_is_odd, TYP_int, {TYP_int}),
    },
    // String table (concatenated null-terminated strings as one literal)
    {
        "math_abs\0"
        "math_add\0"
        "math_sub\0"
        "math_mul\0"
        "math_div\0"
        "math_mod\0"
        "math_min\0"
        "math_max\0"
        "math_pow2\0"
        "math_is_even\0"
        "math_is_odd\0"
        "int\0"
        "void\0"
    },
};
