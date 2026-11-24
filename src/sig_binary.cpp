#include "sig_binary.h"

#include <functional>
#include <memory>
#include <stdexcept>
#include <cstring>
#include <sstream>

#include "../std_modules/at_sigs.h"

namespace at {

void BinSigWriter::writeStr(const std::string& s) {
    writeU16((uint16_t)s.size());
    for (char c : s) buf_.push_back(c);
}

void BinSigWriter::writeType(const BinSigType& t) {
    writeU8((uint8_t)t.tag);
    switch (t.tag) {
        case BinSigType::Prim:
            writeU8(t.primKind);
            break;
        case BinSigType::Named:
            writeStr(t.name);
            break;
        case BinSigType::Func: {
            writeU8((uint8_t)t.params.size());
            for (const auto& p : t.params) writeU16(0);
            writeU16(0);
            break;
        }
        case BinSigType::Ptr:
            writeU16(0);
            break;
        case BinSigType::Ref:
            writeU16(0);
            break;
        case BinSigType::Array:
            writeU16(0);
            break;
        case BinSigType::Tmpl:
            writeStr(t.name);
            writeU8((uint8_t)t.params.size());
            for (const auto& p : t.params) writeU16(0);
            break;
    }
}

void BinSigWriter::writeDecl(const BinSigDecl& d) {
    writeU8((uint8_t)d.kind);
    writeStr(d.name);

    switch (d.kind) {
        case BinSigDecl::Fn:
        case BinSigDecl::Meta: {
            writeU8(d.kind == BinSigDecl::Meta ? 1 : 0);
            writeU8((uint8_t)d.params.size());
            for (const auto& p : d.params) writeU16(0);
            writeU16(0);
            break;
        }
        case BinSigDecl::Class: {
            writeU32((uint32_t)d.bases.size());
            for (const auto& b : d.bases) writeStr(b);
            writeU32((uint32_t)d.fields.size());
            for (const auto& f : d.fields) {
                writeStr(f.first);
                writeU16(0);
            }
            writeU32((uint32_t)d.methods.size());
            for (const auto& m : d.methods) writeDecl(m);
            break;
        }
        case BinSigDecl::Alias: {
            writeU16(0);
            break;
        }
    }
}

std::vector<uint8_t> BinSigWriter::write(const std::vector<BinSigDecl>& decls) {
    buf_.clear();

    std::vector<std::string> strings;
    std::map<const BinSigType*, uint16_t> typeIndex;

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
        auto it = typeIndex.find(t);
        if (it != typeIndex.end()) return it->second;
        uint16_t idx = (uint16_t)typeIndex.size();
        typeIndex[t] = idx;
        return idx;
    };

    std::function<void(const BinSigType&)> collectTypes = [&](const BinSigType& t) {
        addType(&t);
        addString(t.name);
        switch (t.tag) {
            case BinSigType::Func:
                for (const auto& p : t.params) {
                    collectTypes(*p);
                }
                collectTypes(*t.returnType);
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
    };

    for (const auto& d : decls) {
        addString(d.name);
        for (const auto& tp : d.templateParams) addString(tp);
        switch (d.kind) {
            case BinSigDecl::Fn:
            case BinSigDecl::Meta:
                for (const auto& p : d.params) {
                    addString(p.first);
                    collectTypes(*p.second);
                }
                collectTypes(d.returnType);
                break;
            case BinSigDecl::Class:
                for (const auto& b : d.bases) addString(b);
                for (const auto& f : d.fields) {
                    addString(f.first);
                    collectTypes(f.second);
                }
                for (const auto& m : d.methods) {
                    for (const auto& p : m.params) collectTypes(*p.second);
                    collectTypes(m.returnType);
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
    writeU32(3);
    writeU32((uint32_t)typeIndex.size());
    writeU32((uint32_t)decls.size());

    for (auto* t : sortedTypes) {
        size_t payloadStart = buf_.size();
        buf_.resize(buf_.size() + 4);
        size_t payloadBodyStart = buf_.size();

        switch (t->tag) {
            case BinSigType::Prim:
                writeU8(t->primKind);
                break;
            case BinSigType::Named:
                writeStr(t->name);
                break;
            case BinSigType::Func: {
                writeU8((uint8_t)t->params.size());
                for (const auto& p : t->params) writeU16(typeIndex.at(p.get()));
                writeU16(typeIndex.at(&*t->returnType));
                break;
            }
            case BinSigType::Ptr:
                writeU16(typeIndex.at(&*t->elementType));
                break;
            case BinSigType::Ref:
                writeU16(typeIndex.at(&*t->elementType));
                break;
            case BinSigType::Array:
                writeU16(typeIndex.at(&*t->elementType));
                break;
            case BinSigType::Tmpl:
                writeStr(t->name);
                writeU8((uint8_t)t->params.size());
                for (const auto& p : t->params) writeU16(typeIndex.at(p.get()));
                break;
        }

        uint32_t payloadLen = (uint32_t)(buf_.size() - payloadBodyStart);
        uint8_t* lenPtr = &buf_[payloadStart];
        lenPtr[0] = payloadLen & 0xFF;
        lenPtr[1] = (payloadLen >> 8) & 0xFF;
        lenPtr[2] = (payloadLen >> 16) & 0xFF;
        lenPtr[3] = (payloadLen >> 24) & 0xFF;
    }

    size_t declStart = buf_.size();
    buf_.resize(buf_.size() + 4);
    size_t declBodyStart = buf_.size();

    for (const auto& d : decls) {
        writeU8((uint8_t)d.kind);
        writeStr(d.name);

        switch (d.kind) {
            case BinSigDecl::Fn:
            case BinSigDecl::Meta: {
                writeU8(d.kind == BinSigDecl::Meta ? 1 : 0);
                writeU8((uint8_t)d.params.size());
                for (const auto& p : d.params) writeU16(0);
                writeU16(0);
                break;
            }
            case BinSigDecl::Class: {
                writeU32((uint32_t)d.bases.size());
                for (const auto& b : d.bases) writeStr(b);
                writeU32((uint32_t)d.fields.size());
                for (const auto& f : d.fields) {
                    writeStr(f.first);
                    writeU16(0);
                }
                writeU32((uint32_t)d.methods.size());
                for (const auto& m : d.methods) writeDecl(m);
                break;
            }
            case BinSigDecl::Alias: {
                writeU16(0);
                break;
            }
        }
    }

    uint32_t declPayloadLen = (uint32_t)(buf_.size() - declBodyStart);
    uint8_t* dLenPtr = &buf_[declStart];
    dLenPtr[0] = declPayloadLen & 0xFF;
    dLenPtr[1] = (declPayloadLen >> 8) & 0xFF;
    dLenPtr[2] = (declPayloadLen >> 16) & 0xFF;
    dLenPtr[3] = (declPayloadLen >> 24) & 0xFF;

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
    p += 4;
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
        BinSigType t;
        t.tag = (BinSigType::Tag)tag;
        const uint8_t* pp = payloadStart;
        switch (tag) {
            case ATSG_T_PRIM:
                t.primKind = *pp++;
                break;
            case BinSigType::Named: {
                uint32_t nameOff = *reinterpret_cast<const uint32_t*>(pp); pp += 4;
                break;
            }
            case BinSigType::Func: {
                uint8_t paramCount = *pp++;
                for (uint8_t j = 0; j < paramCount; ++j) {
                    uint16_t typeIdx = *reinterpret_cast<const uint16_t*>(pp); pp += 2;
                }
                uint16_t retIdx = *reinterpret_cast<const uint16_t*>(pp); pp += 2;
                break;
            }
            case ATSG_T_PTR:
            case ATSG_T_REF:
            case ATSG_T_ARRAY: {
                uint16_t elemIdx = *reinterpret_cast<const uint16_t*>(pp); pp += 2;
                break;
            }
            case ATSG_T_TMPL: {
                uint32_t nameOff = *reinterpret_cast<const uint32_t*>(pp); pp += 4;
                uint8_t argCount = *pp++;
                for (uint8_t j = 0; j < argCount; ++j) {
                    uint16_t typeIdx = *reinterpret_cast<const uint16_t*>(pp); pp += 2;
                }
                break;
            }
        }
        btypes.push_back(t);
    }

    decls_.clear();
    decls_.reserve(num_decls);
    for (uint32_t i = 0; i < num_decls; ++i) {
        if (p + 1 + 4 > data.data() + data.size()) return false;
        uint8_t kind = *p++;
        uint32_t payloadLen = *reinterpret_cast<const uint32_t*>(p); p += 4;
        if (p + payloadLen > data.data() + data.size()) return false;
        const uint8_t* payloadStart = p;
        p += payloadLen;

        BinSigDecl d;
        d.kind = (BinSigDecl::Kind)kind;
        const uint8_t* pp = payloadStart;
        switch (kind) {
            case BinSigDecl::Fn:
            case BinSigDecl::Meta: {
                d.kind = kind == BinSigDecl::Meta ? BinSigDecl::Meta : BinSigDecl::Fn;
                uint8_t paramCount = *pp++;
                d.params.reserve(paramCount);
                for (uint8_t j = 0; j < paramCount; ++j) {
                    uint16_t typeIdx = *reinterpret_cast<const uint16_t*>(pp); pp += 2;
                    d.params.push_back({"", btypes.size() > typeIdx ? std::make_shared<BinSigType>(btypes[typeIdx]) : std::make_shared<BinSigType>(BinSigType::prim(BinSigType::PUnknown))});
                }
                uint16_t retIdx = *reinterpret_cast<const uint16_t*>(pp); pp += 2;
                d.returnType = btypes.size() > retIdx ? btypes[retIdx] : BinSigType::prim(BinSigType::PVoid);
                break;
            }
            case BinSigDecl::Class: {
                uint32_t numBases = *reinterpret_cast<const uint32_t*>(pp); pp += 4;
                for (uint32_t j = 0; j < numBases; ++j) {
                    uint32_t nameOff = *reinterpret_cast<const uint32_t*>(pp); pp += 4;
                    d.bases.push_back(std::string(reinterpret_cast<const char*>(payloadStart + nameOff)));
                }
                uint32_t numFields = *reinterpret_cast<const uint32_t*>(pp); pp += 4;
                for (uint32_t j = 0; j < numFields; ++j) {
                    uint32_t nameOff = *reinterpret_cast<const uint32_t*>(pp); pp += 4;
                    uint16_t typeIdx = *reinterpret_cast<const uint16_t*>(pp); pp += 2;
                    d.fields.push_back({std::string(reinterpret_cast<const char*>(payloadStart + nameOff)),
                                       btypes.size() > typeIdx ? btypes[typeIdx] : BinSigType::prim(BinSigType::PUnknown)});
                }
                uint32_t numMethods = *reinterpret_cast<const uint32_t*>(pp); pp += 4;
                for (uint32_t j = 0; j < numMethods; ++j) {
                    uint16_t declIdx = *reinterpret_cast<const uint16_t*>(pp); pp += 2;
                    if (declIdx < num_decls) {
                        d.methods.push_back(decls_[declIdx]);
                    }
                }
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
    return true;
}SigType binTypeToText(const BinSigType& t) {
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
    r.fields.clear(); for (const auto& f : d.fields) r.fields.push_back({f.first, binTypeToText(f.second)});
    for (const auto& m : d.methods) r.methods.push_back(binDeclToText(m));
    r.aliasTarget = binTypeToText(d.aliasTarget);
    return r;
}

std::string binDeclToString(const BinSigDecl& d) {
    std::ostringstream oss;
    oss << d.name << "(";
    for (size_t i = 0; i < d.params.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << d.params[i].first << ": " << binTypeToText(*d.params[i].second).name;
    }
    oss << ") -> " << binTypeToText(d.returnType).name;
    return oss.str();
}

} // namespace at
