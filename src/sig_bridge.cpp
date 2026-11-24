#include "sig_bridge.h"

#include <sstream>

namespace at {

namespace {

std::vector<std::string> templateNamesFor(const TypePtr& t) {
    std::vector<std::string> names;
    if (!t) return names;
    if (auto ft = std::dynamic_pointer_cast<FunctionType>(t)) names = ft->templateParamNames;
    // Arity is known but names were not recorded: synthesize stable ones.
    for (int i = (int)names.size(); i < t->templateParams; ++i) names.push_back("T" + std::to_string(i));
    return names;
}

BinSigType legacyType(const std::string& w) {
    if (w == "int" || w == "bool") return BinSigType::prim(BinSigType::PInt);
    if (w == "decimal") return BinSigType::prim(BinSigType::PDecimal);
    if (w == "str" || w == "string") return BinSigType::prim(BinSigType::PString);
    if (w == "ptr") return BinSigType::prim(BinSigType::PPtr);
    if (w == "void") return BinSigType::prim(BinSigType::PVoid);
    return BinSigType::named(w);
}

} // namespace

BinSigType typeToBinSig(const TypePtr& t) {
    if (!t) return BinSigType::prim(BinSigType::PUnknown);

    if (auto p = std::dynamic_pointer_cast<PrimitiveType>(t)) {
        switch (p->kind) {
            case PrimitiveType::Kind::Int:
            case PrimitiveType::Kind::Bool: return BinSigType::prim(BinSigType::PInt);
            case PrimitiveType::Kind::Decimal: return BinSigType::prim(BinSigType::PDecimal);
            case PrimitiveType::Kind::String: return BinSigType::prim(BinSigType::PString);
            case PrimitiveType::Kind::Void: return BinSigType::prim(BinSigType::PVoid);
            case PrimitiveType::Kind::Unknown: return BinSigType::prim(BinSigType::PUnknown);
        }
    }
    if (auto p = std::dynamic_pointer_cast<PointerType>(t)) return BinSigType::wrap(BinSigType::Ptr, typeToBinSig(p->pointee));
    if (auto r = std::dynamic_pointer_cast<ReferenceType>(t)) return BinSigType::wrap(BinSigType::Ref, typeToBinSig(r->referent));
    if (auto a = std::dynamic_pointer_cast<ArrayType>(t)) return BinSigType::wrap(BinSigType::Array, typeToBinSig(a->element));
    if (auto f = std::dynamic_pointer_cast<FunctionType>(t)) {
        BinSigType r;
        r.tag = BinSigType::Func;
        for (const auto& p : f->params) r.params.push_back(std::make_shared<BinSigType>(typeToBinSig(p)));
        r.returnType = std::make_shared<BinSigType>(typeToBinSig(f->returnType));
        return r;
    }
    if (auto a = std::dynamic_pointer_cast<AliasType>(t)) {
        if (!a->name.empty()) return BinSigType::named(a->name);
        return typeToBinSig(a->target);
    }
    return BinSigType::named(t->name.empty() ? "?" : t->name);
}

TypePtr binSigToType(const BinSigType& t) {
    switch (t.tag) {
        case BinSigType::Prim:
            switch (t.primKind) {
                case BinSigType::PInt: return makePrimitive(PrimitiveType::Kind::Int, "int");
                case BinSigType::PDecimal: return makePrimitive(PrimitiveType::Kind::Decimal, "decimal");
                case BinSigType::PString: return makePrimitive(PrimitiveType::Kind::String, "string");
                case BinSigType::PVoid: return makePrimitive(PrimitiveType::Kind::Void, "void");
                case BinSigType::PPtr: return makePointer(makePrimitive(PrimitiveType::Kind::Unknown, "unknown"));
                default: return makePrimitive(PrimitiveType::Kind::Unknown, "unknown");
            }
        case BinSigType::Named: {
            auto c = std::make_shared<ClassType>();
            c->name = t.name;
            return c;
        }
        case BinSigType::Func: {
            std::vector<TypePtr> params;
            for (const auto& p : t.params) params.push_back(binSigToType(*p));
            TypePtr ret = t.returnType ? binSigToType(*t.returnType) : makePrimitive(PrimitiveType::Kind::Void, "void");
            return makeFunction(std::move(params), std::move(ret));
        }
        case BinSigType::Array:
            return makeArray(t.elementType ? binSigToType(*t.elementType) : nullptr);
        case BinSigType::Ptr:
            return makePointer(t.elementType ? binSigToType(*t.elementType) : nullptr);
        case BinSigType::Ref:
            return makeReference(t.elementType ? binSigToType(*t.elementType) : nullptr, false);
        case BinSigType::Tmpl: {
            auto c = std::make_shared<ClassType>();
            c->name = t.name;
            return c;
        }
    }
    return makePrimitive(PrimitiveType::Kind::Unknown, "unknown");
}

bool symbolToBinSigDecl(const SymbolPtr& sym, BinSigDecl& out) {
    if (!sym || !sym->type) return false;
    out = BinSigDecl{};
    out.name = sym->name;
    out.templateParams = templateNamesFor(sym->type);

    if (auto f = std::dynamic_pointer_cast<FunctionType>(sym->type)) {
        out.kind = sym->type->templateParams > 0 ? BinSigDecl::Meta : BinSigDecl::Fn;
        for (size_t i = 0; i < f->params.size(); ++i) {
            std::string pname = i < f->paramNames.size() ? f->paramNames[i] : "";
            out.params.push_back({pname, std::make_shared<BinSigType>(typeToBinSig(f->params[i]))});
        }
        out.returnType = typeToBinSig(f->returnType);
        return true;
    }

    if (sym->kind != SymbolKind::Alias && sym->kind != SymbolKind::Class) return false;

    auto ct = std::dynamic_pointer_cast<ClassType>(sym->type);
    if (ct && (ct->name == "class" || ct->name == "template")) {
        // Anonymous class expression bound to a name: a class declaration.
        out.kind = BinSigDecl::Class;
        if (ct->scope) {
            auto* scope = static_cast<SymbolTable*>(ct->scope);
            for (const auto& m : scope->exportedSymbols()) {
                BinSigDecl md;
                if (m->type && std::dynamic_pointer_cast<FunctionType>(m->type) && symbolToBinSigDecl(m, md)) {
                    out.methods.push_back(md);
                } else {
                    out.fields.push_back({m->name, typeToBinSig(m->type)});
                }
            }
        }
        return true;
    }

    out.kind = BinSigDecl::Alias;
    out.aliasTarget = typeToBinSig(sym->type);
    return true;
}

std::vector<BinSigDecl> declsFromScope(const SymbolTable& scope) {
    std::vector<BinSigDecl> decls;
    for (const auto& sym : scope.exportedSymbols()) {
        BinSigDecl d;
        if (symbolToBinSigDecl(sym, d)) decls.push_back(std::move(d));
    }
    return decls;
}

SymbolPtr binSigDeclToSymbol(const BinSigDecl& d) {
    auto sym = std::make_shared<Symbol>();
    sym->name = d.name;
    sym->visibility = Visibility::Public;

    switch (d.kind) {
        case BinSigDecl::Fn:
        case BinSigDecl::Meta: {
            std::vector<TypePtr> params;
            std::vector<std::string> names;
            for (const auto& [n, t] : d.params) {
                params.push_back(binSigToType(*t));
                names.push_back(n);
            }
            auto ft = std::static_pointer_cast<FunctionType>(makeFunction(std::move(params), binSigToType(d.returnType)));
            ft->paramNames = std::move(names);
            ft->templateParamNames = d.templateParams;
            ft->templateParams = (int)d.templateParams.size();
            sym->kind = d.kind == BinSigDecl::Meta ? SymbolKind::Template : SymbolKind::Function;
            sym->type = ft;
            break;
        }
        case BinSigDecl::Class: {
            auto ct = std::make_shared<ClassType>();
            ct->name = d.name;
            ct->templateParams = (int)d.templateParams.size();
            sym->kind = SymbolKind::Class;
            sym->type = ct;
            break;
        }
        case BinSigDecl::Alias:
            // Aliases are transparent to the type checker: use the target type.
            sym->kind = SymbolKind::Alias;
            sym->type = binSigToType(d.aliasTarget);
            break;
    }
    return sym;
}

std::string elfSymbolNameFor(const BinSigDecl& d) {
    return d.kind == BinSigDecl::Meta ? d.name + "_meta" : d.name;
}

bool parseLegacySigLine(const std::string& line, BinSigDecl& out) {
    std::istringstream iss(line);
    std::string name;
    if (!(iss >> name)) return false;
    out = BinSigDecl{};
    out.kind = BinSigDecl::Fn;
    out.name = name;
    out.returnType = BinSigType::prim(BinSigType::PVoid);

    std::string w;
    bool afterArrow = false;
    while (iss >> w) {
        if (w == "->") { afterArrow = true; continue; }
        if (afterArrow) out.returnType = legacyType(w);
        else out.params.push_back({"", std::make_shared<BinSigType>(legacyType(w))});
    }
    return afterArrow;
}

} // namespace at
