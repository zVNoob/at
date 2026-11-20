#pragma once

#include <cstddef>
#include <cstdint>

// =============================================================================
// `at` language C/C++ Module ABI
// =============================================================================
//
// This header documents the calling conventions that C/C++ modules must follow
// to interoperate with the `at` compiler and runtime.  Any module can include
// this header and use these conventions.
//
// 1. SCALAR TYPES
//    - int / bool  -> int64_t (BigInt).  Passed as int64_t by value.
//    - decimal     -> double.  Passed as double by value.
//    - string      -> const char* (null-terminated).  Owned by caller unless
//                     documented otherwise.
//    - ptr         -> void*.
//
// 2. ARRAY ABI
//    Arrays are passed to C as TWO arguments: (data pointer, length).
//      - data : int64_t*  (pointer to the first element, i64 array)
//      - len  : size_t    (number of elements)
//    The data pointer is owned by the `at` runtime (malloc'd).  C modules
//    must NOT free it.  If a C module needs to retain the array, it must
//    copy the data.
//
//    Example C signature:
//        size_t my_func(const int64_t* data, size_t len);
//
//    Example `at` call site (codegen lowers `arr` to (ptr, len)):
//        r = my_func(arr);
//
//    For strings, the same (const char*, size_t) pattern applies when the
//    string contains embedded nulls; otherwise null-terminated const char*
//    is sufficient.
//
// 3. OPAQUE HANDLE ABI (for classes/objects)
//    C modules expose objects via opaque handles.  No destructor syntax is
//    needed in `at` — the C module owns cleanup via the vtable's destroy fn.
//
//      - Factory returns a void* (or typed struct*) handle.
//      - Methods take the handle as the first argument.
//      - A vtable struct of function pointers is provided for method dispatch.
//      - The vtable MUST include a `destroy(void* self)` entry.
//
//    When an `at` object holding an opaque handle is destroyed (refcount 0),
//    the runtime calls `vtable->destroy(handle)` automatically.  The C module
//    frees all resources it owns.  `at` never calls free() on the handle.
//
//    Example:
//      typedef struct { void (*destroy)(void*); /* ... */ } my_vtable;
//      void* my_create() { ...; return handle; }
//      const my_vtable* my_vtable_ptr() { return &VTABLE; }
//
//    See std_modules/io/io.h for a reference implementation.
//
// 4. SYMBOL NAMING
//    C modules should prefix exported symbols with the module name to avoid
//    collisions, e.g. `io_fopen`, `io_fread`.  The .at.sigs section stores
//    the logical (unprefixed) name.
//
// 5. SIGNATURES (.at.sigs)
//    Modules should embed a `.at.sigs` ELF section (binary ATSG format) so the
//    compiler can read type signatures for cross-module type checking.
//    Use BinSigWriter (src/sig_binary.h) to generate the section, or a
//    generator tool (see tools/gen_sig.py if available).
// =============================================================================

// Standard array helper macros for C modules.
#define AT_ARRAY_DATA(arr_ptr) ((const int64_t*)(arr_ptr))
#define AT_ARRAY_LEN(arr_ptr)  (0)  // length must be passed separately

// Type aliases for clarity.
typedef int64_t at_int;
typedef double  at_decimal;
