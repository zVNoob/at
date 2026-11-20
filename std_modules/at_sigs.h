/*
 * at_sigs.h — C++ header for declaring .at.sigs section entries.
 *
 * Provides constexpr builder functions for constructing ATSigType and
 * ATSigDecl structs programmatically, plus string table helpers.
 *
 * Example usage:
 *
 *   #include "at_sigs.h"
 *
 *   // Build types and declarations with constexpr helpers:
 *   static const ATSigType math_types[] = {
 *       at_sigs_type_prim(ATSG_P_INT),    // idx 0: int
 *       at_sigs_type_prim(ATSG_P_VOID),   // idx 1: void
 *   };
 *
 *   static const ATSigDecl math_decls[] = {
 *       at_sigs_fn(0, 0, {0}),              // math_abs(int) -> int
 *       at_sigs_fn(1, 0, {0, 0}),           // math_add(int, int) -> int
 *   };
 *
 *   // Or use text format (easier to read):
 *   //   "math_abs(int) -> int"
 *
 * Compile with: g++ -std=c++20 -c module.cpp
 * The .at.sigs section is created by objcopy:
 *   objcopy --add-section .at.sigs=module_sigs.bin module.o
 */

#ifndef ATSG_SIGS_H
#define ATSG_SIGS_H

#include <cstdint>
#include <cstddef>
#include <initializer_list>

#pragma pack(push, 1)

/* Magic number: "ATSG" in little-endian */
#define ATSG_MAGIC 0x47535441u
#define ATSG_VERSION 2u

/* ---- Type tags ---- */
#define ATSG_T_PRIM     0   /* primitive */
#define ATSG_T_NAMED    1   /* named type (class/alias/template) */
#define ATSG_T_FUNC     2   /* function type */
#define ATSG_T_ARRAY    3   /* array type */
#define ATSG_T_PTR      4   /* pointer type */
#define ATSG_T_REF      5   /* reference type */
#define ATSG_T_TMPL     6   /* template type */

/* ---- Primitive kinds ---- */
#define ATSG_P_INT      0
#define ATSG_P_DECIMAL  1
#define ATSG_P_STRING   2
#define ATSG_P_VOID     3
#define ATSG_P_PTR      4
#define ATSG_P_UNKNOWN  5

/* ---- Declaration kinds ---- */
#define ATSG_KIND_FN    0
#define ATSG_KIND_CLASS 1
#define ATSG_KIND_ALIAS 2
#define ATSG_KIND_META  3

/* ---- Fixed limits ---- */
#define ATSG_MAX_TYPES    64
#define ATSG_MAX_DECLS    64
#define ATSG_MAX_PARAMS   16
#define ATSG_MAX_FIELDS   16
#define ATSG_MAX_METHODS  16
#define ATSG_MAX_BASES    8
#define ATSG_MAX_TMPL     8

/* =========================================================================
 * Fixed-size type encoding (32 bytes)
 * ========================================================================= */
struct ATSigType {
    uint8_t  tag;             /* ATSG_T_* */
    uint8_t  prim_kind;       /* ATSG_P_* (if tag == ATSG_T_PRIM) */
    uint16_t name_off;        /* string table offset (if NAMED/TMPL) */
    uint8_t  param_count;     /* (if tag == ATSG_T_FUNC) */
    uint16_t ret_type_idx;    /* type table index (if FUNC) */
    uint16_t param_type_idx[ATSG_MAX_PARAMS]; /* type table indices (if FUNC) */
    uint16_t elem_type_idx;   /* type table index (if ARRAY/PTR/REF) */
    uint8_t  tmpl_arg_count;  /* (if tag == ATSG_T_TMPL) */
    uint16_t tmpl_arg_type_idx[ATSG_MAX_TMPL]; /* type table indices (if TMPL) */
    uint8_t  pad[3];
};  /* sizeof is platform-dependent; used for documentation only */

// Note: ATSigType and ATSigDecl sizes depend on the field layout.
// The .at.sigs binary format is defined by the raw byte layout in the
// .at.sigs section, not by sizeof() of these structs.

/* ---- Fixed-size declaration encoding (128 bytes) ======================== */
struct ATSigDecl {
    uint8_t  kind;              /* ATSG_KIND_* */
    uint16_t name_off;          /* string table offset */
    uint8_t  tmpl_param_count;  /* number of template parameters */
    uint16_t tmpl_param_name_off[ATSG_MAX_TMPL]; /* string offsets */

    /* Fn / Meta fields */
    uint8_t  is_meta;           /* 1 if this is a meta (template) function */
    uint8_t  param_count;       /* number of parameters */
    uint16_t param_name_off[ATSG_MAX_PARAMS];  /* string offsets */
    uint16_t param_type_idx[ATSG_MAX_PARAMS];  /* type table indices */
    uint16_t ret_type_idx;      /* type table index for return type */

    /* Class fields */
    uint8_t  num_bases;
    uint16_t base_name_off[ATSG_MAX_BASES];    /* string offsets */
    uint16_t num_fields;
    uint16_t field_name_off[ATSG_MAX_FIELDS];  /* string offsets */
    uint16_t field_type_idx[ATSG_MAX_FIELDS];  /* type table indices */
    uint16_t num_methods;
    uint16_t method_decl_idx[ATSG_MAX_METHODS]; /* decl table indices */

    /* Alias field */
    uint16_t target_type_idx;   /* type table index */

    uint8_t  pad[18];
};  /* sizeof is platform-dependent; used for documentation only */

// Note: ATSigDecl size depends on the field layout.

/* =========================================================================
 * Section header (16 bytes)
 * ========================================================================= */
struct ATSigSection {
    uint32_t magic;       /* ATSG_MAGIC */
    uint32_t version;     /* ATSG_VERSION */
    uint32_t num_types;   /* number of ATSigType records */
    uint32_t num_decls;   /* number of ATSigDecl records */
    /* followed immediately by: num_types * ATSigType, then num_decls * ATSigDecl,
     * then a null-terminated string table */
};

/* =========================================================================
 * Section placement macro
 * ========================================================================= */
#if defined(__GNUC__) || defined(__clang__)
#define ATSG_SECTION __attribute__((section(".at.sigs")))
#else
#define ATSG_SECTION
#endif

/* =========================================================================
 * constexpr builder functions (C++20)
 * ========================================================================= */

/* String table offset helper */
constexpr uint16_t at_sigs_str(uint16_t i) noexcept { return i; }

/* Type table index helper */
constexpr uint16_t at_sigs_tidx(uint16_t i) noexcept { return i; }

/* ---- Type builders ---- */

constexpr ATSigType at_sigs_type_prim(uint8_t kind) noexcept {
    ATSigType t{};
    t.tag = ATSG_T_PRIM;
    t.prim_kind = kind;
    t.name_off = 0;
    t.param_count = 0;
    t.ret_type_idx = 0;
    t.elem_type_idx = 0;
    t.tmpl_arg_count = 0;
    return t;
}

constexpr ATSigType at_sigs_type_named(uint16_t name_off) noexcept {
    ATSigType t{};
    t.tag = ATSG_T_NAMED;
    t.prim_kind = 0;
    t.name_off = name_off;
    t.param_count = 0;
    t.ret_type_idx = 0;
    t.elem_type_idx = 0;
    t.tmpl_arg_count = 0;
    return t;
}

constexpr ATSigType at_sigs_type_func(uint16_t ret_idx,
                                       std::initializer_list<uint16_t> params) noexcept {
    ATSigType t{};
    t.tag = ATSG_T_FUNC;
    t.prim_kind = 0;
    t.name_off = 0;
    t.param_count = static_cast<uint8_t>(params.size());
    t.ret_type_idx = ret_idx;
    size_t i = 0;
    for (auto p : params) {
        if (i < ATSG_MAX_PARAMS) t.param_type_idx[i++] = p;
    }
    t.elem_type_idx = 0;
    t.tmpl_arg_count = 0;
    return t;
}

constexpr ATSigType at_sigs_type_ptr(uint16_t elem_idx) noexcept {
    ATSigType t{};
    t.tag = ATSG_T_PTR;
    t.prim_kind = 0;
    t.name_off = 0;
    t.param_count = 0;
    t.ret_type_idx = 0;
    t.elem_type_idx = elem_idx;
    t.tmpl_arg_count = 0;
    return t;
}

constexpr ATSigType at_sigs_type_ref(uint16_t elem_idx) noexcept {
    ATSigType t{};
    t.tag = ATSG_T_REF;
    t.prim_kind = 0;
    t.name_off = 0;
    t.param_count = 0;
    t.ret_type_idx = 0;
    t.elem_type_idx = elem_idx;
    t.tmpl_arg_count = 0;
    return t;
}

constexpr ATSigType at_sigs_type_array(uint16_t elem_idx) noexcept {
    ATSigType t{};
    t.tag = ATSG_T_ARRAY;
    t.prim_kind = 0;
    t.name_off = 0;
    t.param_count = 0;
    t.ret_type_idx = 0;
    t.elem_type_idx = elem_idx;
    t.tmpl_arg_count = 0;
    return t;
}

/* ---- Declaration builders ---- */

constexpr ATSigDecl at_sigs_fn(uint16_t name_off, uint16_t ret_idx,
                                std::initializer_list<uint16_t> params) noexcept {
    ATSigDecl d{};
    d.kind = ATSG_KIND_FN;
    d.name_off = name_off;
    d.tmpl_param_count = 0;
    d.is_meta = 0;
    d.param_count = static_cast<uint8_t>(params.size());
    size_t i = 0;
    for (auto p : params) {
        if (i < ATSG_MAX_PARAMS) d.param_type_idx[i++] = p;
    }
    d.ret_type_idx = ret_idx;
    return d;
}

constexpr ATSigDecl at_sigs_meta(uint16_t name_off, uint16_t ret_idx,
                                  std::initializer_list<uint16_t> params) noexcept {
    ATSigDecl d = at_sigs_fn(name_off, ret_idx, params);
    d.kind = ATSG_KIND_META;
    d.is_meta = 1;
    return d;
}

constexpr ATSigDecl at_sigs_class(uint16_t name_off) noexcept {
    ATSigDecl d{};
    d.kind = ATSG_KIND_CLASS;
    d.name_off = name_off;
    return d;
}

constexpr ATSigDecl at_sigs_alias(uint16_t name_off, uint16_t target_type_idx) noexcept {
    ATSigDecl d{};
    d.kind = ATSG_KIND_ALIAS;
    d.name_off = name_off;
    d.target_type_idx = target_type_idx;
    return d;
}

#pragma pack(pop)

#endif /* ATSG_SIGS_H */
