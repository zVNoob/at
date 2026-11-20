#include "semantic.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>

namespace at {

static AstPtr findChild(const AstPtr& node, const std::string& kind) {
    if (!node) return nullptr;
    for (const auto& c : node->children) {
        if (c && c->kind == kind) return c;
    }
    return nullptr;
}

static bool isToken(const AstPtr& node, const std::string& text) {
    return node && node->text == text;
}

static std::vector<AstPtr> childrenOfKind(const AstPtr& node, const std::string& kind) {
    std::vector<AstPtr> result;
    if (!node) return result;
    for (const auto& c : node->children) {
        if (c && c->kind == kind) result.push_back(c);
    }
    return result;
}

static void collectIdentifiers(const AstPtr& node, std::vector<std::string>& names) {
    if (!node) return;
    if (node->kind == "IDENTIFIER") {
        names.push_back(node->text);
        return;
    }
    if (node->kind == "id_tuple_list") {
        for (const auto& c : node->children) collectIdentifiers(c, names);
        return;
    }
    for (const auto& c : node->children) collectIdentifiers(c, names);
}

SemanticAnalyzer::SemanticAnalyzer() {
    globalScope_ = std::make_shared<SymbolTable>();
    globalScope_->setParent(nullptr);
}

void SemanticAnalyzer::analyze(const AstPtr& root) {
    if (!root) return;
    currentScope_ = globalScope_.get();
    collect(root, globalScope_.get());
    check(root, globalScope_.get());
}

size_t SemanticAnalyzer::importSymbols(const std::vector<SymbolPtr>& symbols) {
    size_t n = 0;
    for (const auto& sym : symbols) {
        if (sym && globalScope_->insert(sym)) ++n;
    }
    return n;
}

size_t SemanticAnalyzer::importModule(const std::string& path, SymbolTable* scope) {
    SymbolTable* target = scope ? scope : globalScope_.get();
    ObjectImporter importer;
    if (!importer.importObjectFile(path)) return 0;
    // Import all symbols into the target scope.
    size_t n = 0;
    for (const auto& sym : importer.allSymbols()) {
        if (sym && target->insert(sym)) ++n;
    }
    return n;
}

void SemanticAnalyzer::pushScope(SymbolTable* scope) {
    if (scope) scope->setParent(currentScope_);
    currentScope_ = scope;
}

void SemanticAnalyzer::popScope() {
    if (currentScope_) currentScope_ = currentScope_->parent();
}

void SemanticAnalyzer::error(const AstPtr& node, const std::string& message) {
    SemanticError e;
    e.span = (node ? node->span : SourceSpan{});
    e.message = message;
    errors_.push_back(e);
}

Visibility SemanticAnalyzer::parseVisibility(const AstPtr& node) const {
    if (!node || node->children.empty()) return Visibility::Public;
    const std::string& t = node->children[0]->text;
    if (t == "^:") return Visibility::Protected;
    if (t == "~:") return Visibility::Private;
    if (t == "!:") return Visibility::ConstPublic;
    if (t == "!^:") return Visibility::ConstProtected;
    if (t == "!~:") return Visibility::ConstPrivate;
    return Visibility::Public;
}

TypePtr SemanticAnalyzer::primitiveFromName(const std::string& name) const {
    // No primitive type names exist in the language.
    // All types come from typeof(expr): `42`, `"hello"`, etc.
    // "int", "string", "bool", etc. are NOT valid type names.
    return nullptr;
}

TypePtr SemanticAnalyzer::functionTypeFromSig(const AstPtr& node, SymbolTable* scope) {
    std::vector<TypePtr> params;
    std::vector<std::string> names;
    TypePtr ret;
    bool afterArrow = false;
    for (const auto& c : node->children) {
        if (!c) continue;
        if (c->kind == "func_arg_declare_list") {
            for (const auto& p : c->children) {
                if (!p || p->kind != "func_arg_declare") continue;
                std::string pname;
                if (!p->children.empty() && p->children[0] && p->children[0]->kind == "IDENTIFIER") pname = p->children[0]->text;
                TypePtr pt;
                if (AstPtr tn = findChild(p, "type")) pt = resolveType(tn, scope);
                if (!pt) pt = makePrimitive(PrimitiveType::Kind::Unknown, "unknown");
                params.push_back(pt);
                names.push_back(pname);
            }
        } else if (c->kind == "ARROW") {
            afterArrow = true;
        } else if (afterArrow && !ret && (c->kind == "type" || c->kind == "base_type" || c->kind == "complex_type")) {
            ret = resolveType(c, scope);
        }
    }
    if (!ret) ret = makePrimitive(afterArrow ? PrimitiveType::Kind::Unknown : PrimitiveType::Kind::Void,
                                  afterArrow ? "unknown" : "void");
    auto ft = makeFunction(params, ret);
    if (auto f = std::dynamic_pointer_cast<FunctionType>(ft)) f->paramNames = names;
    return ft;
}

TypePtr SemanticAnalyzer::resolveType(const AstPtr& node, SymbolTable* scope) {
    if (!node) return nullptr;

    // Template declaration: <| T |> # (...) { } or function pointer
    if (node->kind == "anonymous_template") {
        // Function template
        bool isFunc = false;
        for (const auto& c : node->children) {
            if (c && c->text == "@") { isFunc = true; break; }
        }
        if (isFunc) {
            int tp = 0;
            AstPtr tpNode = findChild(node, "template_arg_declare_list");
            if (tpNode) for (const auto& p : tpNode->children) if (p && p->kind == "IDENTIFIER") ++tp;
            auto ft = makeFunction({}, makePrimitive(PrimitiveType::Kind::Unknown, "unknown"));
            ft->templateParams = tp;
            types_[node] = ft;
            return ft;
        }
        int tp = 0;
        AstPtr paramsNode = findChild(node, "template_arg_declare_list");
        if (paramsNode) {
            for (const auto& c : paramsNode->children) {
                if (c && c->kind == "IDENTIFIER") ++tp;
            }
        }
        TypePtr base = nullptr;
        for (const auto& c : node->children) {
            if (c && (c->kind == "class" || c->text == "#" || c->kind == "FUNPTR" || c->text == "@?" ||
                      c->kind == "expr" || c->kind == "type" || c->kind == "complex_type" || c->kind == "base_type")) {
                if (c->text == "#" || c->kind == "class") {
                    base = std::make_shared<ClassType>();
                    base->name = "class";
                } else if (c->text == "@?" || c->kind == "FUNPTR") {
                    base = makeFunction({}, makePrimitive(PrimitiveType::Kind::Unknown, "unknown"));
                } else {
                    base = resolveType(c, scope);
                }
                if (base) base->templateParams = tp;
            }
        }
        if (!base) {
            base = std::make_shared<ClassType>();
            base->name = "template";
            base->templateParams = tp;
        }
        types_[node] = base;
        return base;
    }

    // Template instantiation: X <| T1, T2 |>
    // Detect TEMPLATE_OPEN among children.
    {
        AstPtr open = nullptr;
        for (const auto& c : node->children) {
            if (c && c->kind == "TEMPLATE_OPEN") { open = c; } // last
        }
        if (open) {
            // base is first identifier/complex type before open
            AstPtr baseNode = nullptr;
            int openIndex = -1;
            for (size_t i = 0; i < node->children.size(); ++i) {
                if (node->children[i] == open) openIndex = (int)i;
            }
            if (openIndex > 0) {
                for (int i = openIndex - 1; i >= 0; --i) {
                    if (node->children[i] && (node->children[i]->kind == "IDENTIFIER" || node->children[i]->kind == "complex_type" ||
                                             node->children[i]->kind == "base_type" || node->children[i]->kind == "type" ||
                                             node->children[i]->kind == "anonymous_template")) {
                        baseNode = node->children[i];
                        break;
                    }
                }
            }
            TypePtr baseType = baseNode ? resolveType(baseNode, scope) : nullptr;
            // resolve args
            AstPtr argsNode = findChild(node, "template_arg_list");
            std::vector<TypePtr> args;
            if (argsNode) {
                // Lists are flattened by parseSource, so each type is a direct child.
                for (const auto& c : argsNode->children) {
                    if (c && (c->kind == "type" || c->kind == "base_type" || c->kind == "complex_type")) {
                        args.push_back(resolveType(c, scope));
                    }
                }
            }
            // arity check
            if (baseType && baseType->templateParams > 0 && (int)args.size() != baseType->templateParams) {
                error(node, "template '" + baseType->name + "' expects " + std::to_string(baseType->templateParams) +
                            " template argument(s), got " + std::to_string(args.size()));
            }
            // Also if base is a symbol with templateParams
            if (baseNode && baseNode->kind == "IDENTIFIER" && scope) {
                if (auto sym = scope->lookup(baseNode->text)) {
                    if (sym->type && sym->type->templateParams > 0 && (int)args.size() != sym->type->templateParams) {
                        error(node, "template '" + sym->name + "' expects " + std::to_string(sym->type->templateParams) +
                                    " template argument(s), got " + std::to_string(args.size()));
                    }
                }
            }
            if (baseType) {
                // Create an instantiated type alias
                auto t = std::make_shared<ClassType>();
                t->name = baseType->name;
                t->templateParams = 0;
                return t;
            }
            return nullptr;
        }
    }

    // Function pointer type with signature: @? ( args ) [-> ( type )]
    if (node->kind == "complex_type" && !node->children.empty() && node->children[0] &&
        (node->children[0]->kind == "FUNPTR" || node->children[0]->text == "@?")) {
        return functionTypeFromSig(node, scope);
    }

    const bool isTypeofNode = node->kind == "complex_type" && !node->children.empty() &&
                              node->children[0] && node->children[0]->text == "`";
    if (!isTypeofNode && (node->kind == "base_type" || node->kind == "type" || node->kind == "complex_type")) {
        for (const auto& c : node->children) {
            if (auto t = resolveType(c, scope)) return t;
        }
        return nullptr;
    }

    if (node->kind == "IDENTIFIER") {
        auto sit = typeSubstitution_.find(node->text);
        if (sit != typeSubstitution_.end()) return sit->second;
        if (auto prim = primitiveFromName(node->text)) return prim;
        // Native functions (at_*) are always declared
        if (node->text.rfind("at_", 0) == 0) {
            auto t = makePrimitive(PrimitiveType::Kind::Unknown, "unknown");
            types_[node] = t;
            return t;
        }
        if (scope) {
            if (auto sym = scope->lookup(node->text)) {

                if (sym->type) {
                    return sym->type;
                }
                if (sym->kind == SymbolKind::Class) {
                    auto t = std::make_shared<ClassType>();
                    t->name = sym->name;
                    return t;
                }
                if (sym->kind == SymbolKind::Alias) {
                    // Alias target not stored separately; return class type by name.
                    auto t = std::make_shared<ClassType>();
                    t->name = sym->name;
                    // propagate template params if alias target unknown; best effort
                    return t;
                }
            } else {
            }
        }
        auto t = std::make_shared<ClassType>();
        t->name = node->text;
        return t;
    }

    // Qualified type: base_type :: IDENTIFIER  (e.g. Class::member)
    {
        bool hasScope = false;
        for (const auto& c : node->children) {
            if (c && c->kind == "SCOPE") { hasScope = true; break; }
        }
        if (hasScope && node->children.size() >= 3) {
            // Resolve the base type (children[0])
            TypePtr baseType = resolveType(node->children[0], scope);
            if (!baseType) return nullptr;

            // Find the IDENTIFIER after :: (children[2])
            std::string memberName;
            for (size_t i = 0; i < node->children.size(); ++i) {
                if (node->children[i] && node->children[i]->kind == "IDENTIFIER") {
                    memberName = node->children[i]->text;
                    break;
                }
            }

            // If base is a class, look up the member in the class scope
            if (auto ct = std::dynamic_pointer_cast<ClassType>(baseType)) {
                if (!memberName.empty() && ct->scope) {
                    auto it = classScopes_.find(ct->scope);
                    if (it != classScopes_.end()) {
                        auto& clsScope = it->second;
                        if (auto sym = clsScope->lookup(memberName)) {
                            TypePtr memberType = sym->type ? sym->type : makePrimitive(PrimitiveType::Kind::Unknown, "unknown");
                            types_[node] = memberType;
                            return memberType;
                        }
                        error(node, "unknown member '" + memberName + "' in class '" + ct->name + "'");
                    }
                }
            }

            // Fallback: return the base type
            types_[node] = baseType;
            return baseType;
        }
    }

    // Pointer: '*' type
    if (node->text == "*" || node->kind == "CHAR(42)") {
        for (const auto& c : node->children) {
            if (auto t = resolveType(c, scope)) return makePointer(t);
        }
        // child may be type node
        for (const auto& c : node->children) {
            if (auto t = resolveType(c, scope)) return makePointer(t);
        }
    }

    // Anonymous template
    if (node->kind == "anonymous_template") {
        // Function template
        bool isFunc = false;
        for (const auto& c : node->children) {
            if (c && c->text == "@") { isFunc = true; break; }
        }
        if (isFunc) {
            int tp = 0;
            AstPtr tpNode = findChild(node, "template_arg_declare_list");
            if (tpNode) for (const auto& p : tpNode->children) if (p && p->kind == "IDENTIFIER") ++tp;
            auto ft = makeFunction({}, makePrimitive(PrimitiveType::Kind::Unknown, "unknown"));
            ft->templateParams = tp;
            types_[node] = ft;
            return ft;
        }
        // count template params
        AstPtr paramsNode = findChild(node, "template_arg_declare_list");
        int tp = 0;
        if (paramsNode) {
            for (const auto& c : paramsNode->children) {
                if (c && c->kind == "IDENTIFIER") ++tp;
            }
        }
        // find base class/function pointer
        TypePtr base = nullptr;
        for (const auto& c : node->children) {
            if (c && c->kind == "class") {
                base = std::make_shared<ClassType>();
                base->name = "class";
                base->templateParams = tp;
            } else if (c && c->kind == "FUNPTR") {
                base = makeFunction({}, makePrimitive(PrimitiveType::Kind::Unknown, "unknown"));
                base->templateParams = tp;
            } else if (c && (c->kind == "IDENTIFIER" || c->kind == "complex_type" || c->kind == "type" || c->kind == "base_type")) {
                base = resolveType(c, scope);
                if (base) base->templateParams = tp;
            }
        }
        if (base) return base;
        auto t = std::make_shared<ClassType>();
        t->name = "template";
        t->templateParams = tp;
        return t;
    }

    // Function pointer type
    if (node->kind == "FUNPTR" || node->text == "@?") {
        return makeFunction({}, makePrimitive(PrimitiveType::Kind::Unknown, "unknown"));
    }

    // Class expression (as type)
    if (node->text == "#") {
        auto t = std::make_shared<ClassType>();
        t->name = "class";
        return t;
    }

    // Typeof: `expr`
    if (node->text == "`" || (node->kind == "complex_type" && !node->children.empty() && node->children[0] && node->children[0]->text == "`")) {
        // Find the expr inside
        for (const auto& c : node->children) {
            if (c && c->kind == "expr") {
                TypePtr t = checkExpr(c, scope);
                if (t) return t;
            }
        }
        // If no expr child, try all children
        for (const auto& c : node->children) {
            if (c && c->kind != "CHAR(96)") {
                TypePtr t = resolveType(c, scope);
                if (t) return t;
            }
        }
    }

    // Try children
    for (const auto& c : node->children) {
        if (auto t = resolveType(c, scope)) return t;
    }
    return nullptr;
}

void SemanticAnalyzer::collect(const AstPtr& node, SymbolTable* scope) {
    if (!node) return;
    if (node->kind == "start") {
        for (const auto& c : node->children) collect(c, scope);
        return;
    }
    if (node->kind == "scope") {
        std::shared_ptr<SymbolTable> childScope;
        if (scope == globalScope_.get()) {
            childScope = globalScope_;
        } else {
            childScope = std::make_shared<SymbolTable>();
            childScope->setParent(scope);
        }
        scopes_[node] = childScope;
        for (const auto& c : node->children) collect(c, childScope.get());
        return;
    }
    if (node->kind != "stmt") return;
    if (node->children.empty()) return;

    const AstPtr& first = node->children[0];

    // Variable declaration: IDENTIFIER declare ...
    if (first && first->kind == "IDENTIFIER" && node->children.size() >= 2 &&
        node->children[1] && node->children[1]->kind == "declare") {
        const std::string name = first->text;
        auto sym = std::make_shared<Symbol>();
        sym->name = name;
        sym->kind = SymbolKind::Variable;
        sym->visibility = parseVisibility(node->children[1]);
        sym->isConst = (!node->children[1]->children.empty() && node->children[1]->children[0]->text.rfind("!", 0) == 0);
        sym->span = first->span;
        if (node->children.size() >= 3 && node->children[2] && node->children[2]->kind == "type") {
            sym->type = resolveType(node->children[2], scope);
        }
        if (!scope->insert(sym)) {
            error(first, "redefinition of '" + name + "'");
        } else {
            symbols_[first] = sym;
        }
        return;
    }

    // Tuple declaration: '(' id_tuple_list ')' declare tuple
    if (first && first->kind == "CHAR(40)" && node->children.size() >= 4) {
        // find id_tuple_list
        AstPtr idList = findChild(node, "id_tuple_list");
        std::vector<std::string> names;
        collectIdentifiers(idList, names);
        for (const auto& name : names) {
            auto sym = std::make_shared<Symbol>();
            sym->name = name;
            sym->kind = SymbolKind::Variable;
            // visibility from declare node
            AstPtr declNode = findChild(node, "declare");
            sym->visibility = declNode ? parseVisibility(declNode) : Visibility::Public;
            sym->type = makePrimitive(PrimitiveType::Kind::Unknown, "unknown");
            if (!scope->insert(sym)) error(first, "redefinition of '" + name + "'");
        }
        return;
    }

    // Alias: IDENTIFIER ALIAS_PUBLIC/PRIVATE type
    if (first && first->kind == "IDENTIFIER" && node->children.size() >= 3 &&
        node->children[1] && (node->children[1]->kind == "ALIAS_PUBLIC" || node->children[1]->kind == "ALIAS_PRIVATE")) {
        const std::string name = first->text;
        auto sym = std::make_shared<Symbol>();
        sym->name = name;
        sym->kind = SymbolKind::Alias;
        sym->visibility = (node->children[1]->kind == "ALIAS_PUBLIC") ? Visibility::Public : Visibility::Private;
        sym->type = resolveType(node->children[2], scope);
        if (!scope->insert(sym)) error(first, "redefinition of alias '" + name + "'");
        else symbols_[first] = sym;
        return;
    }

    // Block or nested scopes
    for (const auto& c : node->children) {
        if (c && c->kind == "scope") {
            auto childScope = std::make_shared<SymbolTable>();
            childScope->setParent(scope);
            collect(c, childScope.get());
        }
    }
}

void SemanticAnalyzer::check(const AstPtr& node, SymbolTable* scope) {
    if (!node) return;
    if (node->kind == "start") {
        for (const auto& c : node->children) check(c, scope);
        return;
    }
    if (node->kind == "scope") {
        std::shared_ptr<SymbolTable> childScope;
        auto it = scopes_.find(node);
        if (it != scopes_.end()) childScope = it->second;
        else {
            childScope = std::make_shared<SymbolTable>();
            childScope->setParent(scope);
            scopes_[node] = childScope;
        }
        if (childScope.get() != scope) pushScope(childScope.get());
        for (const auto& c : node->children) check(c, childScope.get());
        if (childScope.get() != scope) popScope();
        return;
    }
    if (node->kind != "stmt") {
        if (node->kind == "expr") checkExpr(node, scope);
        return;
    }
    if (node->children.empty()) return;

    const AstPtr& first = node->children[0];

    // Variable declaration
    if (first && first->kind == "IDENTIFIER" && node->children.size() >= 2 &&
        node->children[1] && node->children[1]->kind == "declare") {
        SymbolPtr sym = (symbols_.count(first) ? symbols_[first] : scope->lookup(first->text));
        if (node->children.size() >= 3 && node->children[2] && node->children[2]->kind == "type") {
            TypePtr t = resolveType(node->children[2], scope);
            types_[node->children[2]] = t;
            if (sym) sym->type = t;
        }
        // memory pinning: IDENTIFIER declare type '@' INTEGER
        for (size_t i = 0; i < node->children.size(); ++i) {
            if (node->children[i] && node->children[i]->kind == "CHAR(64)" && i + 1 < node->children.size()) {
                // @ INTEGER
                if (node->children[i+1] && node->children[i+1]->kind != "INTEGER") {
                    error(node->children[i+1], "memory pinning requires an integer address");
                }
            }
        }
        // initializer
        for (size_t i = 0; i < node->children.size(); ++i) {
            if (node->children[i] && node->children[i]->kind == "CHAR(61)" && i + 1 < node->children.size()) {
                TypePtr initType = checkExpr(node->children[i+1], scope);
                if (sym && !sym->type) sym->type = initType;
                if (sym && sym->type && initType && !typesCompatible(sym->type, initType)) {
                    error(node->children[i+1], "cannot initialize '" + sym->name + "' of type '" + sym->type->name +
                                                "' with '" + initType->name + "'");
                }
            }
        }
        if (sym && !sym->type) sym->type = makePrimitive(PrimitiveType::Kind::Unknown, "unknown");
        return;
    }

    // Tuple declaration
    if (first && first->kind == "CHAR(40)") {
        // check tuple exprs
        // find tuple node
        AstPtr tupleNode = nullptr;
        for (const auto& c : node->children) {
            if (c && c->kind == "tuple") { tupleNode = c; break; }
        }
        if (tupleNode) {
            for (const auto& c : tupleNode->children) {
                if (c && c->kind == "expr") checkExpr(c, scope);
            }
        }
        return;
    }

    // Alias
    if (first && first->kind == "IDENTIFIER" && node->children.size() >= 3 &&
        node->children[1] && (node->children[1]->kind == "ALIAS_PUBLIC" || node->children[1]->kind == "ALIAS_PRIVATE")) {
        TypePtr t = nullptr;
        // Unwrap type -> base_type -> complex_type -> class
        AstPtr classNode = node->children[2];
        while (classNode && (classNode->kind == "type" || classNode->kind == "base_type" || classNode->kind == "complex_type") &&
               classNode->children.size() == 1) {
            classNode = classNode->children[0];
        }
        if (classNode && classNode->kind != "class") classNode = nullptr;
        if (node->children[2] && node->children[2]->kind == "anonymous_template") {
            t = checkExpr(node->children[2], scope);
        } else if (classNode) {
            // Class bodies need a full check (builds the class scope with its members).
            t = checkExpr(classNode, scope);
        } else {
            t = resolveType(node->children[2], scope);
        }
        types_[node->children[2]] = t;
        if (symbols_.count(first) && symbols_[first]) symbols_[first]->type = t;
        return;
    }

    // Block
    if (first && first->kind == "CHAR(123)") {
        for (const auto& c : node->children) check(c, scope);
        return;
    }

    // If statement: expr '?' '{' scope '}' (':' '{' scope '}')?
    if (node->children.size() >= 2 && node->children[1] && node->children[1]->kind == "CHAR(63)") {
        TypePtr ct = checkExpr(node->children[0], scope);
        if (ct && ct->name != "bool" && ct->name != "unknown" && ct->name != "int") {
            error(node->children[0], "if condition must be boolean");
        }
        for (const auto& c : node->children) {
            if (c && c->kind == "scope") {
                auto childScope = std::make_shared<SymbolTable>();
                childScope->setParent(scope);
                check(c, childScope.get());
            }
        }
        return;
    }

    // While/for
    if (first && first->kind == "CHAR(36)") {
        // While: $ ( expr ) { scope }
        // For: $ ( IDENTIFIER : expr ) { scope }
        bool isFor = false;
        for (const auto& c : node->children) {
            if (c && c->kind == "IDENTIFIER") { isFor = true; break; } // crude
        }
        // Actually for has IDENTIFIER before ':'
        // Check exprs
        for (const auto& c : node->children) {
            if (c && c->kind == "expr") checkExpr(c, scope);
        }
        // scopes
        loopDepth_++;
        if (isFor) {
            // find identifier and expr
            AstPtr idNode = nullptr;
            AstPtr exprNode = nullptr;
            for (const auto& c : node->children) {
                if (c && c->kind == "IDENTIFIER") idNode = c;
                if (c && c->kind == "expr") exprNode = c;
            }
            auto childScope = std::make_shared<SymbolTable>();
            childScope->setParent(scope);
            if (idNode) {
                auto sym = std::make_shared<Symbol>();
                sym->name = idNode->text;
                sym->kind = SymbolKind::Variable;
                if (exprNode) {
                    TypePtr t = typeOf(exprNode);
                    if (t) sym->type = t;
                }
                if (!sym->type) sym->type = makePrimitive(PrimitiveType::Kind::Unknown, "unknown");
                childScope->insert(sym);
                symbols_[idNode] = sym;
            }
            pushScope(childScope.get());
            for (const auto& c : node->children) {
                if (c && c->kind == "scope") check(c, childScope.get());
            }
            popScope();
        } else {
            for (const auto& c : node->children) {
                if (c && c->kind == "scope") {
                    auto childScope = std::make_shared<SymbolTable>();
                    childScope->setParent(scope);
                    check(c, childScope.get());
                }
            }
        }
        loopDepth_--;
        return;
    }

    // Break/continue
    if (first && (first->kind == "CONTINUE" || first->kind == "BREAK")) {
        if (loopDepth_ == 0) error(first, std::string(first->kind == "CONTINUE" ? "continue" : "break") + " outside loop");
        return;
    }

    // Return
    if (first && first->kind == "RETURN") {
        if (!inFunction_) error(first, "return outside function");
        if (node->children.size() >= 2) {
            TypePtr rt = checkExpr(node->children[1], scope);
            if (inferringReturn_) {
                if (!inferredReturnType_ && rt && rt->name != "unknown") {
                    inferredReturnType_ = rt;
                } else if (inferredReturnType_ && rt && rt->name != "unknown" &&
                           !typesCompatible(inferredReturnType_, rt)) {
                    // Inconsistent return types -> fall back to unknown.
                    inferredReturnType_ = makePrimitive(PrimitiveType::Kind::Unknown, "unknown");
                }
            }
            if (currentReturnType_ && rt && !typesCompatible(currentReturnType_, rt)) {
                error(node->children[1], "return type mismatch: expected '" + currentReturnType_->name + "', got '" + rt->name + "'");
            }
        } else {
            // bare `@<` with no value -> void
            if (inferringReturn_ && !inferredReturnType_) {
                inferredReturnType_ = makePrimitive(PrimitiveType::Kind::Void, "void");
            }
        }
        return;
    }

    // Unwrap: '*' '(' expr_list ')' '{' scope '}' (':' '{' scope '}')?
    if (first && first->kind == "CHAR(42)" && node->children.size() >= 2 && node->children[1] && node->children[1]->kind == "CHAR(40)") {
        for (const auto& c : node->children) {
            if (c && c->kind == "expr") checkExpr(c, scope);
            if (c && c->kind == "scope") {
                auto childScope = std::make_shared<SymbolTable>();
                childScope->setParent(scope);
                check(c, childScope.get());
            }
        }
        return;
    }

    // Try cast: '?' '(' cast_stmt_list ')' '{' scope '}' (':' '{' scope '}')?
    if (first && first->kind == "CHAR(63)" && node->children.size() >= 2 && node->children[1] && node->children[1]->kind == "CHAR(40)") {
        // check cast stmt exprs
        AstPtr castList = findChild(node, "cast_stmt_list");
        if (castList) {
            for (const auto& c : castList->children) {
                if (c && c->kind == "cast_stmt") {
                    for (const auto& cc : c->children) {
                        if (cc && cc->kind == "expr") checkExpr(cc, scope);
                    }
                }
            }
        }
        for (const auto& c : node->children) {
            if (c && c->kind == "scope") {
                auto childScope = std::make_shared<SymbolTable>();
                childScope->setParent(scope);
                check(c, childScope.get());
            }
        }
        return;
    }

    // Overloading: expr "^=" expr or OP "^=" expr
    {
        bool hasOverload = false;
        for (const auto& c : node->children) {
            if (c && c->kind == "OVERLOAD") { hasOverload = true; break; }
        }
        if (hasOverload) {
            for (const auto& c : node->children) {
                if (c && c->kind == "expr") checkExpr(c, scope);
            }
            return;
        }
    }

    // Expression statement or assignment
    // assignment: expr '=' expr
    if (node->children.size() == 3 && node->children[1] && node->children[1]->kind == "CHAR(61)") {
        // Auto-declare if target is an undeclared identifier
        // The LHS might be an expr wrapping an IDENTIFIER
        AstPtr lhsNode = node->children[0];
        if (lhsNode && lhsNode->kind == "expr" && lhsNode->children.size() == 1 && lhsNode->children[0] && lhsNode->children[0]->kind == "IDENTIFIER") {
            lhsNode = lhsNode->children[0];
        }
        if (lhsNode && lhsNode->kind == "IDENTIFIER") {
            const std::string name = lhsNode->text;
            if (!scope->lookup(name)) {
                auto sym = std::make_shared<Symbol>();
                sym->name = name;
                sym->kind = SymbolKind::Variable;
                sym->type = nullptr;
                scope->insert(sym);
                symbols_[node->children[0]] = sym;
            }
        }
        TypePtr lt = checkExpr(node->children[0], scope);
        TypePtr rt = checkExpr(node->children[2], scope);
        if (lt && rt && !typesCompatible(lt, rt)) {
            // Allow type inference: if target type is unknown, set it
            if (lt && lt->name == "unknown") {
                // find the symbol and update type
                AstPtr idNode = node->children[0];
                if (idNode && idNode->kind == "expr" && idNode->children.size() == 1 && idNode->children[0] && idNode->children[0]->kind == "IDENTIFIER") {
                    idNode = idNode->children[0];
                }
                if (idNode && idNode->kind == "IDENTIFIER") {
                    if (auto sym = scope->lookup(idNode->text)) {
                        sym->type = rt;
                        types_[idNode] = rt;
                    }
                }
            } else {
                error(node->children[2], "type mismatch in assignment: '" + lt->name + "' vs '" + rt->name + "'");
            }
        }
        return;
    }
    // tuple assignment: tuple '=' tuple
    // default
    for (const auto& c : node->children) {
        if (c && c->kind == "expr") checkExpr(c, scope);
        if (c && c->kind == "scope") {
            auto childScope = std::make_shared<SymbolTable>();
            childScope->setParent(scope);
            check(c, childScope.get());
        }
    }
}

TypePtr SemanticAnalyzer::checkExpr(const AstPtr& node, SymbolTable* scope) {
    if (!node) return nullptr;
    if (node->kind == "expr" && node->children.size() == 1) {
        return checkExpr(node->children[0], scope);
    }
    if (node->kind == "INTEGER") {
        auto t = makePrimitive(PrimitiveType::Kind::Int, "int");
        types_[node] = t;
        return t;
    }
    if (node->kind == "DECIMAL") {
        auto t = makePrimitive(PrimitiveType::Kind::Decimal, "decimal");
        types_[node] = t;
        return t;
    }
    if (node->kind == "STRING") {
        auto t = makePrimitive(PrimitiveType::Kind::String, "string");
        types_[node] = t;
        return t;
    }
    if (node->kind == "IMPORT") {
        // Import symbols from the ELF object file into the current scope.
        // The token text is the module path (e.g. 'module.o' or 'module.so').
        std::string path = node->text;
        if (path.size() >= 2 && (path.front() == '"' || path.front() == '\'')) {
            path = path.substr(1, path.size() - 2);
        }
        if (!path.empty() && (path.rfind(".o") == path.size() - 2 || path.rfind(".so") == path.size() - 3)) {
            importModule(path, scope);
        }
        // An import expression evaluates to a module object.
        auto t = makePrimitive(PrimitiveType::Kind::String, "module");
        types_[node] = t;
        return t;
    }
    if (node->kind == "IDENTIFIER") {
        // Native functions (at_*) are always declared; use an imported
        // signature when one is available, otherwise treat as untyped.
        if (node->text.rfind("at_", 0) == 0) {
            SymbolPtr sym = scope ? scope->lookup(node->text) : nullptr;
            if (sym && sym->type) {
                symbols_[node] = sym;
                types_[node] = sym->type;
                return sym->type;
            }
            auto t = makePrimitive(PrimitiveType::Kind::Unknown, "unknown");
            types_[node] = t;
            return t;
        }
        if (scope) {
            if (auto sym = scope->lookup(node->text)) {
                symbols_[node] = sym;
                TypePtr t = sym->type ? sym->type : makePrimitive(PrimitiveType::Kind::Unknown, "unknown");
                types_[node] = t;
                return t;
            }
        }
        error(node, "undeclared identifier '" + node->text + "'");
        auto t = makePrimitive(PrimitiveType::Kind::Unknown, "unknown");
        types_[node] = t;
        return t;
    }

    // Binary operators (3 children, middle is operator token)
    if (node->children.size() == 3 && node->children[1]) {
        const std::string op = node->children[1]->text;
        // Only treat as binary if middle is an operator token, not something else.
        bool isOp = (op == "+" || op == "-" || op == "*" || op == "/" || op == "%" || op == "^" || op == "|" ||
                     op == "&" || op == "?" || op == "<" || op == ">" || op == "<=" || op == ">=" || op == "==" ||
                     op == "!=" || op == "||" || op == "&&" || op == "^^");
        // '?' ternary handled separately; but if middle is '?' and there is ':' later, ternary.
        if (isOp) {
            TypePtr lt = checkExpr(node->children[0], scope);
            TypePtr rt = checkExpr(node->children[2], scope);
            TypePtr result = nullptr;
            bool isArith = (op == "+" || op == "-" || op == "*" || op == "/" || op == "%");
            if (isArith && lt && rt && lt->name != "unknown" && rt->name != "unknown" &&
                rt->name != "function" && lt->name != "function") {
                bool numeric = (lt->name == "int" && rt->name == "int") || (lt->name == "decimal" && rt->name == "decimal");
                if (!numeric && lt->name != rt->name) {
                    error(node, "no operator '" + op + "' for '" + lt->name + "' and '" + rt->name + "'");
                }
            }
            if (op == "==" || op == "!=" || op == "<" || op == ">" || op == "<=" || op == ">=") {
                result = makePrimitive(PrimitiveType::Kind::Bool, "bool");
            } else if (lt && rt && lt->name == "int" && rt->name == "int") {
                result = makePrimitive(PrimitiveType::Kind::Int, "int");
            } else if (lt && lt->name == "decimal") {
                result = makePrimitive(PrimitiveType::Kind::Decimal, "decimal");
            } else if (lt && rt && lt->name == rt->name) {
                result = lt;
            } else {
                result = lt ? lt : makePrimitive(PrimitiveType::Kind::Unknown, "unknown");
            }
            types_[node] = result;
            return result;
        }
    }

    // Ternary ? : (children contain '?' and ':')
    {
        int q = -1, c = -1;
        for (size_t i = 0; i < node->children.size(); ++i) {
            if (node->children[i] && node->children[i]->text == "?") q = (int)i;
            if (node->children[i] && node->children[i]->text == ":") c = (int)i;
        }
        if (q >= 0 && c > q) {
            if (q > 0) checkExpr(node->children[q - 1], scope);
            TypePtr t1 = nullptr, t2 = nullptr;
            if (q + 1 < (int)node->children.size() && node->children[q+1]->kind != "CHAR(123)") {
                t1 = checkExpr(node->children[q + 1], scope);
            }
            if (c + 1 < (int)node->children.size()) {
                t2 = checkExpr(node->children[c + 1], scope);
            }
            TypePtr res = t1 ? t1 : (t2 ? t2 : makePrimitive(PrimitiveType::Kind::Unknown, "unknown"));
            types_[node] = res;
            return res;
        }
    }

    // Unary
    if (node->children.size() == 2 && node->children[0] &&
        (node->children[0]->text == "-" || node->children[0]->text == "!")) {
        TypePtr t = checkExpr(node->children[1], scope);
        types_[node] = t ? t : makePrimitive(PrimitiveType::Kind::Unknown, "unknown");
        return types_[node];
    }

    // Borrow &
    if (!node->children.empty() && node->children[0] && node->children[0]->text == "&") {
        if (node->children.size() >= 2) {
            TypePtr t = checkExpr(node->children[1], scope);
            auto r = makeReference(t ? t : makePrimitive(PrimitiveType::Kind::Unknown, "unknown"), true);
            types_[node] = r;
            return r;
        }
    }
    // Borrow & expr where '&' is the node itself? (leaf)
    if (node->kind == "CHAR(38)") {
        // & is a leaf token; parent handles.
    }

    // Cast expr -> type (but not function literals: @ (...) -> (T) {...})
    {
        bool isFuncLit = (!node->children.empty() && node->children[0] && node->children[0]->text == "@");
        int arrow = -1;
        for (size_t i = 0; i < node->children.size(); ++i) {
            if (node->children[i] && node->children[i]->kind == "ARROW") { arrow = (int)i; break; }
        }
        if (!isFuncLit && arrow >= 1 && arrow + 1 < (int)node->children.size()) {
            TypePtr t = checkExpr(node->children[arrow - 1], scope);
            (void)t;
            TypePtr target = resolveType(node->children[arrow + 1], scope);
            if (!target) target = makePrimitive(PrimitiveType::Kind::Unknown, "unknown");
            types_[node] = target;
            return target;
        }
    }

    // Self function (@$): refers to the current function (for recursion)
    if (node->kind == "SELF_FUNC") {
        TypePtr t = currentFunctionType_;
        if (!t) {
            t = makeFunction({makePrimitive(PrimitiveType::Kind::Unknown, "unknown")},
                            makePrimitive(PrimitiveType::Kind::Unknown, "unknown"));
        }
        types_[node] = t;
        return t;
    }
    // Self class (#$): refers to self
    if (node->kind == "SELF_CLASS") {
        auto t = makePrimitive(PrimitiveType::Kind::Unknown, "unknown");
        types_[node] = t;
        return t;
    }

    // Function expression
    if (!node->children.empty() && node->children[0] && node->children[0]->text == "@") {
        auto childScope = std::make_shared<SymbolTable>();
        childScope->setParent(scope);
        pushScope(childScope.get());

        std::vector<TypePtr> paramTypes;
        std::vector<std::string> paramNames;
        AstPtr paramsNode = findChild(node, "func_arg_declare_list");
        if (paramsNode) {
            for (const auto& p : paramsNode->children) {
                if (!p || p->kind != "func_arg_declare") continue;
                // name
                std::string pname;
                if (!p->children.empty() && p->children[0] && p->children[0]->kind == "IDENTIFIER") pname = p->children[0]->text;
                paramNames.push_back(pname);
                TypePtr ptype = nullptr;
                // explicit type
                AstPtr typeNode = findChild(p, "type");
                if (typeNode) ptype = resolveType(typeNode, childScope.get());
                // default expr
                for (const auto& c : p->children) {
                    if (c && c->kind == "expr") {
                        TypePtr dt = checkExpr(c, childScope.get());
                        if (!ptype) ptype = dt;
                    }
                }
                if (!ptype) ptype = makePrimitive(PrimitiveType::Kind::Unknown, "unknown");
                paramTypes.push_back(ptype);
                if (!pname.empty()) {
                    auto sym = std::make_shared<Symbol>();
                    sym->name = pname;
                    sym->kind = SymbolKind::Variable;
                    sym->type = ptype;
                    childScope->insert(sym);
                }
            }
        }

        bool hasArrow = false;
        for (const auto& c : node->children) {
            if (c && c->kind == "ARROW") { hasArrow = true; break; }
        }

        TypePtr returnType = nullptr;
        // arrow
        for (size_t i = 0; i < node->children.size(); ++i) {
            if (node->children[i] && node->children[i]->kind == "ARROW") {
                if (i + 1 < node->children.size()) {
                    // type may be in parentheses: '(' type ')'
                    for (size_t j = i + 1; j < node->children.size(); ++j) {
                        if (node->children[j] && (node->children[j]->kind == "type" || node->children[j]->kind == "base_type" || node->children[j]->kind == "complex_type")) {
                            returnType = resolveType(node->children[j], childScope.get());
                            break;
                        }
                    }
                }
                break;
            }
        }
        if (!returnType) returnType = makePrimitive(PrimitiveType::Kind::Unknown, "unknown");

        TypePtr oldReturn = currentReturnType_;
        bool oldInFunc = inFunction_;
        bool oldInferring = inferringReturn_;
        TypePtr oldInferred = inferredReturnType_;
        TypePtr oldFunctionType = currentFunctionType_;

        // Create function type early so SELF_FUNC (@$) resolves
        auto ftEarly = makeFunction(paramTypes, returnType);
        currentFunctionType_ = ftEarly;

        bool inferReturn = !hasArrow;
        if (inferReturn) {
            inferredReturnType_ = nullptr;
            inferringReturn_ = true;
            currentReturnType_ = makePrimitive(PrimitiveType::Kind::Unknown, "unknown");
        } else {
            currentReturnType_ = returnType;
        }
        inFunction_ = true;

        for (const auto& c : node->children) {
            if (c && c->kind == "scope") check(c, childScope.get());
        }

        if (inferReturn && inferredReturnType_) {
            returnType = inferredReturnType_;
        } else if (inferReturn && !inferredReturnType_ && !paramTypes.empty()) {
            // Param-dependent return inference: if all concrete param types agree,
            // propagate that type as the inferred return type.
            TypePtr common = nullptr;
            bool consistent = true;
            for (const auto& pt : paramTypes) {
                if (!pt || pt->name == "unknown") continue;
                if (!common) common = pt;
                else if (!typesCompatible(common, pt)) { consistent = false; break; }
            }
            if (consistent && common) returnType = common;
        }

        currentReturnType_ = oldReturn;
        inFunction_ = oldInFunc;
        inferringReturn_ = oldInferring;
        inferredReturnType_ = oldInferred;
        currentFunctionType_ = oldFunctionType;
        popScope();

        auto ft = makeFunction(paramTypes, returnType);
        if (auto ftf = std::dynamic_pointer_cast<FunctionType>(ft)) {
            ftf->paramNames = paramNames;
            for (const auto& c : node->children) {
                if (c && c->kind == "scope") ftf->body = c.get();
            }
        }
        types_[node] = ft;
        return ft;
    }

    // Class expression
    if (!node->children.empty() && node->children[0] && node->children[0]->text == "#") {
        auto childScope = std::make_shared<SymbolTable>();
        childScope->setParent(scope);
        pushScope(childScope.get());

        // constructor params
        AstPtr paramsNode = findChild(node, "func_arg_declare_list");
        if (paramsNode) {
            for (const auto& p : paramsNode->children) {
                if (!p || p->kind != "func_arg_declare") continue;
                std::string pname;
                if (!p->children.empty() && p->children[0] && p->children[0]->kind == "IDENTIFIER") pname = p->children[0]->text;
                TypePtr ptype = nullptr;
                AstPtr typeNode = findChild(p, "type");
                if (typeNode) ptype = resolveType(typeNode, childScope.get());
                for (const auto& c : p->children) {
                    if (c && c->kind == "expr") {
                        TypePtr dt = checkExpr(c, childScope.get());
                        if (!ptype) ptype = dt;
                    }
                }
                if (!ptype) ptype = makePrimitive(PrimitiveType::Kind::Unknown, "unknown");
                if (!pname.empty()) {
                    auto sym = std::make_shared<Symbol>();
                    sym->name = pname;
                    sym->kind = SymbolKind::Variable;
                    // Constructor params are NOT auto-members: they live in the
                    // constructor scope but are not exported as class fields.
                    // Users must declare members explicitly, e.g. `a : a;`.
                    sym->visibility = Visibility::Private;
                    sym->type = ptype;
                    childScope->insert(sym);
                }
            }
        }
        // bases: type_list after ':'
        for (const auto& c : node->children) {
            if (c && (c->kind == "type" || c->kind == "base_type" || c->kind == "complex_type")) {
                resolveType(c, childScope.get());
            }
        }
        // body: the constructor. `self` is bound at runtime; members are
        // added dynamically (self.x = ...), so it is untyped here.
        {
            auto selfSym = std::make_shared<Symbol>();
            selfSym->name = "self";
            selfSym->kind = SymbolKind::Variable;
            selfSym->visibility = Visibility::Private; // not part of the class signature
            selfSym->type = makePrimitive(PrimitiveType::Kind::Unknown, "unknown");
            childScope->insert(selfSym);
        }
        TypePtr oldReturn = currentReturnType_;
        bool oldInFunc = inFunction_;
        currentReturnType_ = makePrimitive(PrimitiveType::Kind::Unknown, "unknown");
        inFunction_ = true;
        for (const auto& c : node->children) {
            if (c && c->kind == "scope") check(c, childScope.get());
        }
        currentReturnType_ = oldReturn;
        inFunction_ = oldInFunc;
        popScope();

        auto ct = std::make_shared<ClassType>();
        ct->name = "class";
        ct->scope = childScope.get();
        classScopes_[ct->scope] = childScope;
        types_[node] = ct;
        return ct;
    }

    // Anonymous template wrapper: template params around class/function
    // Call: expr '(' expr_list ')'
    {
        // detect '(' token
        bool hasCall = false;
        for (const auto& c : node->children) {
            if (c && c->text == "(") { hasCall = true; break; }
        }
        if (hasCall && node->children.size() >= 2 && node->kind != "anonymous_template") {
            // callee is first expr
            AstPtr calleeNode = nullptr;
            for (const auto& c : node->children) {
                if (c && c->kind == "expr") { calleeNode = c; break; }
            }
            TypePtr calleeType = calleeNode ? checkExpr(calleeNode, scope) : nullptr;
            // args
            AstPtr argsNode = findChild(node, "expr_list");
            std::vector<TypePtr> argTypes;
            if (argsNode) {
                for (const auto& c : argsNode->children) {
                    if (c && c->kind == "expr") argTypes.push_back(checkExpr(c, scope));
                }
            }
            TypePtr result = makePrimitive(PrimitiveType::Kind::Unknown, "unknown");

            if (calleeType && (calleeType->name == "function" || (calleeNode && calleeNode->children.size() == 1 && calleeNode->children[0] && calleeNode->children[0]->kind == "IDENTIFIER" && calleeNode->children[0]->text.rfind("at_", 0) == 0))) {
                auto ft = std::dynamic_pointer_cast<FunctionType>(calleeType);
                if (ft) {
                    // Template instantiation / constraint checking.
                    if (ft->templateParams > 0 && !ft->templateParamNames.empty()) {
                        // Deduce template args from function params.
                        std::map<std::string, TypePtr> subst;
                        size_t n = std::min(ft->params.size(), argTypes.size());
                        for (size_t i = 0; i < n; ++i) {
                            TypePtr pt = ft->params[i];
                            // If param type name is a template param name, map it.
                            if (pt) {
                                for (const auto& tpname : ft->templateParamNames) {
                                    if (pt->name == tpname) subst[tpname] = argTypes[i];
                                }
                            }
                        }
                        // Re-check body with substitution.
                        if (ft->body && !subst.empty()) {
                            size_t before = errors_.size();
                            auto savedSubst = typeSubstitution_;
                            typeSubstitution_ = subst;
                            auto childScope = std::make_shared<SymbolTable>();
                            childScope->setParent(scope);
                            // Insert params with substituted types.
                            for (size_t i = 0; i < ft->paramNames.size() && i < ft->params.size(); ++i) {
                                auto sym = std::make_shared<Symbol>();
                                sym->name = ft->paramNames[i];
                                sym->kind = SymbolKind::Variable;
                                TypePtr pt = ft->params[i];
                                if (pt) {
                                    auto it = subst.find(pt->name);
                                    if (it != subst.end()) pt = it->second;
                                }
                                sym->type = pt ? pt : makePrimitive(PrimitiveType::Kind::Unknown, "unknown");
                                childScope->insert(sym);
                            }
                            pushScope(childScope.get());
                            TypePtr oldReturn = currentReturnType_;
                            bool oldInFunc = inFunction_;
                            currentReturnType_ = ft->returnType;
                            inFunction_ = true;
                            // Substitute return type too
                            AstPtr bodyNode(static_cast<AstNode*>(ft->body), [](AstNode*) {});
                            check(bodyNode, childScope.get());
                            currentReturnType_ = oldReturn;
                            inFunction_ = oldInFunc;
                            popScope();
                            typeSubstitution_ = savedSubst;
                            // Report any new errors at use location.
                            if (errors_.size() > before) {
                                // Collect new errors
                                std::vector<SemanticError> newErrors(errors_.begin() + before, errors_.end());
                                errors_.resize(before);
                                for (const auto& e : newErrors) {
                                    SemanticError ne;
                                    ne.span = node->span; // use location
                                    ne.message = "constraint not satisfied: " + e.message;
                                    errors_.push_back(ne);
                                }
                            }
                        }
                    }
                    if (ft->params.size() != argTypes.size()) {
                        error(node, "function expects " + std::to_string(ft->params.size()) + " argument(s), got " + std::to_string(argTypes.size()));
                    } else {
                        for (size_t i = 0; i < argTypes.size(); ++i) {
                            const TypePtr& pt = ft->params[i];
                            if (!pt || !argTypes[i]) continue;
                            bool isTemplateParam = false;
                            for (const auto& tp : ft->templateParamNames) if (pt->name == tp) isTemplateParam = true;
                            if (isTemplateParam) continue;
                            if (!typesCompatible(pt, argTypes[i])) {
                                error(node, "argument " + std::to_string(i + 1) + ": expected '" + pt->name +
                                            "', got '" + argTypes[i]->name + "'");
                            }
                        }
                    }
                    result = ft->returnType ? ft->returnType : makePrimitive(PrimitiveType::Kind::Unknown, "unknown");
                }
            } else if (calleeType && !(calleeNode && calleeNode->children.size() == 1 && calleeNode->children[0] && calleeNode->children[0]->kind == "IDENTIFIER" && calleeNode->children[0]->text.rfind("at_", 0) == 0)) {
                // Check if it's a method call (dot expression) or a variable
                bool isMethodCall = false;
                bool isVariable = false;
                if (calleeNode) {
                    for (const auto& c : calleeNode->children) {
                        if (c && c->text == ".") { isMethodCall = true; break; }
                    }
                    if (calleeNode->kind == "IDENTIFIER" || (calleeNode->children.size() == 1 && calleeNode->children[0] && calleeNode->children[0]->kind == "IDENTIFIER")) {
                        isVariable = true;
                    }
                }
                if (!isMethodCall && !isVariable) {
                    // Allow calls on function-typed, unknown, or primitive callees
                    // (primitives may be callable in some contexts; suppress false positives)
                }

            }
            types_[node] = result;
            return result;
        }
    }

    // Dot: expr '.' IDENTIFIER
    {
        // find '.' token
        bool hasDot = false;
        for (const auto& c : node->children) {
            if (c && c->text == ".") { hasDot = true; break; }
        }
        if (hasDot) {
            AstPtr baseNode = nullptr;
            AstPtr memberNode = nullptr;
            for (const auto& c : node->children) {
                if (c && c->kind == "expr") { if (!baseNode) baseNode = c; }
                if (c && c->kind == "IDENTIFIER") memberNode = c;
            }
            if (baseNode) {
                TypePtr baseType = checkExpr(baseNode, scope);
                if (auto ct = std::dynamic_pointer_cast<ClassType>(baseType)) {
                    if (ct->scope) {
                        auto it = classScopes_.find(ct->scope);
                        if (it != classScopes_.end()) {
                            if (memberNode) {
                                if (auto sym = it->second->lookup(memberNode->text)) {
                                    symbols_[memberNode] = sym;
                                    TypePtr t = sym->type ? sym->type : makePrimitive(PrimitiveType::Kind::Unknown, "unknown");
                                    types_[node] = t;
                                    return t;
                                }
                                error(memberNode, "unknown member '" + memberNode->text + "'");
                            }
                        }
                    }
                }
            }
            auto t = makePrimitive(PrimitiveType::Kind::Unknown, "unknown");
            types_[node] = t;
            return t;
        }
    }

    // Index: expr '[' expr_list ']'
    {
        bool hasIndex = false;
        for (const auto& c : node->children) {
            if (c && c->text == "[") { hasIndex = true; break; }
        }
        if (hasIndex) {
            AstPtr baseNode = nullptr;
            for (const auto& c : node->children) {
                if (c && c->kind == "expr") { if (!baseNode) baseNode = c; else checkExpr(c, scope); }
            }
            TypePtr baseType = baseNode ? checkExpr(baseNode, scope) : nullptr;
            TypePtr elem = makePrimitive(PrimitiveType::Kind::Unknown, "unknown");
            if (auto at = std::dynamic_pointer_cast<ArrayType>(baseType)) {
                if (at->element) elem = at->element;
            }
            types_[node] = elem;
            return elem;
        }
    }

    // Array literal
    if (!node->children.empty() && node->children[0] && node->children[0]->text == "[") {
        TypePtr elem = nullptr;
        for (const auto& c : node->children) {
            if (c && c->kind == "expr") {
                TypePtr t = checkExpr(c, scope);
                if (!elem) elem = t;
            }
        }
        auto at = makeArray(elem ? elem : makePrimitive(PrimitiveType::Kind::Unknown, "unknown"));
        types_[node] = at;
        return at;
    }

    // Construction: IDENTIFIER <{ expr_list }> or complex_type <{ ... }>
    {
        bool hasConstruct = false;
        for (const auto& c : node->children) {
            if (c && c->kind == "CONSTRUCT_OPEN") { hasConstruct = true; break; }
        }
        if (hasConstruct) {
            // type
            AstPtr typeNode = nullptr;
            for (const auto& c : node->children) {
                if (c && (c->kind == "IDENTIFIER" || c->kind == "complex_type" || c->kind == "type" || c->kind == "base_type")) {
                    typeNode = c; break;
                }
            }
            if (typeNode) {
                if (typeNode->kind == "IDENTIFIER") {
                    if (scope && !scope->lookup(typeNode->text)) {
                        error(typeNode, "unknown type '" + typeNode->text + "'");
                    }
                } else {
                    checkExpr(typeNode, scope);
                }
            }
            AstPtr argsNode = findChild(node, "expr_list");
            if (argsNode) {
                for (const auto& c : argsNode->children) {
                    if (c && c->kind == "expr") checkExpr(c, scope);
                }
            }
            TypePtr t = typeNode ? resolveType(typeNode, scope) : makePrimitive(PrimitiveType::Kind::Unknown, "unknown");
            if (!t) t = makePrimitive(PrimitiveType::Kind::Unknown, "unknown");
            types_[node] = t;
            return t;
        }

    // Anonymous template wrapper
        int tp = 0;
        AstPtr paramsNode = findChild(node, "template_arg_declare_list");
        if (paramsNode) {
            for (const auto& c : paramsNode->children) {
                if (c && c->kind == "IDENTIFIER") ++tp;
            }
        }
        TypePtr result = nullptr;
        // Function template
        bool isFunc = false;
        for (const auto& c : node->children) {
            if (c && c->text == "@") { isFunc = true; break; }
        }
        if (isFunc) {
            auto childScope = std::make_shared<SymbolTable>();
            childScope->setParent(scope);
            pushScope(childScope.get());
            std::vector<TypePtr> paramTypes;
            std::vector<std::string> paramNames;
            AstPtr paramsNode = findChild(node, "func_arg_declare_list");
            if (paramsNode) {
                for (const auto& p : paramsNode->children) {
                    if (!p || p->kind != "func_arg_declare") continue;
                    std::string pname;
                    if (!p->children.empty() && p->children[0] && p->children[0]->kind == "IDENTIFIER") pname = p->children[0]->text;
                    paramNames.push_back(pname);
                    TypePtr ptype = nullptr;
                    AstPtr typeNode = findChild(p, "type");
                    if (typeNode) ptype = resolveType(typeNode, childScope.get());
                    for (const auto& cc : p->children) {
                        if (cc && cc->kind == "expr") {
                            TypePtr dt = checkExpr(cc, childScope.get());
                            if (!ptype) ptype = dt;
                        }
                    }
                    if (!ptype) ptype = makePrimitive(PrimitiveType::Kind::Unknown, "unknown");
                    paramTypes.push_back(ptype);
                    if (!pname.empty()) {
                        auto sym = std::make_shared<Symbol>();
                        sym->name = pname;
                        sym->kind = SymbolKind::Variable;
                        sym->type = ptype;
                        childScope->insert(sym);
                    }
                }
            }
            bool hasArrow = false;
            for (const auto& c : node->children) {
                if (c && c->kind == "ARROW") { hasArrow = true; }
            }
            TypePtr returnType = makePrimitive(PrimitiveType::Kind::Unknown, "unknown");
            // arrow type
            bool arrow = false;
            for (const auto& c : node->children) {
                if (c && c->kind == "ARROW") { arrow = true; }
                if (arrow && (c->kind == "type" || c->kind == "base_type" || c->kind == "complex_type")) {
                    TypePtr t = resolveType(c, childScope.get());
                    if (t) returnType = t;
                }
            }

            AstPtr bodyScope = nullptr;
            for (const auto& c : node->children) {
                if (c && c->kind == "scope") bodyScope = c;
            }
            TypePtr oldReturn = currentReturnType_;
            bool oldInFunc = inFunction_;
            bool oldInferring = inferringReturn_;
            TypePtr oldInferred = inferredReturnType_;
            TypePtr oldFunctionType = currentFunctionType_;

            currentFunctionType_ = makeFunction(paramTypes, returnType);

            bool inferReturn = !hasArrow;
            if (inferReturn) {
                inferredReturnType_ = nullptr;
                inferringReturn_ = true;
                currentReturnType_ = makePrimitive(PrimitiveType::Kind::Unknown, "unknown");
            } else {
                currentReturnType_ = returnType;
            }
            inFunction_ = true;
            size_t beforeErrors = errors_.size();
            if (bodyScope) check(bodyScope, childScope.get());
            errors_.resize(beforeErrors);

            if (inferReturn && inferredReturnType_) {
                returnType = inferredReturnType_;
            }

            currentReturnType_ = oldReturn;
            inFunction_ = oldInFunc;
            inferringReturn_ = oldInferring;
            inferredReturnType_ = oldInferred;
            currentFunctionType_ = oldFunctionType;
            popScope();

            auto ft = makeFunction(paramTypes, returnType);
            if (auto ftf = std::dynamic_pointer_cast<FunctionType>(ft)) {
                ftf->paramNames = paramNames;
                if (bodyScope) ftf->body = bodyScope.get();
            }
            // template param names
            std::vector<std::string> tpNames;
            AstPtr tpNode = findChild(node, "template_arg_declare_list");
            if (tpNode) {
                for (const auto& p : tpNode->children) {
                    if (p && p->kind == "IDENTIFIER") tpNames.push_back(p->text);
                }
            }
            ft->templateParams = (int)tpNames.size();
            if (auto ftf = std::dynamic_pointer_cast<FunctionType>(ft)) {
                ftf->templateParamNames = tpNames;
            }
            types_[node] = ft;
            return ft;
        }

        for (const auto& c : node->children) {
            if (c && (c->text == "#" || c->kind == "class" ||
                      c->kind == "expr" || c->kind == "type" || c->kind == "complex_type" || c->kind == "base_type")) {
                if (c->text == "#" || c->kind == "class") {
                    auto ct = std::make_shared<ClassType>();
                    ct->name = "class";
                    int tp = 0;
                    AstPtr tpNode = findChild(node, "template_arg_declare_list");
                    if (tpNode) for (const auto& p : tpNode->children) if (p && p->kind == "IDENTIFIER") ++tp;
                    ct->templateParams = tp;
                    if (!result) result = ct;
                } else {
                    TypePtr t = resolveType(c, scope);
                    if (t) {
                        int tp = 0;
                        AstPtr tpNode = findChild(node, "template_arg_declare_list");
                        if (tpNode) for (const auto& p : tpNode->children) if (p && p->kind == "IDENTIFIER") ++tp;
                        t->templateParams = tp;
                        if (!result) result = t;
                    }
                }
            }
        }
        if (!result) {
            int tp = 0;
            AstPtr tpNode = findChild(node, "template_arg_declare_list");
            if (tpNode) for (const auto& p : tpNode->children) if (p && p->kind == "IDENTIFIER") ++tp;
            result = std::make_shared<ClassType>();
            result->name = "template";
            result->templateParams = tp;
        }
        types_[node] = result;
        return result;
    }

    // Default: check children
    for (const auto& c : node->children) {
        if (c && c->kind == "expr") checkExpr(c, scope);
        if (c && c->kind == "scope") check(c, scope);
        if (c && c->kind == "type") resolveType(c, scope);
    }
    auto t = makePrimitive(PrimitiveType::Kind::Unknown, "unknown");
    types_[node] = t;
    return t;
}

TypePtr SemanticAnalyzer::typeOf(const AstPtr& node) const {
    auto it = types_.find(node);
    if (it == types_.end()) return nullptr;
    return it->second;
}

SymbolPtr SemanticAnalyzer::symbolOf(const AstPtr& node) const {
    auto it = symbols_.find(node);
    if (it == symbols_.end()) return nullptr;
    return it->second;
}

} // namespace at
