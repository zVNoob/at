---
title: Standard Modules
tags: [std-modules, math, string, io, stdlib]
sourceRefs: [std_modules/at_sigs.h, std_modules/math/, std_modules/string/, std_modules/io/, src/std_modules.h, src/std_modules.cpp]
lastReviewed: 2026-11-19
---

# Standard Modules

The AT language provides three standard modules: `math`, `string`, and `io`. Each module consists of:

- A C++ header (`.h`) declaring the C ABI functions
- A C++ implementation (`.cpp`) with the function bodies
- A signature file (`*_sigs.cpp`) declaring `.at.sigs` section entries using `at_sigs.h`
- A Makefile for building `.o` or `.so` shared libraries

## Module: math

**Files**: `std_modules/math/math.h`, `math.cpp`, `math_sigs.cpp`, `Makefile`

Integer math operations. All args and returns are `at_int` (int64_t).

| Function | Signature |
|----------|-----------|
| `math_abs` | `(int) -> int` |
| `math_add` | `(int, int) -> int` |
| `math_sub` | `(int, int) -> int` |
| `math_mul` | `(int, int) -> int` |
| `math_div` | `(int, int) -> int` |
| `math_mod` | `(int, int) -> int` |
| `math_min` | `(int, int) -> int` |
| `math_max` | `(int, int) -> int` |
| `math_pow2` | `(int) -> int` |
| `math_is_even` | `(int) -> int` |
| `math_is_odd` | `(int) -> int` |

The `.at.sigs` section declares 2 types (`int`, `void`) and 11 function declarations using `at_sigs_type_prim()` and `at_sigs_fn()`.

## Module: string

**Files**: `std_modules/string/string.h`, `string.cpp`, `string_sigs.cpp`, `Makefile`

String operations. String args/returns are `const char*` (int64_t); integer args/returns are `int64_t`.

| Function | Signature |
|----------|-----------|
| `string_length` | `(string) -> int` |
| `string_substring` | `(string, int, int) -> string` |
| `string_concat` | `(string, string) -> string` |
| `string_char_at` | `(string, int) -> int` |
| `string_find` | `(string, string) -> int` |
| `string_parse_int` | `(string) -> int` |
| `string_format_int` | `(int) -> string` |
| `string_compare` | `(string, string) -> int` |

The `.at.sigs` section declares 3 types (`int`, `string`, `void`) and 8 function declarations.

## Module: io

**Files**: `std_modules/io/io.h`, `io.cpp`, `io_sigs.cpp`, `Makefile`

I/O module with stream support. Exports an opaque-handle + method-table ABI so `at` can treat streams as classes.

Key types:
- `at_io_stream` — opaque handle with vtable pointer, FILE* backing, and name buffer
- `at_io_stream_vtable` — method table with destroy, name, read, write, close, flush

Key functions:
- `at_io_open(path, mode)` — open a file stream
- `at_io_stream_vtable_ptr()` — access the shared vtable instance
- `at_io_stdin_stream()`, `at_io_stdout_stream()`, `at_io_stderr_stream()` — standard streams

The `.at.sigs` section declares 3 types (`int`, `string`, `void`) and 16 function declarations.

## Build

Each module's Makefile supports two targets:
- `all` — builds `module.o` (relocatable object for linking)
- `shared` — builds `libmodule.so` (shared library with `-fPIC -shared`)

Build steps:
1. Compile `module.cpp` → `module_tmp.o`
2. Compile `module_sigs.cpp` → `module_sigs_tmp.o`
3. Link with `ld -r` → `module.o`
4. Clean up temp files

## Linking

Standard modules are linked into the main program at compile time. The `CodeGen::compile()` function collects imported `.at` files, compiles them to `.o` in `/tmp/`, and links them with the main `.o`. Imported modules' functions are resolved via the `.at.sigs` section entries.

## See Also

- [[concepts/StdModuleSigs]] — `.at.sigs` binary format and `at_sigs.h` builders
- [[entities/CodeGen]] — LLVM-based code generation including import linking
