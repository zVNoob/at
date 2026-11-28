#include "sig_binary.h"

#include <functional>
#include <memory>
#include <stdexcept>
#include <cstring>
#include <sstream>

#include "../std_modules/at_sigs.h"

namespace at {

// Serialize a type to a string key for content-based deduplication.
static std::string typeKey(const BinSigType& t);

uint32_t BinSigWriter::writeStr(const std::string& s) {
    uint32_t off = (uint32_t)buf_.size();
    writeU16((uint16_t)s.size());
    for (char c : s) buf_.push_back((uint8_t)c);
    return off;
}

void BinSigWriter::writeType(const BinSigType& t) {
    switch (t.tag) {
        case BinSigType::Prim:
            writeU8(t.primKind);
            break;
        case BinSigType::Named:
            writeStr(t.name);
            writeU32(t.size);
            writeU32(t.alignment);
            writeU32(t.parent_count);
            for (uint32_t i = 0; i < t.parent_count; ++i) writeU32(t.parent_ids[i]);
            for (uint32_t i = 0; i < t.parent_count; ++i) writeU32(t.parent_offsets[i]);
            writeU32(t.field_count);
            for (uint32_t i = 0; i < t.field_count; ++i) {
                writeStr(t.fields[i].name);
                writeU16(typeIndex_[typeKey(*t.fields[i].type)]);
                writeU32(t.fields[i].offset);
                writeU8(t.fields[i].flags);
            }
            break;
        case BinSigType::Func: {
            writeU8((uint8_t)t.params.size());
            for (const auto& p : t.params) writeU16(typeIndex_[typeKey(*p)]);
            writeU16(typeIndex_[typeKey(*t.returnType)]);
            break;
        }
        case BinSigType::Ptr:
        case BinSigType::Ref:
        case BinSigType::Array:
            writeU16(typeIndex_[typeKey(*t.elementType)]);
            break;
        case BinSigType::Tmpl:
            writeStr(t.name);
            writeU8((uint8_t)t.params.size());
            for (const auto& p : t.params) writeU16(typeIndex_[typeKey(*p)]);
            break;
    }
}

void BinSigWriter::writeDecl(const BinSigDecl& d) {
    switch (d.kind) {
        case BinSigDecl::Fn:
        case BinSigDecl::Meta: {
            writeU8(d.flags);
            writeU8((uint8_t)d.templateParams.size());
            for (const auto& tp : d.templateParams) writeStr(tp);
            writeU8((uint8_t)d.params.size());
            for (const auto& p : d.params) {
                writeStr(p.first);
                writeU16(typeIndex_[typeKey(*p.second)]);
            }
            writeU16(typeIndex_[typeKey(d.returnType)]);
            break;
        }
        case BinSigDecl::Class: {
            // Look up the class type index from extraTypes_
            if (extraTypes_ && d.typeIdx < extraTypes_->size()) {
                writeU16(typeIndex_[typeKey((*extraTypes_)[d.typeIdx])]);
            } else {
                writeU16(0);
            }
            break;
        }
        case BinSigDecl::Alias: {
            writeU16(typeIndex_[typeKey(d.aliasTarget)]);
            break;
        }
    }
}

std::vector<uint8_t> BinSigWriter::write(const std::vector<BinSigDecl>& decls,
                                          const std::vector<BinSigType>& extraTypes) {
    buf_.clear();
    typeIndex_.clear();
    extraTypes_ = &extraTypes;

    // Collect all unique types into allTypes vector, in dependency order
    std::vector<BinSigType> allTypes;

    std::function<void(const BinSigType&)> collectAndStore = [&](const BinSigType& t) {
        std::string key = typeKey(t);
        if (typeIndex_.count(key)) return;

        // First, collect all dependencies (so they are added before the current type)
        switch (t.tag) {
            case BinSigType::Func:
                for (const auto& p : t.params) collectAndStore(*p);
                if (t.returnType) collectAndStore(*t.returnType);
                break;
            case BinSigType::Named:
                for (auto& f : t.fields) {
                    if (f.type) collectAndStore(*f.type);
                }
                break;
            case BinSigType::Ptr:
            case BinSigType::Ref:
            case BinSigType::Array:
                if (t.elementType) collectAndStore(*t.elementType);
                break;
            case BinSigType::Tmpl:
                for (const auto& p : t.params) collectAndStore(*p);
                break;
            default: break;
        }

        // Then add the current type
        uint16_t idx = (uint16_t)allTypes.size();
        allTypes.push_back(t);
        typeIndex_[key] = idx;
    };

    for (const auto& t : extraTypes) collectAndStore(t);
    for (const auto& d : decls) {
        switch (d.kind) {
            case BinSigDecl::Fn:
            case BinSigDecl::Meta:
                for (const auto& p : d.params) collectAndStore(*p.second);
                collectAndStore(d.returnType);
                break;
            case BinSigDecl::Alias:
                collectAndStore(d.aliasTarget);
                break;
            case BinSigDecl::Class:
                break;
        }
    }

    // Write header
    buf_.push_back('A'); buf_.push_back('T'); buf_.push_back('S'); buf_.push_back('G');
    writeU32(4); // version 4
    writeU32((uint32_t)allTypes.size());
    writeU32((uint32_t)decls.size());

    // Write type records: uint8 tag, uint32 payload_len, uint8 payload[payload_len]
    for (uint32_t i = 0; i < allTypes.size(); ++i) {
        const BinSigType* t = &allTypes[i];
        writeU8((uint8_t)t->tag);
        size_t payloadLenPos = buf_.size();
        buf_.resize(buf_.size() + 4);
        size_t payloadBodyStart = buf_.size();

        writeType(*t);

        uint32_t payloadLen = (uint32_t)(buf_.size() - payloadBodyStart);
        uint8_t* lenPtr = &buf_[payloadLenPos];
        lenPtr[0] = payloadLen & 0xFF;
        lenPtr[1] = (payloadLen >> 8) & 0xFF;
        lenPtr[2] = (payloadLen >> 16) & 0xFF;
        lenPtr[3] = (payloadLen >> 24) & 0xFF;
    }

    // Write decl records
    for (const auto& d : decls) {
        writeU8((uint8_t)d.kind);
        writeStr(d.name);
        size_t payloadLenPos = buf_.size();
        buf_.resize(buf_.size() + 4);
        size_t payloadBodyStart = buf_.size();

        switch (d.kind) {
            case BinSigDecl::Fn:
            case BinSigDecl::Meta: {
                writeU8(d.flags);
                writeU8((uint8_t)d.templateParams.size());
                for (const auto& tp : d.templateParams) writeStr(tp);
                writeU8((uint8_t)d.params.size());
                for (const auto& p : d.params) {
                    writeStr(p.first);
                    writeU16(typeIndex_[typeKey(*p.second)]);
                }
                writeU16(typeIndex_[typeKey(d.returnType)]);
                break;
            }
            case BinSigDecl::Class: {
                // Look up the class type index from extraTypes_
                if (extraTypes_ && d.typeIdx < extraTypes_->size()) {
                    writeU16(typeIndex_[typeKey((*extraTypes_)[d.typeIdx])]);
                } else {
                    writeU16(0);
                }
                break;
            }
            case BinSigDecl::Alias: {
                writeU16(typeIndex_[typeKey(d.aliasTarget)]);
                break;
            }
        }

        uint32_t payloadLen = (uint32_t)(buf_.size() - payloadBodyStart);
        uint8_t* lenPtr = &buf_[payloadLenPos];
        lenPtr[0] = payloadLen & 0xFF;
        lenPtr[1] = (payloadLen >> 8) & 0xFF;
        lenPtr[2] = (payloadLen >> 16) & 0xFF;
        lenPtr[3] = (payloadLen >> 24) & 0xFF;
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
    if (version != 4) return false;
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
                bt.size = *reinterpret_cast<const uint32_t*>(pp); pp += 4;
                bt.alignment = *reinterpret_cast<const uint32_t*>(pp); pp += 4;
                bt.parent_count = *reinterpret_cast<const uint32_t*>(pp); pp += 4;
                for (uint32_t j = 0; j < bt.parent_count; ++j) {
                    bt.parent_ids.push_back(*reinterpret_cast<const uint32_t*>(pp)); pp += 4;
                }
                for (uint32_t j = 0; j < bt.parent_count; ++j) {
                    bt.parent_offsets.push_back(*reinterpret_cast<const uint32_t*>(pp)); pp += 4;
                }
                bt.field_count = *reinterpret_cast<const uint32_t*>(pp); pp += 4;
                for (uint32_t j = 0; j < bt.field_count; ++j) {
                    BinSigType::Field f;
                    uint16_t nameLen = *reinterpret_cast<const uint16_t*>(pp); pp += 2;
                    f.name = std::string(reinterpret_cast<const char*>(pp), nameLen);
                    pp += nameLen;
                    uint16_t fieldTypeIdx = *reinterpret_cast<const uint16_t*>(pp); pp += 2;
                    f.type = btypes.size() > fieldTypeIdx ? std::make_shared<BinSigType>(btypes[fieldTypeIdx]) : std::make_shared<BinSigType>(BinSigType::prim(BinSigType::PUnknown));
                    f.offset = *reinterpret_cast<const uint32_t*>(pp); pp += 4;
                    f.flags = *pp++;
                    bt.fields.push_back(f);
                }
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

    decls_.clear();
    decls_.reserve(num_decls);
    for (uint32_t i = 0; i < num_decls; ++i) {
        if (p + 1 > data.data() + data.size()) return false;
        uint8_t kind = *p++;
        if (p + 2 > data.data() + data.size()) return false;
        uint16_t nameLen = *reinterpret_cast<const uint16_t*>(p); p += 2;
        if (p + nameLen > data.data() + data.size()) return false;
        std::string name(reinterpret_cast<const char*>(p), nameLen);
        p += nameLen;
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
                d.flags = *pp++;
                uint8_t tmplCount = *pp++;
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
                uint16_t typeIdx = *reinterpret_cast<const uint16_t*>(pp); pp += 2;
                d.typeIdx = typeIdx;
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
}

SigType binTypeToText(const BinSigType& t);

std::string typeKey(const BinSigType& t) {
    std::ostringstream os;
    os << (int)t.tag;
    if (t.tag == BinSigType::Prim) {
        os << ":" << (int)t.primKind;
    } else if (t.tag == BinSigType::Named) {
        os << ":" << t.name;
    } else if (t.tag == BinSigType::Func) {
        os << ":";
        for (const auto& p : t.params) os << typeKey(*p) << ";";
        os << "->";
        if (t.returnType) os << typeKey(*t.returnType);
    } else if (t.tag == BinSigType::Ptr || t.tag == BinSigType::Ref || t.tag == BinSigType::Array) {
        if (t.elementType) os << ":" << typeKey(*t.elementType);
    } else if (t.tag == BinSigType::Tmpl) {
        os << ":" << t.name;
        for (const auto& p : t.params) os << "," << typeKey(*p);
    }
    return os.str();
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
            if (t.tag == BinSigType::Array) r.name += "[]";
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
                    oss << binTypeToText(e).name;
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
