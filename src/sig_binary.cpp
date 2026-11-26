#include "sig_binary.h"

#include <functional>
#include <memory>
#include <stdexcept>
#include <cstring>
#include <sstream>

#include "../std_modules/at_sigs.h"

namespace at {

// Canonical type key for deduplication: serialize type structure to a string.
static std::string typeKey(const BinSigType& t) {
    std::ostringstream os;
    os << (int)t.tag;
    if (t.tag == BinSigType::Prim) os << ":" << (int)t.primKind;
    else if (t.tag == BinSigType::Named) os << ":" << t.name;
    else if (t.tag == BinSigType::Tmpl) {
        os << ":" << t.name;
        for (const auto& p : t.params) os << "," << typeKey(*p);
    } else if (t.tag == BinSigType::Func) {
        os << ":";
        for (const auto& p : t.params) os << typeKey(*p) << ";";
        os << "->";
        if (t.returnType) os << typeKey(*t.returnType);
    } else if (t.tag == BinSigType::Ptr || t.tag == BinSigType::Ref || t.tag == BinSigType::Array) {
        if (t.elementType) os << ":" << typeKey(*t.elementType);
    }
    return os.str();
}

static uint16_t typeIndexFor(const BinSigType* t,
                              std::map<const BinSigType*, uint16_t>& typeIndex,
                              std::map<std::string, const BinSigType*>& canonicalTypes) {
    std::string key = typeKey(*t);
    auto cit = canonicalTypes.find(key);
    if (cit != canonicalTypes.end()) {
        t = cit->second; // use canonical pointer
    } else {
        canonicalTypes[key] = t; // register canonical pointer
    }
    auto it = typeIndex.find(t);
    if (it != typeIndex.end()) return it->second;
    uint16_t idx = (uint16_t)typeIndex.size();
    typeIndex[t] = idx;
    return idx;
}

void BinSigWriter::writeStr(const std::string& s) {
    writeU16((uint16_t)s.size());
    for (char c : s) buf_.push_back(c);
}

void BinSigWriter::writeType(const BinSigType& t, std::map<const BinSigType*, uint16_t>& typeIndex,
                              std::map<std::string, const BinSigType*>& canonicalTypes) {
    switch (t.tag) {
        case BinSigType::Prim:
            writeU8(t.primKind);
            break;
        case BinSigType::Named:
            writeStr(t.name);
            break;
        case BinSigType::Func: {
            writeU8((uint8_t)t.params.size());
            for (const auto& p : t.params) writeU16(typeIndexFor(p.get(), typeIndex, canonicalTypes));
            writeU16(typeIndexFor(t.returnType.get(), typeIndex, canonicalTypes));
            break;
        }
        case BinSigType::Ptr:
        case BinSigType::Ref:
        case BinSigType::Array:
            writeU16(typeIndexFor(t.elementType.get(), typeIndex, canonicalTypes));
            break;
        case BinSigType::Tmpl:
            writeStr(t.name);
            writeU8((uint8_t)t.params.size());
            for (const auto& p : t.params) writeU16(typeIndexFor(p.get(), typeIndex, canonicalTypes));
            break;
    }
}

void BinSigWriter::writeDeclPayload(const BinSigDecl& d, std::map<const BinSigType*, uint16_t>& typeIndex,
                                     std::map<std::string, const BinSigType*>& canonicalTypes,
                                     size_t baseIndex) {
    switch (d.kind) {
        case BinSigDecl::Fn:
        case BinSigDecl::Meta: {
            writeU8(d.kind == BinSigDecl::Meta ? 1 : 0);
            writeU8((uint8_t)d.templateParams.size());
            for (const auto& tp : d.templateParams) writeStr(tp);
            writeU8((uint8_t)d.params.size());
            for (const auto& p : d.params) {
                writeStr(p.first);
                writeU16(typeIndexFor(p.second.get(), typeIndex, canonicalTypes));
            }
            writeU16(typeIndexFor(&d.returnType, typeIndex, canonicalTypes));
            break;
        }
        case BinSigDecl::Class: {
            writeU32((uint32_t)d.bases.size());
            for (const auto& b : d.bases) writeStr(b);
            writeU32((uint32_t)d.fields.size());
            for (const auto& f : d.fields) {
                writeStr(f.first);
                writeU16(typeIndexFor(&f.second, typeIndex, canonicalTypes));
            }
            // Write method count + indices (methods are separate top-level decls)
            writeU32((uint32_t)d.methods.size());
            for (size_t j = 0; j < d.methods.size(); ++j) {
                writeU16((uint16_t)(baseIndex + j));
            }
            break;
        }
        case BinSigDecl::Alias: {
            writeU16(typeIndexFor(&d.aliasTarget, typeIndex, canonicalTypes));
            break;
        }
    }
}

std::vector<uint8_t> BinSigWriter::write(const std::vector<BinSigDecl>& decls) {
    buf_.clear();

    std::vector<std::string> strings;
    std::map<const BinSigType*, uint16_t> typeIndex;
    std::map<std::string, const BinSigType*> canonicalTypes;

    auto addString = [&](const std::string& s) -> uint32_t {
        uint32_t off = 0;
        for (const auto& existing : strings) {
            if (existing == s) return off;
            off += (uint32_t)(existing.size() + 1);
        }
        strings.push_back(s);
        return off;
    };

    auto addType = [&](const BinSigType* t) -> uint16_t {
        return typeIndexFor(t, typeIndex, canonicalTypes);
    };

    std::function<void(const BinSigType&)> collectTypes = [&](const BinSigType& t) {
        addString(t.name);
        switch (t.tag) {
            case BinSigType::Func:
                for (const auto& p : t.params) {
                    collectTypes(*p);
                }
                if (t.returnType) collectTypes(*t.returnType);
                break;
            case BinSigType::Ptr:
            case BinSigType::Ref:
            case BinSigType::Array:
                if (t.elementType) collectTypes(*t.elementType);
                break;
            case BinSigType::Tmpl:
                addString(t.name);
                for (const auto& p : t.params) collectTypes(*p);
                break;
            default: break;
        }
        addType(&t);
    };

    // Collect all decls, inserting methods right after their class
    std::vector<BinSigDecl> allDecls;
    for (const auto& d : decls) {
        allDecls.push_back(d);
        if (d.kind == BinSigDecl::Class) {
            for (const auto& m : d.methods) {
                allDecls.push_back(m);
            }
        }
    }

    for (const auto& d : allDecls) {
        addString(d.name);
        for (const auto& tp : d.templateParams) addString(tp);
        switch (d.kind) {
            case BinSigDecl::Fn:
            case BinSigDecl::Meta:
                for (const auto& p : d.params) {
                    addString(p.first);
                    collectTypes(*p.second);
                }
                if (d.returnType.tag != BinSigType::Prim || d.returnType.primKind != BinSigType::PVoid)
                    collectTypes(d.returnType);
                break;
            case BinSigDecl::Class:
                for (const auto& b : d.bases) addString(b);
                for (const auto& f : d.fields) {
                    addString(f.first);
                    collectTypes(f.second);
                }
                break;
            case BinSigDecl::Alias:
                collectTypes(d.aliasTarget);
                break;
        }
    }

    std::vector<const BinSigType*> sortedTypes(typeIndex.size());
    for (auto& kv : typeIndex) sortedTypes[kv.second] = kv.first;

    buf_.push_back('A'); buf_.push_back('T'); buf_.push_back('S'); buf_.push_back('G');
    writeU32(3); // version 3
    writeU32((uint32_t)typeIndex.size());
    writeU32((uint32_t)allDecls.size());

    // Write type records: uint8 tag, uint32 payload_len, uint8 payload[payload_len]
    for (auto* t : sortedTypes) {
        writeU8((uint8_t)t->tag); // type tag
        size_t payloadLenPos = buf_.size();
        buf_.resize(buf_.size() + 4); // reserve payload_len
        size_t payloadBodyStart = buf_.size();

        writeType(*t, typeIndex, canonicalTypes);

        uint32_t payloadLen = (uint32_t)(buf_.size() - payloadBodyStart);
        uint8_t* lenPtr = &buf_[payloadLenPos];
        lenPtr[0] = payloadLen & 0xFF;
        lenPtr[1] = (payloadLen >> 8) & 0xFF;
        lenPtr[2] = (payloadLen >> 16) & 0xFF;
        lenPtr[3] = (payloadLen >> 24) & 0xFF;
    }

    // Write decl records: uint8 kind, uint16 name_len, name bytes, uint32 payload_len, uint8 payload[payload_len]
    for (const auto& d : allDecls) {
        writeU8((uint8_t)d.kind); // decl kind
        writeStr(d.name);         // decl name (length-prefixed)
        size_t payloadLenPos = buf_.size();
        buf_.resize(buf_.size() + 4); // reserve payload_len
        size_t payloadBodyStart = buf_.size();

        // Base index for method indices: methods are right after their class in allDecls
        size_t baseIndex = 0;
        for (size_t k = 0; k < allDecls.size(); ++k) {
            if (&allDecls[k] == &d) { baseIndex = k + 1; break; }
        }
        writeDeclPayload(d, typeIndex, canonicalTypes, baseIndex);

        uint32_t payloadLen = (uint32_t)(buf_.size() - payloadBodyStart);
        uint8_t* lenPtr = &buf_[payloadLenPos];
        lenPtr[0] = payloadLen & 0xFF;
        lenPtr[1] = (payloadLen >> 8) & 0xFF;
        lenPtr[2] = (payloadLen >> 16) & 0xFF;
        lenPtr[3] = (payloadLen >> 24) & 0xFF;
    }

    // Write header decl count as allDecls.size()
    // (already written above as num_decls, but we need to fix it)
    // Actually num_decls was written before we knew allDecls size.
    // Fix it now:
    uint32_t* declCountPtr = reinterpret_cast<uint32_t*>(&buf_[12]);
    *declCountPtr = (uint32_t)allDecls.size();

    for (const auto& s : strings) {
        for (char c : s) buf_.push_back((uint8_t)c);
        buf_.push_back(0);
    }

    return buf_;
}

bool BinSigReader::read(const std::vector<uint8_t>& data) {
    decls_.clear();
    if (data.size() < 16) return false;
    if (data[0] != 'A' || data[1] != 'T' || data[2] != 'S' || data[3] != 'G') return false;

    const uint8_t* p = data.data();
    p += 4; // skip magic
    uint32_t version = *reinterpret_cast<const uint32_t*>(p); p += 4;
    if (version != 3) return false;
    uint32_t num_types = *reinterpret_cast<const uint32_t*>(p); p += 4;
    uint32_t num_decls = *reinterpret_cast<const uint32_t*>(p); p += 4;

    std::vector<BinSigType> btypes;
    btypes.reserve(num_types);
    for (uint32_t i = 0; i < num_types; ++i) {
        if (p + 1 + 4 > data.data() + data.size()) return false;
        uint8_t tag = *p++;
        uint32_t payloadLen = *reinterpret_cast<const uint32_t*>(p); p += 4;
        if (p + payloadLen > data.data() + data.size()) return false;
        const uint8_t* payloadStart = p;
        p += payloadLen;

        BinSigType bt;
        const uint8_t* pp = payloadStart;
        switch (tag) {
            case ATSG_T_PRIM:
                bt.tag = BinSigType::Prim;
                bt.primKind = *pp++;
                break;
            case BinSigType::Named: {
                bt.tag = BinSigType::Named;
                uint16_t nameLen = *reinterpret_cast<const uint16_t*>(pp); pp += 2;
                bt.name = std::string(reinterpret_cast<const char*>(pp), nameLen);
                pp += nameLen;
                break;
            }
            case BinSigType::Func: {
                bt.tag = BinSigType::Func;
                uint8_t paramCount = *pp++;
                for (uint8_t j = 0; j < paramCount; ++j) {
                    uint16_t typeIdx = *reinterpret_cast<const uint16_t*>(pp); pp += 2;
                    bt.params.push_back(btypes.size() > typeIdx ? std::make_shared<BinSigType>(btypes[typeIdx]) : std::make_shared<BinSigType>(BinSigType::prim(BinSigType::PUnknown)));
                }
                uint16_t retIdx = *reinterpret_cast<const uint16_t*>(pp); pp += 2;
                bt.returnType = btypes.size() > retIdx ? std::make_shared<BinSigType>(btypes[retIdx]) : std::make_shared<BinSigType>(BinSigType::prim(BinSigType::PVoid));
                break;
            }
            case ATSG_T_PTR:
            case ATSG_T_REF:
            case ATSG_T_ARRAY: {
                bt.tag = (tag == ATSG_T_ARRAY) ? BinSigType::Array
                       : (tag == ATSG_T_PTR)   ? BinSigType::Ptr
                                               : BinSigType::Ref;
                uint16_t elemIdx = *reinterpret_cast<const uint16_t*>(pp); pp += 2;
                bt.elementType = btypes.size() > elemIdx ? std::make_shared<BinSigType>(btypes[elemIdx]) : std::make_shared<BinSigType>(BinSigType::prim(BinSigType::PUnknown));
                break;
            }
            case ATSG_T_TMPL: {
                bt.tag = BinSigType::Tmpl;
                uint16_t nameLen = *reinterpret_cast<const uint16_t*>(pp); pp += 2;
                bt.name = std::string(reinterpret_cast<const char*>(pp), nameLen);
                pp += nameLen;
                uint8_t argCount = *pp++;
                for (uint8_t j = 0; j < argCount; ++j) {
                    uint16_t typeIdx = *reinterpret_cast<const uint16_t*>(pp); pp += 2;
                    bt.params.push_back(btypes.size() > typeIdx ? std::make_shared<BinSigType>(btypes[typeIdx]) : std::make_shared<BinSigType>(BinSigType::prim(BinSigType::PUnknown)));
                }
                break;
            }
        }
        btypes.push_back(bt);
    }

    // First pass: read all decls without resolving class method indices
    // (methods are separate top-level decls that come after classes)
    decls_.clear();
    decls_.reserve(num_decls);
    // Store pending method index lists for classes
    struct PendingClass {
        size_t declIndex;
        std::vector<uint16_t> methodIndices;
    };
    std::vector<PendingClass> pendingClasses;

    for (uint32_t i = 0; i < num_decls; ++i) {
        if (p + 1 > data.data() + data.size()) return false;
        uint8_t kind = *p++;
        // Read name: uint16 length + bytes
        if (p + 2 > data.data() + data.size()) return false;
        uint16_t nameLen = *reinterpret_cast<const uint16_t*>(p); p += 2;
        if (p + nameLen > data.data() + data.size()) return false;
        std::string name(reinterpret_cast<const char*>(p), nameLen);
        p += nameLen;
        // Read payload_len
        if (p + 4 > data.data() + data.size()) return false;
        uint32_t payloadLen = *reinterpret_cast<const uint32_t*>(p); p += 4;
        if (p + payloadLen > data.data() + data.size()) return false;
        const uint8_t* payloadStart = p;
        p += payloadLen;

        BinSigDecl d;
        d.kind = (BinSigDecl::Kind)kind;
        d.name = name;
        const uint8_t* pp = payloadStart;
        switch (kind) {
            case BinSigDecl::Fn:
            case BinSigDecl::Meta: {
                d.kind = kind == BinSigDecl::Meta ? BinSigDecl::Meta : BinSigDecl::Fn;
                uint8_t metaFlag = *pp++; // meta flag byte
                uint8_t tmplCount = *pp++; // template param count
                for (uint8_t j = 0; j < tmplCount; ++j) {
                    uint16_t tnameLen = *reinterpret_cast<const uint16_t*>(pp); pp += 2;
                    d.templateParams.push_back(std::string(reinterpret_cast<const char*>(pp), tnameLen));
                    pp += tnameLen;
                }
                uint8_t paramCount = *pp++;
                for (uint8_t j = 0; j < paramCount; ++j) {
                    uint16_t pnameLen = *reinterpret_cast<const uint16_t*>(pp); pp += 2;
                    std::string pname(reinterpret_cast<const char*>(pp), pnameLen);
                    pp += pnameLen;
                    uint16_t typeIdx = *reinterpret_cast<const uint16_t*>(pp); pp += 2;
                    d.params.push_back({pname, btypes.size() > typeIdx ? std::make_shared<BinSigType>(btypes[typeIdx]) : std::make_shared<BinSigType>(BinSigType::prim(BinSigType::PUnknown))});
                }
                uint16_t retIdx = *reinterpret_cast<const uint16_t*>(pp); pp += 2;
                d.returnType = btypes.size() > retIdx ? btypes[retIdx] : BinSigType::prim(BinSigType::PVoid);
                break;
            }
            case BinSigDecl::Class: {
                uint32_t numBases = *reinterpret_cast<const uint32_t*>(pp); pp += 4;
                for (uint32_t j = 0; j < numBases; ++j) {
                    uint16_t nameLen = *reinterpret_cast<const uint16_t*>(pp); pp += 2;
                    d.bases.push_back(std::string(reinterpret_cast<const char*>(pp), nameLen));
                    pp += nameLen;
                }
                uint32_t numFields = *reinterpret_cast<const uint32_t*>(pp); pp += 4;
                for (uint32_t j = 0; j < numFields; ++j) {
                    uint16_t nameLen = *reinterpret_cast<const uint16_t*>(pp); pp += 2;
                    std::string fname(reinterpret_cast<const char*>(pp), nameLen);
                    pp += nameLen;
                    uint16_t typeIdx = *reinterpret_cast<const uint16_t*>(pp); pp += 2;
                    d.fields.push_back({fname, btypes.size() > typeIdx ? btypes[typeIdx] : BinSigType::prim(BinSigType::PUnknown)});
                }
                uint32_t numMethods = *reinterpret_cast<const uint32_t*>(pp); pp += 4;
                std::vector<uint16_t> methodIndices(numMethods);
                for (uint32_t j = 0; j < numMethods; ++j) {
                    methodIndices[j] = *reinterpret_cast<const uint16_t*>(pp); pp += 2;
                }
                pendingClasses.push_back({decls_.size(), std::move(methodIndices)});
                break;
            }
            case BinSigDecl::Alias: {
                uint16_t targetIdx = *reinterpret_cast<const uint16_t*>(pp); pp += 2;
                d.aliasTarget = btypes.size() > targetIdx ? btypes[targetIdx] : BinSigType::prim(BinSigType::PUnknown);
                break;
            }
        }
        decls_.push_back(d);
    }

    // Second pass: resolve class method indices
    for (const auto& pc : pendingClasses) {
        BinSigDecl& cls = decls_[pc.declIndex];
        for (uint16_t mi : pc.methodIndices) {
            if (mi < decls_.size()) {
                cls.methods.push_back(decls_[mi]);
            }
        }
    }

    return true;
}

SigType binTypeToText(const BinSigType& t) {
    static const char* primNames[] = {"int", "decimal", "string", "void", "ptr", "unknown"};
    SigType r;
    switch (t.tag) {
        case BinSigType::Prim:
            r.kind = SigType::Kind::Primitive;
            r.name = t.primKind < 6 ? primNames[t.primKind] : "?";
            break;
        case BinSigType::Named:
            r.kind = SigType::Kind::Class;
            r.name = t.name;
            break;
        case BinSigType::Func:
            r.kind = SigType::Kind::Function;
            r.name = "function";
            for (const auto& p : t.params) r.params.push_back(binTypeToText(*p));
            r.returnType = std::make_shared<SigType>(
                t.returnType ? binTypeToText(*t.returnType) : binTypeToText(BinSigType::prim(BinSigType::PVoid)));
            break;
        case BinSigType::Array:
        case BinSigType::Ptr:
        case BinSigType::Ref: {
            r.kind = t.tag == BinSigType::Array ? SigType::Kind::Array
                   : t.tag == BinSigType::Ptr   ? SigType::Kind::Pointer
                                                : SigType::Kind::Reference;
            SigType elem = t.elementType ? binTypeToText(*t.elementType) : SigType{};
            r.name = elem.name;
            r.params.push_back(elem);
            break;
        }
        case BinSigType::Tmpl:
            r.kind = SigType::Kind::Class;
            r.name = t.name;
            for (const auto& p : t.params) r.params.push_back(binTypeToText(*p));
            break;
    }
    return r;
}

SigDecl binDeclToText(const BinSigDecl& d) {
    SigDecl r;
    r.kind = static_cast<SigDecl::Kind>(d.kind);
    r.name = d.name;
    r.templateParams = d.templateParams;
    for (const auto& p : d.params) {
        r.params.push_back({p.first, binTypeToText(*p.second)});
    }
    r.returnType = binTypeToText(d.returnType);
    r.bases = d.bases;
    for (const auto& f : d.fields) r.fields.push_back({f.first, binTypeToText(f.second)});
    for (const auto& m : d.methods) r.methods.push_back(binDeclToText(m));
    r.aliasTarget = binTypeToText(d.aliasTarget);
    return r;
}

std::string binDeclToString(const BinSigDecl& d) {
    std::ostringstream oss;
    if (d.kind == BinSigDecl::Alias) {
        oss << "alias " << d.name << " = ";
        const auto& t = d.aliasTarget;
        switch (t.tag) {
            case BinSigType::Ptr:
                oss << "*";
                if (t.elementType) {
                    const auto& e = *t.elementType;
                    if (e.tag == BinSigType::Array && e.elementType) {
                        const auto& ee = *e.elementType;
                        if (ee.tag == BinSigType::Named) {
                            oss << ee.name << "[]";
                        } else {
                            oss << binTypeToText(ee).name << "[]";
                        }
                    } else {
                        oss << binTypeToText(e).name << "[]";
                    }
                }
                break;
            default:
                oss << binTypeToText(t).name;
                break;
        }
        return oss.str();
    }
    if (d.kind == BinSigDecl::Fn || d.kind == BinSigDecl::Meta)
        oss << "fn ";
    oss << d.name << " (";
    for (size_t i = 0; i < d.params.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << d.params[i].first << ": " << binTypeToText(*d.params[i].second).name;
    }
    oss << ") -> " << binTypeToText(d.returnType).name;
    return oss.str();
}

} // namespace at
