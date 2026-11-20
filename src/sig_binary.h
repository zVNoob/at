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
// Version 2 layout (used by C modules via at_sigs.h):
//
//   Header (16 bytes):
//     magic: "ATSG" (4 bytes)
//     version: uint32 (=2) (4 bytes)
//     num_types: uint32 (4 bytes)
//     num_decls: uint32 (4 bytes)
//
//   Type table: num_types * sizeof(ATSigType) packed structs
//   Decl table: num_decls * sizeof(ATSigDecl) packed structs
//   String table: null-terminated C strings
//
// See std_modules/at_sigs.h for the C struct definitions (ATSigType, ATSigDecl).
//
// Type encoding:
//   tag: uint8
//     0 = primitive (uint8 primKind: 0=int, 1=decimal, 2=string, 3=void, 4=ptr, 5=unknown)
//     1 = named (uint16 name_off)
//     2 = function (uint8 param_count, uint16 ret_type_idx, uint16 param_type_idx[N])
//     3 = array (uint16 elem_type_idx)
//     4 = pointer (uint16 elem_type_idx)
//     5 = reference (uint16 elem_type_idx)
//     6 = template (uint16 name_off, uint8 tmpl_arg_count, uint16 tmpl_arg_type_idx[N])

struct BinSigType {
    enum Tag : uint8_t { Prim=0, Named=1, Func=2, Array=3, Ptr=4, Ref=5, Tmpl=6 };
    Tag tag = Prim;
    uint8_t primKind = 0; // see PrimKind
    std::string name;
    std::vector<BinSigType> params; // for Func (params) or Tmpl (args)
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
    std::vector<std::pair<std::string, BinSigType>> params; // (name, type)
    BinSigType returnType;
    // For Class:
    std::vector<std::string> bases;
    std::vector<std::pair<std::string, BinSigType>> fields;
    std::vector<BinSigDecl> methods;
    // For Alias:
    BinSigType aliasTarget;
};

// Binary writer
class BinSigWriter {
public:
    std::vector<uint8_t> write(const std::vector<BinSigDecl>& decls);

private:
    std::vector<uint8_t> buf_;
    void writeU8(uint8_t v) { buf_.push_back(v); }
    void writeU16(uint16_t v) { buf_.push_back(v & 0xFF); buf_.push_back((v >> 8) & 0xFF); }
    void writeU32(uint32_t v) { for (int i=0;i<4;i++) buf_.push_back((v>>(i*8))&0xFF); }
    void writeStr(const std::string& s) { writeU16((uint16_t)s.size()); for (char c: s) buf_.push_back(c); }
    void writeType(const BinSigType& t);
    void writeDecl(const BinSigDecl& d);
};

// Binary reader: reads version 2 format (packed C structs from at_sigs.h)
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
