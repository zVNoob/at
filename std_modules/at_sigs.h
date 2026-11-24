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
#define ATSG_VERSION 3u

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

struct ATSigType {
    uint8_t  tag;
    uint8_t  prim_kind;
    uint32_t name_off;
    uint8_t  param_count;
    uint16_t ret_type_idx;
    uint16_t param_type_idx[255];
    uint16_t elem_type_idx;
    uint8_t  tmpl_arg_count;
    uint16_t tmpl_arg_type_idx[255];
};

struct ATSigDecl {
    uint8_t  kind;
    uint32_t name_off;
    uint8_t  tmpl_param_count;
    uint32_t tmpl_param_name_off[255];
    uint8_t  is_meta;
    uint8_t  param_count;
    uint32_t param_name_off[255];
    uint16_t param_type_idx[255];
    uint16_t ret_type_idx;
    uint32_t num_bases;
    uint32_t base_name_off[255];
    uint32_t num_fields;
    uint32_t field_name_off[255];
    uint16_t field_type_idx[255];
    uint32_t num_methods;
    uint16_t method_decl_idx[255];
    uint16_t target_type_idx;
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
    for (size_t i = 0; i < sizeof...(Params) && i < 255; ++i) t.param_type_idx[i] = arr[i];
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
    for (size_t i = 0; i < sizeof...(type_indices) && i < 255; ++i) t.tmpl_arg_type_idx[i] = arr[i];
    return t;
}

constexpr ATSigDecl at_sigs_fn(uint32_t name_off, uint16_t ret_idx, auto&&... type_indices) noexcept {
    ATSigDecl d{}; d.kind = ATSG_KIND_FN; d.name_off = name_off; d.is_meta = 0;
    d.ret_type_idx = ret_idx;
    uint16_t arr[] = { static_cast<uint16_t>(type_indices)... };
    d.param_count = static_cast<uint8_t>(sizeof...(type_indices));
    for (size_t i = 0; i < sizeof...(type_indices) && i < 255; ++i) d.param_type_idx[i] = arr[i];
    return d;
}
constexpr ATSigDecl at_sigs_meta(uint32_t name_off, uint16_t ret_idx, auto&&... type_indices) noexcept {
    ATSigDecl d{}; d.kind = ATSG_KIND_META; d.name_off = name_off; d.is_meta = 1;
    d.ret_type_idx = ret_idx;
    uint16_t arr[] = { static_cast<uint16_t>(type_indices)... };
    d.param_count = static_cast<uint8_t>(sizeof...(type_indices));
    for (size_t i = 0; i < sizeof...(type_indices) && i < 255; ++i) d.param_type_idx[i] = arr[i];
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
