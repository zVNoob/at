#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "sig_format.h"

namespace at {

// Binary signature format for .at.sigs section.
// Version 4 layout (variable-length TLV records):
//
//   Header (16 bytes):
//     magic: "ATSG" (4 bytes)
//     version: uint32 (=4) (4 bytes)
//     num_types: uint32 (4 bytes)
//     num_decls: uint32 (4 bytes)
//
//   Type records (num_types entries):
//     uint8  tag            ATSG_T_*
//     uint32 payload_len    length of following payload in bytes
//     uint8  payload[payload_len]
//
//   Decl records (num_decls entries):
//     uint8  kind           ATSG_KIND_*
//     uint16 name_len       length of name bytes
//     uint8  name[name_len]
//     uint32 payload_len    length of following payload in bytes
//     uint8  payload[payload_len]
//
//   String table: null-terminated C strings; all name_off values are byte
//   offsets into this table.
//
// Type payload encodings:
//   PRIM:   uint8 prim_kind                          (ATSG_P_*)
//   NAMED:  uint32 name_off
//           uint32 size
//           uint32 alignment
//           uint32 parent_count
//           parent_count × uint32 parent_id
//           parent_count × uint32 parent_offset
//           uint32 field_count
//           field_count × (uint32 name_off + uint16 type_idx + uint32 offset + uint8 flags)
//   FUNC:   uint8 param_count
//           param_count × uint16 type_idx
//           uint16 ret_type_idx
//   PTR/REF/ARRAY: uint16 elem_type_idx
//   TMPL:   uint8 arg_count
//           arg_count × uint16 type_idx
//
// Decl payload encodings:
//   FN/META:   uint8 flags              (const/! + visibility)
//              uint8 tmpl_param_count
//              tmpl_param_count × (uint16 name_len + name bytes)
//              uint8 param_count
//              param_count × (uint16 name_len + name bytes + uint16 type_idx)
//              uint16 ret_type_idx
//   CLASS:   uint16 type_idx            // index into type table for layout
//   ALIAS:   uint16 target_type_idx     // index into type table

struct BinSigType {
    enum Tag : uint8_t { Prim=0, Named=1, Func=2, Array=3, Ptr=4, Ref=5, Tmpl=6 };
    Tag tag = Prim;
    uint8_t primKind = 0; // see PrimKind
    std::string name;
    // Class layout (used when tag == Named and it's a class)
    uint32_t size = 0;
    uint32_t alignment = 0;
    uint32_t parent_count = 0;
    std::vector<uint32_t> parent_ids;        // base type indices
    std::vector<uint32_t> parent_offsets;    // byte offset of each parent
    // Fields (uniform: data fields and method fields)
    uint32_t field_count = 0;
    struct Field {
        std::string name;
        uint16_t type_idx;
        uint32_t offset;
        uint8_t flags; // ATSG_FLAG_CONST, ATSG_FLAG_PUBLIC, ATSG_FLAG_PRIVATE
    };
    std::vector<Field> fields;
    // For Func / Tmpl / Ptr / Ref / Array
    std::vector<std::shared_ptr<BinSigType>> params; // for Func (params) or Tmpl (args)
    std::shared_ptr<BinSigType> returnType; // for Func
    std::shared_ptr<BinSigType> elementType; // for Array, Ptr, Ref

    enum PrimKind : uint8_t { PInt=0, PDecimal=1, PString=2, PVoid=3, PPtr=4, PUnknown=5 };

    static BinSigType prim(uint8_t k) { BinSigType t; t.tag = Prim; t.primKind = k; return t; }
    static BinSigType named(const std::string& n) { BinSigType t; t.tag = Named; t.name = n; return t; }
    static BinSigType wrap(Tag tag, const BinSigType& elem) {
        BinSigType t; t.tag = tag; t.elementType = std::make_shared<BinSigType>(elem); return t;
    }
};

struct BinSigDecl {
    enum Kind : uint8_t { Fn=0, Class=1, Alias=2, Meta=3 };
    Kind kind = Fn;
    std::string name;
    std::vector<std::string> templateParams;
    uint8_t flags = 0; // ATSG_FLAG_CONST, ATSG_FLAG_PUBLIC, ATSG_FLAG_PRIVATE
    // For Fn/Meta:
    std::vector<std::pair<std::string, std::shared_ptr<BinSigType>>> params; // (name, type)
    BinSigType returnType;
    // For Class:
    uint16_t typeIdx = 0; // index into type table for layout
    // For Alias:
    BinSigType aliasTarget;
};

// Binary writer for version 4 TLV format
class BinSigWriter {
public:
    std::vector<uint8_t> write(const std::vector<BinSigDecl>& decls,
                               const std::vector<BinSigType>& types = {});

private:
    std::vector<uint8_t> buf_;
    std::map<const BinSigType*, uint16_t> typeIndex_;
    std::map<std::string, const BinSigType*> canonicalTypes_;
    const std::vector<BinSigType>* extraTypes_ = nullptr;

    void writeU8(uint8_t v) { buf_.push_back(v); }
    void writeU16(uint16_t v) { buf_.push_back(v & 0xFF); buf_.push_back((v >> 8) & 0xFF); }
    void writeU32(uint32_t v) { for (int i=0;i<4;i++) buf_.push_back((v>>(i*8))&0xFF); }
    uint32_t writeStr(const std::string& s);
    void writeType(const BinSigType& t);
    void writeDecl(const BinSigDecl& d);
};

// Binary reader for version 4 TLV format
class BinSigReader {
public:
    bool read(const std::vector<uint8_t>& data);
    const std::vector<BinSigDecl>& decls() const { return decls_; }

private:
    std::vector<BinSigDecl> decls_;
};

// Conversion to the human-readable text format (for debugging / dumps).
// Uses SigType/SigDecl from sig_format.h.
std::string binDeclToString(const BinSigDecl& d);

} // namespace at
