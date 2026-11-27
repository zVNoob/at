#ifndef ATSG_SIGS_H
#define ATSG_SIGS_H

#include <cstdint>
#include <cstddef>
#include <initializer_list>
#include <utility>
#include <memory>
#include <vector>

#pragma pack(push, 1)

#define ATSG_MAGIC 0x47535441u
#define ATSG_VERSION 4u

#define ATSG_T_PRIM     0
#define ATSG_T_NAMED    1
#define ATSG_T_FUNC     2
#define ATSG_T_ARRAY    3
#define ATSG_T_PTR      4
#define ATSG_T_REF      5
#define ATSG_T_TMPL     6

#define ATSG_P_INT      0
#define ATSG_P_DECIMAL  1
#define ATSG_P_STRING   2
#define ATSG_P_VOID     3
#define ATSG_P_PTR      4
#define ATSG_P_UNKNOWN  5

#define ATSG_KIND_FN    0
#define ATSG_KIND_CLASS 1
#define ATSG_KIND_ALIAS 2
#define ATSG_KIND_META  3

#define ATSG_FLAG_CONST     1u   /* method/field is const/read-only */
#define ATSG_FLAG_PUBLIC    2u   /* public visibility */
#define ATSG_FLAG_PRIVATE   4u   /* private visibility */

#define ATSG_MAX_BASES      8
#define ATSG_MAX_FIELDS     32
#define ATSG_MAX_PARAMS     16
#define ATSG_MAX_TMPL       8

/*
 * Type table entry.  Describes a type (primitive, named, class, function
 * pointer, etc.).  For class types, parent_ids/parent_offsets describe the
 * inheritance chain and field_offset describes the in-memory layout.
 *
 * All fields and methods are stored uniformly in the field table: a method is
 * simply a field whose type is a function pointer.
 */
struct ATSigType {
    uint8_t  tag;              /* ATSG_T_* */
    uint8_t  prim_kind;        /* ATSG_P_* if Prim */
    uint32_t name_off;         /* string table offset */
    uint32_t size;             /* object size in bytes */
    uint32_t alignment;        /* alignment requirement */
    uint32_t parent_count;     /* number of base types */
    uint32_t parent_ids[ATSG_MAX_BASES];        /* base type indices */
    uint32_t parent_offsets[ATSG_MAX_BASES];  /* byte offset of each parent */
    uint32_t field_count;      /* number of fields (data + methods) */
    uint32_t field_name_off[ATSG_MAX_FIELDS];   /* string offsets */
    uint16_t field_type_idx[ATSG_MAX_FIELDS];   /* type table indices */
    uint32_t field_offset[ATSG_MAX_FIELDS];     /* byte offset within object */
    uint8_t  field_flags[ATSG_MAX_FIELDS];      /* const/! + visibility */

    /* For Func / Tmpl */
    uint8_t  param_count;      /* number of parameters (Func) or args (Tmpl) */
    uint16_t ret_type_idx;     /* return type index (Func) */
    uint16_t param_type_idx[ATSG_MAX_PARAMS];  /* parameter type indices */
    uint8_t  tmpl_arg_count;   /* template arg count (Tmpl) */
    uint16_t tmpl_arg_type_idx[ATSG_MAX_TMPL];  /* template arg type indices */

    /* For Ptr / Ref / Array */
    uint16_t elem_type_idx;    /* element type index */
};

/*
 * Decl table entry.  Describes a top-level declaration (function, class,
 * alias, meta/template function).
 *
 * Fn/Meta entries describe function signatures.  Class entries reference
 * a type table entry for layout (typeIdx).
 */
struct ATSigDecl {
    uint8_t  kind;               /* ATSG_KIND_* */
    uint32_t name_off;           /* string table offset */

    /* Fn / Meta fields */
    uint8_t  tmpl_param_count;   /* number of template parameters */
    uint32_t tmpl_param_name_off[ATSG_MAX_TMPL];  /* string offsets */
    uint8_t  flags;              /* const/! + visibility */
    uint8_t  param_count;        /* number of parameters */
    uint32_t param_name_off[ATSG_MAX_PARAMS];   /* string offsets */
    uint16_t param_type_idx[ATSG_MAX_PARAMS];   /* type table indices */
    uint16_t ret_type_idx;       /* type table index for return type */

    /* For Class: index into type table for layout */
    uint16_t typeIdx;            /* type table index */

    /* Alias field */
    uint16_t target_type_idx;    /* type table index */

    uint8_t  pad[18];
};

constexpr uint32_t at_sigs_str(uint32_t i) noexcept { return i; }
constexpr uint16_t at_sigs_tidx(uint16_t i) noexcept { return i; }

constexpr ATSigType at_sigs_type_prim(uint8_t kind) noexcept {
    ATSigType t{}; t.tag = ATSG_T_PRIM; t.prim_kind = kind; return t;
}
constexpr ATSigType at_sigs_type_named(uint32_t name_off) noexcept {
    ATSigType t{}; t.tag = ATSG_T_NAMED; t.name_off = name_off; return t;
}
template <typename... Params>
constexpr ATSigType at_sigs_type_func(uint16_t ret_idx, Params... params) noexcept {
    ATSigType t{}; t.tag = ATSG_T_FUNC; t.ret_type_idx = ret_idx;
    uint16_t arr[] = { static_cast<uint16_t>(params)... };
    t.param_count = static_cast<uint8_t>(sizeof...(Params));
    for (size_t i = 0; i < sizeof...(Params) && i < ATSG_MAX_PARAMS; ++i) t.param_type_idx[i] = arr[i];
    return t;
}
constexpr ATSigType at_sigs_type_ptr(uint16_t elem_idx) noexcept {
    ATSigType t{}; t.tag = ATSG_T_PTR; t.elem_type_idx = elem_idx; return t;
}
constexpr ATSigType at_sigs_type_ref(uint16_t elem_idx) noexcept {
    ATSigType t{}; t.tag = ATSG_T_REF; t.elem_type_idx = elem_idx; return t;
}
constexpr ATSigType at_sigs_type_array(uint16_t elem_idx) noexcept {
    ATSigType t{}; t.tag = ATSG_T_ARRAY; t.elem_type_idx = elem_idx; return t;
}
constexpr ATSigType at_sigs_type_tmpl(uint16_t name_off, auto&&... type_indices) noexcept {
    ATSigType t{}; t.tag = ATSG_T_TMPL; t.name_off = name_off;
    uint16_t arr[] = { static_cast<uint16_t>(type_indices)... };
    t.tmpl_arg_count = static_cast<uint8_t>(sizeof...(type_indices));
    for (size_t i = 0; i < sizeof...(type_indices) && i < ATSG_MAX_TMPL; ++i) t.tmpl_arg_type_idx[i] = arr[i];
    return t;
}

constexpr ATSigDecl at_sigs_fn(uint32_t name_off, uint16_t ret_idx, auto&&... type_indices) noexcept {
    ATSigDecl d{}; d.kind = ATSG_KIND_FN; d.name_off = name_off;
    d.ret_type_idx = ret_idx;
    uint16_t arr[] = { static_cast<uint16_t>(type_indices)... };
    d.param_count = static_cast<uint8_t>(sizeof...(type_indices));
    for (size_t i = 0; i < sizeof...(type_indices) && i < ATSG_MAX_PARAMS; ++i) d.param_type_idx[i] = arr[i];
    return d;
}

constexpr ATSigDecl at_sigs_meta(uint32_t name_off, uint16_t ret_idx, auto&&... type_indices) noexcept {
    ATSigDecl d{}; d.kind = ATSG_KIND_META; d.name_off = name_off;
    d.ret_type_idx = ret_idx;
    uint16_t arr[] = { static_cast<uint16_t>(type_indices)... };
    d.param_count = static_cast<uint8_t>(sizeof...(type_indices));
    for (size_t i = 0; i < sizeof...(type_indices) && i < ATSG_MAX_PARAMS; ++i) d.param_type_idx[i] = arr[i];
    return d;
}

constexpr ATSigDecl at_sigs_class(uint32_t name_off) noexcept {
    ATSigDecl d{}; d.kind = ATSG_KIND_CLASS; d.name_off = name_off; return d;
}

constexpr ATSigDecl at_sigs_alias(uint32_t name_off, uint16_t target_type_idx) noexcept {
    ATSigDecl d{}; d.kind = ATSG_KIND_ALIAS; d.name_off = name_off; d.target_type_idx = target_type_idx; return d;
}

#pragma pack(pop)
#endif
