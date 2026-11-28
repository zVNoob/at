/* io_sigs.cpp — .at.sigs binary section for the io std module.
 *
 * Uses constexpr builders from at_sigs.h for human-readable signature
 * declarations. Everything is packed into one unnamed struct placed in
 * the .at.sigs ELF section via ATSG_SECTION.
 *
 * Compile together with io.cpp into io.o:
 *   g++ -std=c++20 -c io.cpp io_sigs.cpp -o io.o
 */

#include "../at_sigs.h"

#include <cstddef>
#include <cstdint>

// String table entries (order determines byte offsets).
constexpr const char* io_strings[] = {
    "at_fopen",
    "at_fclose",
    "at_fread",
    "at_fwrite",
    "at_feof",
    "at_fflush",
    "at_ftell",
    "at_fseek",
    "at_fgets",
    "at_fputs",
    "at_fputc",
    "at_fgetc",
    "at_stdin",
    "at_stdout",
    "at_stderr",
    "at_io_stream_vtable_ptr",
    "int",
    "string",
    "void",
};

// Compute byte offset of each string in the concatenated null-terminated table.
constexpr size_t string_offset(size_t index) noexcept {
    return at_sigs_string_offset(io_strings, index);
}

constexpr uint16_t STR_at_fopen = static_cast<uint16_t>(string_offset(0));
constexpr uint16_t STR_at_fclose = static_cast<uint16_t>(string_offset(1));
constexpr uint16_t STR_at_fread = static_cast<uint16_t>(string_offset(2));
constexpr uint16_t STR_at_fwrite = static_cast<uint16_t>(string_offset(3));
constexpr uint16_t STR_at_feof = static_cast<uint16_t>(string_offset(4));
constexpr uint16_t STR_at_fflush = static_cast<uint16_t>(string_offset(5));
constexpr uint16_t STR_at_ftell = static_cast<uint16_t>(string_offset(6));
constexpr uint16_t STR_at_fseek = static_cast<uint16_t>(string_offset(7));
constexpr uint16_t STR_at_fgets = static_cast<uint16_t>(string_offset(8));
constexpr uint16_t STR_at_fputs = static_cast<uint16_t>(string_offset(9));
constexpr uint16_t STR_at_fputc = static_cast<uint16_t>(string_offset(10));
constexpr uint16_t STR_at_fgetc = static_cast<uint16_t>(string_offset(11));
constexpr uint16_t STR_at_stdin = static_cast<uint16_t>(string_offset(12));
constexpr uint16_t STR_at_stdout = static_cast<uint16_t>(string_offset(13));
constexpr uint16_t STR_at_stderr = static_cast<uint16_t>(string_offset(14));
constexpr uint16_t STR_at_io_stream_vtable_ptr = static_cast<uint16_t>(string_offset(15));
constexpr uint16_t STR_int = static_cast<uint16_t>(string_offset(16));
constexpr uint16_t STR_string = static_cast<uint16_t>(string_offset(17));
constexpr uint16_t STR_void = static_cast<uint16_t>(string_offset(18));

// Total string table size (including final NUL from string literal concatenation).
constexpr size_t string_table_size() noexcept {
    return at_sigs_string_table_size(io_strings, sizeof(io_strings)/sizeof(io_strings[0]));
}

// Compile-time string table built from io_strings.
constexpr char string_table[] = []() constexpr {
    char result[string_table_size()] = {};
    size_t off = 0;
    for (size_t i = 0; i < sizeof(io_strings)/sizeof(io_strings[0]); ++i) {
        const char* s = io_strings[i];
        while (*s) { result[off++] = *s++; }
        result[off++] = '\0';
    }
    return result;
}();

// Type table indices.
constexpr uint16_t TYP_int = 0;
constexpr uint16_t TYP_string = 1;
constexpr uint16_t TYP_void = 2;

// Type table.
constexpr ATSigType io_types[] = {
    at_sigs_type_prim(ATSG_P_INT),    // TYP_int
    at_sigs_type_prim(ATSG_P_STRING), // TYP_string
    at_sigs_type_prim(ATSG_P_VOID),   // TYP_void
};

// Declaration table.
constexpr ATSigDecl io_decls[] = {
    // at_fopen(string, string) -> int
    at_sigs_fn(STR_at_fopen, TYP_int, {TYP_string, TYP_string}),
    // at_fclose(int) -> int
    at_sigs_fn(STR_at_fclose, TYP_int, {TYP_int}),
    // at_fread(int, int) -> string
    at_sigs_fn(STR_at_fread, TYP_string, {TYP_int, TYP_int}),
    // at_fwrite(int, string) -> int
    at_sigs_fn(STR_at_fwrite, TYP_int, {TYP_int, TYP_string}),
    // at_feof(int) -> int
    at_sigs_fn(STR_at_feof, TYP_int, {TYP_int}),
    // at_fflush(int) -> int
    at_sigs_fn(STR_at_fflush, TYP_int, {TYP_int}),
    // at_ftell(int) -> int
    at_sigs_fn(STR_at_ftell, TYP_int, {TYP_int}),
    // at_fseek(int, int, int) -> int
    at_sigs_fn(STR_at_fseek, TYP_int, {TYP_int, TYP_int, TYP_int}),
    // at_fgets(int, int) -> string
    at_sigs_fn(STR_at_fgets, TYP_string, {TYP_int, TYP_int}),
    // at_fputs(string, int) -> int
    at_sigs_fn(STR_at_fputs, TYP_int, {TYP_string, TYP_int}),
    // at_fputc(int, int) -> int
    at_sigs_fn(STR_at_fputc, TYP_int, {TYP_int, TYP_int}),
    // at_fgetc(int) -> int
    at_sigs_fn(STR_at_fgetc, TYP_int, {TYP_int}),
    // at_stdin() -> int
    at_sigs_fn(STR_at_stdin, TYP_int, {}),
    // at_stdout() -> int
    at_sigs_fn(STR_at_stdout, TYP_int, {}),
    // at_stderr() -> int
    at_sigs_fn(STR_at_stderr, TYP_int, {}),
    // at_io_stream_vtable_ptr() -> int
    at_sigs_fn(STR_at_io_stream_vtable_ptr, TYP_int, {}),
};

// Single unnamed struct placed in the .at.sigs section.
// Layout: header + types + decls + strings (all packed, contiguous).
ATSG_SECTION __attribute__((used))
struct {
    ATSigSection header;
    ATSigType types[3];
    ATSigDecl decls[16];
    char strings[string_table_size() + 1];
} io_at_sigs_section = {
    // Header
    {
        ATSG_MAGIC,
        ATSG_VERSION,
        3,   // num_types
        16,  // num_decls
    },
    // Type table
    {
        at_sigs_type_prim(ATSG_P_INT),
        at_sigs_type_prim(ATSG_P_STRING),
        at_sigs_type_prim(ATSG_P_VOID),
    },
    // Declaration table
    {
        at_sigs_fn(STR_at_fopen, TYP_int, {TYP_string, TYP_string}),
        at_sigs_fn(STR_at_fclose, TYP_int, {TYP_int}),
        at_sigs_fn(STR_at_fread, TYP_string, {TYP_int, TYP_int}),
        at_sigs_fn(STR_at_fwrite, TYP_int, {TYP_int, TYP_string}),
        at_sigs_fn(STR_at_feof, TYP_int, {TYP_int}),
        at_sigs_fn(STR_at_fflush, TYP_int, {TYP_int}),
        at_sigs_fn(STR_at_ftell, TYP_int, {TYP_int}),
        at_sigs_fn(STR_at_fseek, TYP_int, {TYP_int, TYP_int, TYP_int}),
        at_sigs_fn(STR_at_fgets, TYP_string, {TYP_int, TYP_int}),
        at_sigs_fn(STR_at_fputs, TYP_int, {TYP_string, TYP_int}),
        at_sigs_fn(STR_at_fputc, TYP_int, {TYP_int, TYP_int}),
        at_sigs_fn(STR_at_fgetc, TYP_int, {TYP_int}),
        at_sigs_fn(STR_at_stdin, TYP_int, {}),
        at_sigs_fn(STR_at_stdout, TYP_int, {}),
        at_sigs_fn(STR_at_stderr, TYP_int, {}),
        at_sigs_fn(STR_at_io_stream_vtable_ptr, TYP_int, {}),
    },
    // String table (compiled from io_strings)
    string_table,
};
