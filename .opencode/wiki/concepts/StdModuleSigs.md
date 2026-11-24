---
title: Standard Module Signatures
tags: [std-modules, at-sigs, signatures, binary-format, constexpr]
sourceRefs: [std_modules/at_sigs.h, std_modules/math/math_sigs.cpp, std_modules/string/string_sigs.cpp, std_modules/io/io_sigs.cpp]
lastReviewed: 2026-11-20
---

# Standard Module Signatures (`.at.sigs`)

The `.at.sigs` section is a binary signature table embedded in each standard module's `.o` file. It describes the module's exported functions, types, and their signatures so that the AT compiler can resolve imported module functions at compile time.

## Binary Format (Version 3)

The section layout uses variable-length TLV (Tag-Length-Value) records:

```
[ATSigSection header]  [Type records]  [Decl records]  [string table]
```

### Section Header (16 bytes)

| Field | Type | Description |
|-------|------|-------------|
| magic | uint32 | `0x47535441` ("ATSG" little-endian) |
| version | uint32 | `3` |
| num_types | uint32 | Count of type records |
| num_decls | uint32 | Count of declaration records |

### Type Records

Each type record is a TLV record:

| Field | Type | Description |
|-------|------|-------------|
| tag | uint8 | `ATSG_T_*` type tag |
| payload_len | uint32 | Length of payload in bytes |
| payload | uint8[payload_len] | Type-specific payload |

#### Type Payload Encodings

| Tag | Name | Payload |
|-----|------|-----------|
| 0 | PRIM | `uint8 prim_kind` (`ATSG_P_INT`, `ATSG_P_DECIMAL`, `ATSG_P_STRING`, `ATSG_P_VOID`, `ATSG_P_PTR`, `ATSG_P_UNKNOWN`) |
| 1 | NAMED | `uint32 name_off` (string table offset) |
| 2 | FUNC | `uint8 param_count`, then `param_count × uint16 type_idx`, then `uint16 ret_type_idx` |
| 3 | PTR | `uint16 elem_type_idx` |
| 4 | REF | `uint16 elem_type_idx` |
| 5 | ARRAY | `uint16 elem_type_idx` |
| 6 | TMPL | `uint8 arg_count`, then `arg_count × uint16 type_idx` |

### Declaration Records

Each declaration record is also a TLV record:

| Kind | Name | Payload |
|------|------|-----------|
| 0 | FN | `uint8 param_count`, then `param_count × uint16 type_idx`, then `uint16 ret_type_idx` |
| 1 | CLASS | `uint32 num_bases`, then `num_bases × uint32 name_off`, `uint32 num_fields`, then `num_fields × (uint32 name_off + uint16 type_idx)`, `uint32 num_methods`, then `num_methods × uint16 decl_idx` |
| 2 | ALIAS | `uint16 target_type_idx` |
| 3 | META | Same as FN (uint8 param_count, params, uint16 ret_type_idx) |

### String Table

A sequence of null-terminated C strings. All `name_off` values are byte offsets into this table.

## C++ Builder Helpers (`at_sigs.h`)

The header `std_modules/at_sigs.h` provides `constexpr` builder functions for constructing signatures in a human-readable way:

| Builder | Purpose |
|---------|---------|
| `at_sigs_type_prim(kind)` | Primitive type (int, string, void, etc.) |
| `at_sigs_type_named(name_off)` | Named type (class/alias/template) |
| `at_sigs_type_func(ret_idx, params...)` | Function type (variadic type indices) |
| `at_sigs_type_ptr(elem_idx)` | Pointer type |
| `at_sigs_type_ref(elem_idx)` | Reference type |
| `at_sigs_type_array(elem_idx)` | Array type |
| `at_sigs_type_tmpl(name_off, type_indices...)` | Template type |
| `at_sigs_fn(name_off, ret_idx, type_indices...)` | Function declaration |
| `at_sigs_meta(name_off, ret_idx, type_indices...)` | Template meta function declaration |
| `at_sigs_class(name_off)` | Class declaration |
| `at_sigs_alias(name_off, target_type_idx)` | Type alias declaration |

## Usage Example

```cpp
#include "../at_sigs.h"

// String table
constexpr const char* math_strings[] = {
    "math_abs", "math_add", "int", "void",
};
constexpr uint32_t STR_math_abs = 0;
constexpr uint32_t STR_math_add = 9;
constexpr uint32_t STR_int = 16;
constexpr uint32_t STR_void = 20;

// Type indices
constexpr uint16_t TYP_int = 0;
constexpr uint16_t TYP_void = 1;

// Types
constexpr ATSigType math_types[] = {
    at_sigs_type_prim(ATSG_P_INT),   // TYP_int
    at_sigs_type_prim(ATSG_P_VOID),  // TYP_void
};

// Declarations
constexpr ATSigDecl math_decls[] = {
    at_sigs_fn(STR_math_abs, TYP_int, TYP_int),   // math_abs(int) -> int
    at_sigs_fn(STR_math_add, TYP_int, TYP_int, TYP_int),  // math_add(int,int) -> int
};

// Section placement
ATSG_SECTION __attribute__((used))
struct {
    ATSigSection header;
    ATSigType types[2];
    ATSigDecl decls[2];
    char strings[string_table_size() + 1];
} math_at_sigs_section = {
    {ATSG_MAGIC, ATSG_VERSION, 2, 2},
    {at_sigs_type_prim(ATSG_P_INT), at_sigs_type_prim(ATSG_P_VOID)},
    {at_sigs_fn(STR_math_abs, TYP_int, TYP_int),
     at_sigs_fn(STR_math_add, TYP_int, TYP_int, TYP_int)},
    {"math_abs\0" "math_add\0" "int\0" "void\0"},
};
```

## Modules Using This Approach

| Module | Types | Declarations |
|--------|-------|--------------|
| `math` | 2 (int, void) | 11 functions |
| `string` | 3 (int, string, void) | 8 functions |
| `io` | 3 (int, string, void) | 16 functions |

All three modules use the identical struct-based pattern: a single unnamed `ATSG_SECTION` struct containing header + types + decls + strings, placed in the `.at.sigs` ELF section.

## Changes in Version 3

- **Variable-length TLV records** instead of fixed-size structs — no more fixed-size limits (max 16 params, 64 types, etc.)
- **Removed parameter names** from declaration records (were never populated)
- **Removed variadic flag** from function declarations (to be re-introduced later if needed)
- **`uint32` for class counts** (num_bases, num_fields, num_methods) instead of `uint8` limits
- **`uint32` string offsets** instead of `uint16` (no 64KB string table limit)

## See Also

- [[entities/StdModules]] — math, string, and io module details
- [[entities/CodeGen]] — code generation including `.at.sigs` section emission
- `std_modules/at_sigs.h` — the header with all builder functions