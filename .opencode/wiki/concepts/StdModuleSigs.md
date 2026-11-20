---
title: Standard Module Signatures
tags: [std-modules, at-sigs, signatures, binary-format, constexpr]
sourceRefs: [std_modules/at_sigs.h, std_modules/math/math_sigs.cpp, std_modules/string/string_sigs.cpp, std_modules/io/io_sigs.cpp]
lastReviewed: 2026-11-19
---

# Standard Module Signatures (`.at.sigs`)

The `.at.sigs` section is a binary signature table embedded in each standard module's `.o` file. It describes the module's exported functions, types, and their signatures so that the AT compiler can resolve imported module functions at compile time.

## Binary Format (Version 2)

The section layout is:

```
[ATSigSection header]  [ATSigType records]  [ATSigDecl records]  [string table]
```

### Section Header (16 bytes, packed)

| Field | Type | Description |
|-------|------|-------------|
| magic | uint32 | `0x47535441` ("ATSG" little-endian) |
| version | uint32 | `2` |
| num_types | uint32 | Count of type records |
| num_decls | uint32 | Count of declaration records |

### Type Record (ATSigType)

Each type record encodes an AT type:
- `tag` (uint8): `ATSG_T_PRIM`, `ATSG_T_NAMED`, `ATSG_T_FUNC`, `ATSG_T_ARRAY`, `ATSG_T_PTR`, `ATSG_T_REF`, `ATSG_T_TMPL`
- `prim_kind` (uint8): `ATSG_P_INT`, `ATSG_P_DECIMAL`, `ATSG_P_STRING`, `ATSG_P_VOID`, `ATSG_P_PTR`, `ATSG_P_UNKNOWN`
- `name_off` (uint16): string table offset (for named/template types)
- `param_count` (uint8): parameter count (for function types)
- `ret_type_idx` (uint16): return type table index (for function types)
- `param_type_idx[16]` (uint16): parameter type indices
- `elem_type_idx` (uint16): element type index (for array/ptr/ref)
- `tmpl_arg_count` (uint8): template arg count
- `tmpl_arg_type_idx[8]` (uint16): template arg type indices

### Declaration Record (ATSigDecl)

Each declaration record encodes a function, class, alias, or meta function:
- `kind` (uint8): `ATSG_KIND_FN`, `ATSG_KIND_CLASS`, `ATSG_KIND_ALIAS`, `ATSG_KIND_META`
- `name_off` (uint16): string table offset
- `tmpl_param_count` (uint8): template parameter count
- `is_meta` (uint8): 1 if template meta function
- `param_count` (uint8): parameter count
- `param_name_off[16]` (uint16): parameter name offsets
- `param_type_idx[16]` (uint16): parameter type indices
- `ret_type_idx` (uint16): return type index
- Class fields: bases, fields, methods
- Alias field: `target_type_idx` (uint16)

## C++ Builder Helpers (`at_sigs.h`)

The header `std_modules/at_sigs.h` provides `constexpr` builder functions for constructing signatures in a human-readable way:

| Builder | Purpose |
|---------|---------|
| `at_sigs_type_prim(kind)` | Primitive type (int, string, void, etc.) |
| `at_sigs_type_named(name_off)` | Named type (class/alias/template) |
| `at_sigs_type_func(ret_idx, {params})` | Function type |
| `at_sigs_type_ptr(elem_idx)` | Pointer type |
| `at_sigs_type_ref(elem_idx)` | Reference type |
| `at_sigs_type_array(elem_idx)` | Array type |
| `at_sigs_fn(name_off, ret_idx, {params})` | Function declaration |
| `at_sigs_meta(name_off, ret_idx, {params})` | Template meta function declaration |
| `at_sigs_class(name_off)` | Class declaration |
| `at_sigs_alias(name_off, target_type_idx)` | Type alias declaration |

## Usage Example

```cpp
#include "../at_sigs.h"

// String table
constexpr const char* math_strings[] = {
    "math_abs", "math_add", "int", "void",
};
constexpr uint16_t STR_math_abs = string_offset(0);
constexpr uint16_t STR_int = string_offset(2);

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
    at_sigs_fn(STR_math_abs, TYP_int, {TYP_int}),  // math_abs(int) -> int
    at_sigs_fn(STR_math_add, TYP_int, {TYP_int, TYP_int}),  // math_add(int,int) -> int
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
    {at_sigs_fn(STR_math_abs, TYP_int, {TYP_int}),
     at_sigs_fn(STR_math_add, TYP_int, {TYP_int, TYP_int})},
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

## See Also

- [[entities/StdModules]] — math, string, and io module details
- [[entities/CodeGen]] — code generation including `.at.sigs` section emission
- `std_modules/at_sigs.h` — the header with all builder functions
