#include "sig_binary.h"

#include <functional>
#include <memory>
#include <stdexcept>
#include <cstring>

#include "../std_modules/at_sigs.h"

namespace at {

void BinSigWriter::writeType(const BinSigType& t) {
    writeU8(t.tag);
    switch (t.tag) {
        case BinSigType::Prim:
            writeU8(t.primKind);
            break;
        case BinSigType::Named:
            writeStr(t.name);
            break;
        case BinSigType::Func:
            writeU16((uint16_t)t.params.size());
            for (const auto& p : t.params) writeType(p);
            if (t.returnType) writeType(*t.returnType);
            else { writeU8(BinSigType::Prim); writeU8(3); } // void
            break;
        case BinSigType::Array:
            if (t.elementType) writeType(*t.elementType); else { writeU8(BinSigType::Prim); writeU8(0); }
            break;
        case BinSigType::Ptr:
            if (t.elementType) writeType(*t.elementType); else { writeU8(BinSigType::Prim); writeU8(0); }
            break;
        case BinSigType::Ref:
            if (t.elementType) writeType(*t.elementType); else { writeU8(BinSigType::Prim); writeU8(0); }
            break;
        case BinSigType::Tmpl:
            writeStr(t.name);
            writeU16((uint16_t)t.params.size());
            for (const auto& p : t.params) writeType(p);
            break;
    }
}

void BinSigWriter::writeDecl(const BinSigDecl& d) {
    writeU8(d.kind);
    writeStr(d.name);

    // Template params
    writeU8((uint8_t)d.templateParams.size());
    for (const auto& tp : d.templateParams) writeStr(tp);

    switch (d.kind) {
        case BinSigDecl::Fn:
        case BinSigDecl::Meta: {
            // Meta flag
            writeU8(d.kind == BinSigDecl::Meta ? 1 : 0);
            // Params
            writeU16((uint16_t)d.params.size());
            for (const auto& [name, type] : d.params) {
                writeStr(name);
                writeType(type);
            }
            // Return type
            writeType(d.returnType);
            break;
        }
        case BinSigDecl::Class: {
            // Bases
            writeU8((uint8_t)d.bases.size());
            for (const auto& b : d.bases) writeStr(b);
            // Fields
            writeU16((uint16_t)d.fields.size());
            for (const auto& [name, type] : d.fields) {
                writeStr(name);
                writeType(type);
            }
            // Methods
            writeU16((uint16_t)d.methods.size());
            for (const auto& m : d.methods) writeDecl(m);
            break;
        }
        case BinSigDecl::Alias: {
            writeType(d.aliasTarget);
            break;
        }
    }
}

std::vector<uint8_t> BinSigWriter::write(const std::vector<BinSigDecl>& decls) {
    buf_.clear();

    // Collect all strings
    std::vector<std::string> strings;
    auto addString = [&](const std::string& s) -> uint16_t {
        // Compute byte offset of this string in the string table
        uint16_t off = 0;
        for (const auto& existing : strings) {
            if (existing == s) return off;
            off += (uint16_t)(existing.size() + 1); // +1 for null terminator
        }
        strings.push_back(s);
        return off;
    };

    // Build type table (packed C structs)
    std::vector<ATSigType> ctypes;
    std::function<uint16_t(const BinSigType&)> addType = [&](const BinSigType& t) -> uint16_t {
        for (size_t i = 0; i < ctypes.size(); ++i)
            if (memcmp(&ctypes[i], &t, sizeof(ATSigType)) == 0) return (uint16_t)i;
        ATSigType ct{};
        ct.tag = (uint8_t)t.tag;
        ct.prim_kind = t.primKind;
        ct.name_off = addString(t.name);
        ct.param_count = 0;
        ct.ret_type_idx = 0;
        for (int j = 0; j < ATSG_MAX_PARAMS; ++j) ct.param_type_idx[j] = 0;
        ct.elem_type_idx = 0;
        ct.tmpl_arg_count = 0;
        for (int j = 0; j < ATSG_MAX_TMPL; ++j) ct.tmpl_arg_type_idx[j] = 0;
        switch (t.tag) {
            case BinSigType::Func:
                ct.param_count = (uint8_t)t.params.size();
                ct.ret_type_idx = addType(*t.returnType);
                for (size_t j = 0; j < t.params.size() && j < ATSG_MAX_PARAMS; ++j)
                    ct.param_type_idx[j] = addType(t.params[j]);
                break;
            case BinSigType::Array:
            case BinSigType::Ptr:
            case BinSigType::Ref:
                ct.elem_type_idx = t.elementType ? addType(*t.elementType) : 0;
                break;
            case BinSigType::Tmpl:
                ct.tmpl_arg_count = (uint8_t)t.params.size();
                for (size_t j = 0; j < t.params.size() && j < ATSG_MAX_TMPL; ++j)
                    ct.tmpl_arg_type_idx[j] = addType(t.params[j]);
                break;
            default: break;
        }
        ctypes.push_back(ct);
        return (uint16_t)(ctypes.size() - 1);
    };

    // Build decl table (packed C structs)
    // First pass: expand classes to include their methods as separate decl entries
    std::vector<ATSigDecl> cdecls;
    std::vector<const BinSigDecl*> method_parent;  // track which decl each method belongs to
    for (const auto& d : decls) {
        ATSigDecl cd{};
        cd.kind = (uint8_t)d.kind;
        cd.name_off = addString(d.name);
        cd.tmpl_param_count = (uint8_t)d.templateParams.size();
        for (size_t j = 0; j < d.templateParams.size() && j < ATSG_MAX_TMPL; ++j)
            cd.tmpl_param_name_off[j] = addString(d.templateParams[j]);
        cd.is_meta = (d.kind == BinSigDecl::Meta) ? 1 : 0;

        switch (d.kind) {
            case BinSigDecl::Fn:
            case BinSigDecl::Meta: {
                cd.param_count = (uint8_t)d.params.size();
                for (size_t j = 0; j < d.params.size() && j < ATSG_MAX_PARAMS; ++j) {
                    cd.param_name_off[j] = addString(d.params[j].first);
                    cd.param_type_idx[j] = addType(d.params[j].second);
                }
                cd.ret_type_idx = addType(d.returnType);
                break;
            }
            case BinSigDecl::Class: {
                cd.num_bases = (uint8_t)d.bases.size();
                for (size_t j = 0; j < d.bases.size() && j < ATSG_MAX_BASES; ++j)
                    cd.base_name_off[j] = addString(d.bases[j]);
                cd.num_fields = (uint16_t)d.fields.size();
                for (size_t j = 0; j < d.fields.size() && j < ATSG_MAX_FIELDS; ++j) {
                    cd.field_name_off[j] = addString(d.fields[j].first);
                    cd.field_type_idx[j] = addType(d.fields[j].second);
                }
                cd.num_methods = (uint16_t)d.methods.size();
                // Methods will be written as separate decls after this class
                for (size_t j = 0; j < ATSG_MAX_METHODS; ++j)
                    cd.method_decl_idx[j] = 0;
                break;
            }
            case BinSigDecl::Alias: {
                cd.target_type_idx = addType(d.aliasTarget);
                break;
            }
        }
        cdecls.push_back(cd);

        // If this is a class, write its methods as separate decls right after
        if (d.kind == BinSigDecl::Class) {
            uint16_t class_idx = (uint16_t)(cdecls.size() - 1);
            for (size_t j = 0; j < d.methods.size() && j < ATSG_MAX_METHODS; ++j) {
                const auto& m = d.methods[j];
                ATSigDecl md{};
                md.kind = ATSG_KIND_FN;  // methods are functions
                md.name_off = addString(m.name);
                md.is_meta = 0;
                md.param_count = (uint8_t)m.params.size();
                for (size_t k = 0; k < m.params.size() && k < ATSG_MAX_PARAMS; ++k) {
                    md.param_name_off[k] = addString(m.params[k].first);
                    md.param_type_idx[k] = addType(m.params[k].second);
                }
                md.ret_type_idx = addType(m.returnType);
                cdecls.push_back(md);
                // Point class's method_decl_idx to this method's position
                // method_decl_idx stores the index in cdecls
                // But ATSigDecl::method_decl_idx is uint16_t[16], so we store cdecls.size()-1
                // We need to go back and update the class decl
                // Since cdecls is a vector, we can modify the class entry:
                cdecls[class_idx].method_decl_idx[j] = (uint16_t)(cdecls.size() - 1);
            }
        }
    }

    uint32_t num_decls = (uint32_t)cdecls.size();

    // Emit header: magic + version + num_types + num_decls
    buf_.push_back('A'); buf_.push_back('T'); buf_.push_back('S'); buf_.push_back('G');
    writeU32(2); // version
    writeU32((uint32_t)ctypes.size());
    writeU32((uint32_t)cdecls.size());

    // Type table: raw packed structs
    for (const auto& ct : ctypes) {
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&ct);
        for (size_t i = 0; i < sizeof(ATSigType); ++i) buf_.push_back(bytes[i]);
    }

    // Decl table: raw packed structs
    for (const auto& cd : cdecls) {
        const uint8_t* bytes = reinterpret_cast<const uint8_t*>(&cd);
        for (size_t i = 0; i < sizeof(ATSigDecl); ++i) buf_.push_back(bytes[i]);
    }

    // String table
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
    if (version != 2) return false;
    uint32_t num_types = *reinterpret_cast<const uint32_t*>(p); p += 4;
    uint32_t num_decls = *reinterpret_cast<const uint32_t*>(p); p += 4;

    size_t type_table_size = (size_t)num_types * sizeof(ATSigType);
    size_t decl_table_size = (size_t)num_decls * sizeof(ATSigDecl);
    if (data.size() < 16 + type_table_size + decl_table_size) return false;

    // Type table
    const ATSigType* ctypes = reinterpret_cast<const ATSigType*>(p);
    p += type_table_size;

    // Decl table
    const ATSigDecl* cdecls = reinterpret_cast<const ATSigDecl*>(p);
    p += decl_table_size;

    // String table
    const char* cstrings = reinterpret_cast<const char*>(p);

    auto getString = [&](uint16_t off) -> std::string {
        const char* s = cstrings + off;
        const char* end = reinterpret_cast<const char*>(data.data() + data.size());
        const char* nul = s;
        while (nul < end && *nul) ++nul;
        if (nul >= end) return "";
        return std::string(s, nul - s);
    };

    // Build BinSigType vector from C types
    std::vector<BinSigType> btypes;
    btypes.reserve(num_types);
    for (uint32_t i = 0; i < num_types; ++i) {
        const ATSigType& ct = ctypes[i];
        BinSigType t;
        switch (ct.tag) {
            case ATSG_T_PRIM:
                t.tag = BinSigType::Prim;
                t.primKind = ct.prim_kind;
                break;
            case ATSG_T_NAMED:
                t.tag = BinSigType::Named;
                t.name = getString(ct.name_off);
                break;
            case ATSG_T_FUNC: {
                t.tag = BinSigType::Func;
                t.params.resize(ct.param_count);
                for (uint8_t j = 0; j < ct.param_count; ++j) {
                    if (ct.param_type_idx[j] < btypes.size())
                        t.params[j] = btypes[ct.param_type_idx[j]];
                    else
                        t.params[j] = BinSigType::prim(BinSigType::PUnknown);
                }
                if (ct.ret_type_idx < btypes.size())
                    t.returnType = std::make_shared<BinSigType>(btypes[ct.ret_type_idx]);
                else
                    t.returnType = std::make_shared<BinSigType>(BinSigType::prim(BinSigType::PVoid));
                break;
            }
            case ATSG_T_ARRAY:
                t.tag = BinSigType::Array;
                if (ct.elem_type_idx < btypes.size())
                    t.elementType = std::make_shared<BinSigType>(btypes[ct.elem_type_idx]);
                else
                    t.elementType = std::make_shared<BinSigType>(BinSigType::prim(BinSigType::PUnknown));
                break;
            case ATSG_T_PTR:
                t.tag = BinSigType::Ptr;
                if (ct.elem_type_idx < btypes.size())
                    t.elementType = std::make_shared<BinSigType>(btypes[ct.elem_type_idx]);
                else
                    t.elementType = std::make_shared<BinSigType>(BinSigType::prim(BinSigType::PUnknown));
                break;
            case ATSG_T_REF:
                t.tag = BinSigType::Ref;
                if (ct.elem_type_idx < btypes.size())
                    t.elementType = std::make_shared<BinSigType>(btypes[ct.elem_type_idx]);
                else
                    t.elementType = std::make_shared<BinSigType>(BinSigType::prim(BinSigType::PUnknown));
                break;
            case ATSG_T_TMPL:
                t.tag = BinSigType::Tmpl;
                t.name = getString(ct.name_off);
                t.params.resize(ct.tmpl_arg_count);
                for (uint8_t j = 0; j < ct.tmpl_arg_count; ++j) {
                    if (ct.tmpl_arg_type_idx[j] < btypes.size())
                        t.params[j] = btypes[ct.tmpl_arg_type_idx[j]];
                    else
                        t.params[j] = BinSigType::prim(BinSigType::PUnknown);
                }
                break;
            default:
                t.tag = BinSigType::Prim;
                t.primKind = BinSigType::PUnknown;
                break;
        }
        btypes.push_back(t);
    }

    // Build BinSigDecl vector from C decls
    decls_.clear();
    decls_.reserve(num_decls);
    for (uint32_t i = 0; i < num_decls; ++i) {
        const ATSigDecl& cd = cdecls[i];
        BinSigDecl d;
        switch (cd.kind) {
            case ATSG_KIND_FN:   d.kind = BinSigDecl::Fn; break;
            case ATSG_KIND_CLASS: d.kind = BinSigDecl::Class; break;
            case ATSG_KIND_ALIAS: d.kind = BinSigDecl::Alias; break;
            case ATSG_KIND_META: d.kind = BinSigDecl::Meta; break;
            default: d.kind = BinSigDecl::Fn; break;
        }
        d.name = getString(cd.name_off);
        d.templateParams.resize(cd.tmpl_param_count);
        for (uint8_t j = 0; j < cd.tmpl_param_count; ++j) {
            if (j < ATSG_MAX_TMPL)
                d.templateParams[j] = getString(cd.tmpl_param_name_off[j]);
        }

        switch (d.kind) {
            case BinSigDecl::Fn:
            case BinSigDecl::Meta: {
                d.kind = cd.is_meta ? BinSigDecl::Meta : BinSigDecl::Fn;
                d.params.resize(cd.param_count);
                for (uint8_t j = 0; j < cd.param_count; ++j) {
                    if (j < ATSG_MAX_PARAMS) {
                        d.params[j].first = getString(cd.param_name_off[j]);
                        if (cd.param_type_idx[j] < btypes.size())
                            d.params[j].second = btypes[cd.param_type_idx[j]];
                        else
                            d.params[j].second = BinSigType::prim(BinSigType::PUnknown);
                    }
                }
                if (cd.ret_type_idx < btypes.size())
                    d.returnType = btypes[cd.ret_type_idx];
                else
                    d.returnType = BinSigType::prim(BinSigType::PVoid);
                break;
            }
            case BinSigDecl::Class: {
                d.bases.resize(cd.num_bases);
                for (uint8_t j = 0; j < cd.num_bases; ++j) {
                    if (j < ATSG_MAX_BASES)
                        d.bases[j] = getString(cd.base_name_off[j]);
                }
                d.fields.resize(cd.num_fields);
                for (uint16_t j = 0; j < cd.num_fields; ++j) {
                    if (j < ATSG_MAX_FIELDS) {
                        d.fields[j].first = getString(cd.field_name_off[j]);
                        if (cd.field_type_idx[j] < btypes.size())
                            d.fields[j].second = btypes[cd.field_type_idx[j]];
                        else
                            d.fields[j].second = BinSigType::prim(BinSigType::PUnknown);
                    }
                }
                d.methods.resize(cd.num_methods);
                for (uint16_t j = 0; j < cd.num_methods; ++j) {
                    if (j < ATSG_MAX_METHODS) {
                        uint16_t method_idx = cd.method_decl_idx[j];
                        if (method_idx < num_decls) {
                            const ATSigDecl& mcd = cdecls[method_idx];
                            BinSigDecl md;
                            md.kind = BinSigDecl::Fn;
                            md.name = getString(mcd.name_off);
                            md.params.resize(mcd.param_count);
                            for (uint8_t k = 0; k < mcd.param_count; ++k) {
                                if (k < ATSG_MAX_PARAMS) {
                                    md.params[k].first = getString(mcd.param_name_off[k]);
                                    if (mcd.param_type_idx[k] < btypes.size())
                                        md.params[k].second = btypes[mcd.param_type_idx[k]];
                                    else
                                        md.params[k].second = BinSigType::prim(BinSigType::PUnknown);
                                }
                            }
                            if (mcd.ret_type_idx < btypes.size())
                                md.returnType = btypes[mcd.ret_type_idx];
                            else
                                md.returnType = BinSigType::prim(BinSigType::PVoid);
                            d.methods[j] = md;
                        }
                    }
                }
                break;
            }
            case BinSigDecl::Alias: {
                if (cd.target_type_idx < btypes.size())
                    d.aliasTarget = btypes[cd.target_type_idx];
                else
                    d.aliasTarget = BinSigType::prim(BinSigType::PUnknown);
                break;
            }
        }
        decls_.push_back(d);
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
            for (const auto& p : t.params) r.params.push_back(binTypeToText(p));
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
            for (const auto& p : t.params) r.params.push_back(binTypeToText(p));
            break;
    }
    return r;
}

SigDecl binDeclToText(const BinSigDecl& d) {
    SigDecl r;
    switch (d.kind) {
        case BinSigDecl::Fn: r.kind = SigDecl::Kind::Function; break;
        case BinSigDecl::Class: r.kind = SigDecl::Kind::Class; break;
        case BinSigDecl::Alias: r.kind = SigDecl::Kind::Alias; break;
        case BinSigDecl::Meta: r.kind = SigDecl::Kind::Meta; break;
    }
    r.name = d.name;
    r.templateParams = d.templateParams;
    for (const auto& [n, t] : d.params) r.params.push_back({n, binTypeToText(t)});
    if (d.kind == BinSigDecl::Fn || d.kind == BinSigDecl::Meta) r.returnType = binTypeToText(d.returnType);
    r.bases = d.bases;
    for (const auto& [n, t] : d.fields) r.fields.push_back({n, binTypeToText(t)});
    for (const auto& m : d.methods) r.methods.push_back(binDeclToText(m));
    if (d.kind == BinSigDecl::Alias) r.aliasTarget = binTypeToText(d.aliasTarget);
    return r;
}

std::string binDeclToString(const BinSigDecl& d) {
    return SigParser::declToString(binDeclToText(d));
}

} // namespace at
