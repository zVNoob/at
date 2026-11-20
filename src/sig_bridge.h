#pragma once

#include "sig_binary.h"
#include "symbols.h"
#include "types.h"

#include <string>
#include <vector>

namespace at {

// Conversions between the semantic analyzer's Type/Symbol model and the
// binary .at.sigs model (BinSigType / BinSigDecl).
//
// Mapping notes:
//   - bool is an int (BigInt), so PrimitiveType::Bool encodes as PInt.
//   - PrimitiveType::Unknown encodes as PUnknown; typesCompatible() treats it
//     as a wildcard on both sides, so it round-trips with the same meaning.
//   - ClassType / AliasType / template params encode as Named(name).

BinSigType typeToBinSig(const TypePtr& t);
TypePtr binSigToType(const BinSigType& t);

// Build a declaration from a symbol. Returns false if the symbol has no
// representable signature (e.g. a plain variable or an untyped symbol).
bool symbolToBinSigDecl(const SymbolPtr& sym, BinSigDecl& out);

// Build signatures for every exported symbol of a scope.
std::vector<BinSigDecl> declsFromScope(const SymbolTable& scope);

// Turn a decoded declaration back into a Symbol with a semantic type.
SymbolPtr binSigDeclToSymbol(const BinSigDecl& d);

// Name of the ELF symbol that codegen emits for a declaration.
// Meta (template) functions are emitted as `<name>_meta`.
std::string elfSymbolNameFor(const BinSigDecl& d);

// Parse one line of the legacy text format used by the C runtime:
//   "name t1 t2 ... -> ret"   with t in {int, decimal, str, string, bool, ptr, void}
bool parseLegacySigLine(const std::string& line, BinSigDecl& out);

} // namespace at
