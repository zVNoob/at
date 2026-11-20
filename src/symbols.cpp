#include "symbols.h"

namespace at {

bool SymbolTable::insert(const SymbolPtr& symbol) {
    if (!symbol) return false;
    if (locals_.count(symbol->name)) return false;
    locals_[symbol->name] = symbol;
    return true;
}

SymbolPtr SymbolTable::lookupLocal(const std::string& name) const {
    auto it = locals_.find(name);
    if (it == locals_.end()) return nullptr;
    return it->second;
}

SymbolPtr SymbolTable::lookup(const std::string& name) const {
    if (auto local = lookupLocal(name)) return local;
    if (parent_) return parent_->lookup(name);
    return nullptr;
}

size_t SymbolTable::importObjectFile(const std::string& /*path*/) {
    // Base implementation does nothing. Subclasses can load symbols from .o files.
    return 0;
}

std::vector<SymbolPtr> SymbolTable::exportedSymbols() const {
    std::vector<SymbolPtr> result;
    for (const auto& [name, sym] : locals_) {
        if (sym && sym->visibility == Visibility::Public) result.push_back(sym);
    }
    return result;
}

} // namespace at
