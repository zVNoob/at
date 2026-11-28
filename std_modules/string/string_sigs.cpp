/* string_sigs.cpp — .at.sigs binary section for the string std module.
 *
 * Uses constexpr builders from at_sigs.h for human-readable signature
 * declarations. Everything is packed into one unnamed struct placed in
 * the .at.sigs ELF section via ATSG_SECTION.
 *
 * Compile together with string.cpp into string.o:
 *   g++ -std=c++20 -c string.cpp string_sigs.cpp -o string.o
 */

#include "../at_sigs.h"

#include <cstddef>
#include <cstdint>

// String table entries (order determines byte offsets).
constexpr const char* string_strings[] = {
    "string_length",
    "string_substring",
    "string_concat",
    "string_char_at",
    "string_find",
    "string_parse_int",
    "string_format_int",
    "string_compare",
    "int",
    "string",
    "void",
};

// Compute byte offset of each string in the concatenated null-terminated table.
constexpr size_t string_offset(size_t index) noexcept {
    size_t off = 0;
    for (size_t i = 0; i < index; ++i) {
        off += __builtin_strlen(string_strings[i]) + 1;
    }
    return off;
}

constexpr uint16_t STR_string_length = static_cast<uint16_t>(string_offset(0));
constexpr uint16_t STR_string_substring = static_cast<uint16_t>(string_offset(1));
constexpr uint16_t STR_string_concat = static_cast<uint16_t>(string_offset(2));
constexpr uint16_t STR_string_char_at = static_cast<uint16_t>(string_offset(3));
constexpr uint16_t STR_string_find = static_cast<uint16_t>(string_offset(4));
constexpr uint16_t STR_string_parse_int = static_cast<uint16_t>(string_offset(5));
constexpr uint16_t STR_string_format_int = static_cast<uint16_t>(string_offset(6));
constexpr uint16_t STR_string_compare = static_cast<uint16_t>(string_offset(7));
constexpr uint16_t STR_int = static_cast<uint16_t>(string_offset(8));
constexpr uint16_t STR_string = static_cast<uint16_t>(string_offset(9));
constexpr uint16_t STR_void = static_cast<uint16_t>(string_offset(10));

// Total string table size (including final NUL from string literal concatenation).
constexpr size_t string_table_size() noexcept {
    size_t total = 0;
    for (const char* s : string_strings) {
        total += __builtin_strlen(s) + 1;
    }
    return total;
}

// Type table indices.
constexpr uint16_t TYP_int = 0;
constexpr uint16_t TYP_string = 1;
constexpr uint16_t TYP_void = 2;

// Type table.
constexpr ATSigType string_types[] = {
    at_sigs_type_prim(ATSG_P_INT),    // TYP_int
    at_sigs_type_prim(ATSG_P_STRING), // TYP_string
    at_sigs_type_prim(ATSG_P_VOID),   // TYP_void
};

// Declaration table.
constexpr ATSigDecl string_decls[] = {
    // string_length(string) -> int
    at_sigs_fn(STR_string_length, TYP_int, TYP_string),
    // string_substring(string, int, int) -> string
    at_sigs_fn(STR_string_substring, TYP_string, TYP_string, TYP_int, TYP_int),
    // string_concat(string, string) -> string
    at_sigs_fn(STR_string_concat, TYP_string, TYP_string, TYP_string),
    // string_char_at(string, int) -> int
    at_sigs_fn(STR_string_char_at, TYP_int, TYP_string, TYP_int),
    // string_find(string, string) -> int
    at_sigs_fn(STR_string_find, TYP_int, TYP_string, TYP_string),
    // string_parse_int(string) -> int
    at_sigs_fn(STR_string_parse_int, TYP_int, TYP_string),
    // string_format_int(int) -> string
    at_sigs_fn(STR_string_format_int, TYP_string, TYP_int),
    // string_compare(string, string) -> int
    at_sigs_fn(STR_string_compare, TYP_int, TYP_string, TYP_string),
};

// Single unnamed struct placed in the .at.sigs section.
// Layout: header + types + decls + strings (all packed, contiguous).
ATSG_SECTION __attribute__((used))
struct {
    ATSigSection header;
    ATSigType types[3];
    ATSigDecl decls[8];
    char strings[string_table_size() + 1];
} string_at_sigs_section = {
    // Header
    {
        ATSG_MAGIC,
        ATSG_VERSION,
        3,   // num_types
        8,   // num_decls
    },
    // Type table
    {
        at_sigs_type_prim(ATSG_P_INT),
        at_sigs_type_prim(ATSG_P_STRING),
        at_sigs_type_prim(ATSG_P_VOID),
    },
    // Declaration table
    {
        at_sigs_fn(STR_string_length, TYP_int, TYP_string),
        at_sigs_fn(STR_string_substring, TYP_string, TYP_string, TYP_int, TYP_int),
        at_sigs_fn(STR_string_concat, TYP_string, TYP_string, TYP_string),
        at_sigs_fn(STR_string_char_at, TYP_int, TYP_string, TYP_int),
        at_sigs_fn(STR_string_find, TYP_int, TYP_string, TYP_string),
        at_sigs_fn(STR_string_parse_int, TYP_int, TYP_string),
        at_sigs_fn(STR_string_format_int, TYP_string, TYP_int),
        at_sigs_fn(STR_string_compare, TYP_int, TYP_string, TYP_string),
    },
    // String table (concatenated null-terminated strings as one literal)
    {
        "string_length\0"
        "string_substring\0"
        "string_concat\0"
        "string_char_at\0"
        "string_find\0"
        "string_parse_int\0"
        "string_format_int\0"
        "string_compare\0"
        "int\0"
        "string\0"
        "void\0"
    },
};
