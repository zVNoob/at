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
// Version 3 layout (variable-length TLV records):
//
//   Header (16 bytes):
//     magic: "ATSG" (4 bytes)
//     version: uint32 (=3) (4 bytes)
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
//     uint32 payload_len    length of following payload in bytes
//     uint8  payload[payload_len]
//
//   String table: null-terminated C strings; all name_off values are byte
//   offsets into this table.
//
// Type payload encodings:
//   PRIM:   uint8 prim_kind                          (ATSG_P_*)
//   NAMED:  uint32 name_off
//   FUNC:   uint8 param_count
//           param_count × uint16 type_idx
//           uint16 ret_type_idx
//   PTR/REF/ARRAY: uint16 elem_type_idx
//   TMPL:   uint8 arg_count
//           arg_count × uint16 type_idx
//
// Decl payload encodings:
//   FN/META:   uint8 param_count
//              param_count × uint16 type_idx
//              uint16 ret_type_idx
//   CLASS:   uint32 num_bases
//            num_bases × uint32 name_off
//            uint32 num_fields
//            num_fields × (uint32 name_off + uint16 type_idx)
//            uint32 num_methods
//            num_methods × uint16 decl_idx
//   ALIAS:   uint16 target_type_idx

struct BinSigType {
    enum Tag : uint8_t { Prim=0, Named=1, Func=2, Array=3, Ptr=4, Ref=5, Tmpl=6 };
    Tag tag = Prim;
    uint8_t primKind = 0; // see PrimKind
    std::string name;
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
    // For Fn/Meta:
    std::vector<std::pair<std::string, std::shared_ptr<BinSigType>>> params; // (name, type)
    BinSigType returnType;
    // For Class:
    std::vector<std::string> bases;
    std::vector<std::pair<std::string, BinSigType>> fields;
    std::vector<BinSigDecl> methods;
    // For Alias:
    BinSigType aliasTarget;
};

// Binary writer for version 3 TLV format
class BinSigWriter {
public:
    std::vector<uint8_t> write(const std::vector<BinSigDecl>& decls);

private:
    std::vector<uint8_t> buf_;
    std::map<const BinSigType*, uint16_t> typeIndex_;
    void writeU8(uint8_t v) { buf_.push_back(v); }
    void writeU16(uint16_t v) { buf_.push_back(v & 0xFF); buf_.push_back((v >> 8) & 0xFF); }
    void writeU32(uint32_t v) { for (int i=0;i<4;i++) buf_.push_back((v>>(i*8))&0xFF); }
    void writeStr(const std::string& s);
    void writeType(const BinSigType& t, std::map<const BinSigType*, uint16_t>& typeIndex,
                   std::map<std::string, const BinSigType*>& canonicalTypes);
    void writeDeclPayload(const BinSigDecl& d, std::map<const BinSigType*, uint16_t>& typeIndex,
                          std::map<std::string, const BinSigType*>& canonicalTypes,
                          size_t baseIndex = 0);
};

// Binary reader for version 3 TLV format
class BinSigReader {
public:
    bool read(const std::vector<uint8_t>& data);
    const std::vector<BinSigDecl>& decls() const { return decls_; }

private:
    std::vector<BinSigDecl> decls_;
};

// Conversion to the human-readable text format (for debugging / dumps).
SigType binTypeToText(const BinSigType& t);
SigDecl binDeclToText(const BinSigDecl& d);
std::string binDeclToString(const BinSigDecl& d);

} // namespace at
