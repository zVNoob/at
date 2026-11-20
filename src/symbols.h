#pragma once

#include "ast.h"
#include "types.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace at {

struct SourceSpan;

enum class SymbolKind {
    Variable,
    Function,
    Class,
    Template,
    Alias,
    Import,
};

enum class Visibility {
    Public,
    Protected,
    Private,
    ConstPublic,
    ConstProtected,
    ConstPrivate,
};

struct Symbol {
    std::string name;
    SymbolKind kind = SymbolKind::Variable;
    Visibility visibility = Visibility::Public;
    bool isConst = false;
    TypePtr type;
    // Source declaration, if any.
    void* declaration = nullptr;
    SourceSpan span{};
};

using SymbolPtr = std::shared_ptr<Symbol>;

class SymbolTable {
public:
    virtual ~SymbolTable() = default;

    // Subclasses may override to change storage or add import/export behavior.
    virtual bool insert(const SymbolPtr& symbol);
    virtual SymbolPtr lookupLocal(const std::string& name) const;
    virtual SymbolPtr lookup(const std::string& name) const;

    virtual SymbolTable* parent() const { return parent_; }
    virtual void setParent(SymbolTable* parent) { parent_ = parent; }

    // Hooks for incremental building / .o import.
    virtual size_t importObjectFile(const std::string& path);
    virtual std::vector<SymbolPtr> exportedSymbols() const;

protected:
    SymbolTable* parent_ = nullptr;
    std::map<std::string, SymbolPtr> locals_;
};

} // namespace at
