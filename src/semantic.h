#pragma once

#include "ast.h"
#include "object_importer.h"
#include "symbols.h"
#include "types.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace at {

struct SemanticError {
    SourceSpan span;
    std::string message;
};

class SemanticAnalyzer {
public:
    explicit SemanticAnalyzer();

    void analyze(const AstPtr& root);

    // Make externally-defined symbols (e.g. from ObjectImporter / .at.sigs)
    // visible in the global scope. Call before analyze(). Existing names win.
    size_t importSymbols(const std::vector<SymbolPtr>& symbols);

    // Import symbols from an ELF object file (.o or .so) via its .at.sigs
    // section and insert them into the given scope. Returns the number of
    // symbols imported. Used to handle `import 'module.o'` expressions during
    // semantic analysis so the type checker knows about FFI functions.
    size_t importModule(const std::string& path, SymbolTable* scope = nullptr);

    const std::vector<SemanticError>& errors() const { return errors_; }
    SymbolTable* globalScope() const { return globalScope_.get(); }

    // Type/ symbol queries for later passes.
    TypePtr typeOf(const AstPtr& node) const;
    SymbolPtr symbolOf(const AstPtr& node) const;

private:
    std::shared_ptr<SymbolTable> globalScope_;
    SymbolTable* currentScope_ = nullptr;

    std::map<AstPtr, TypePtr> types_;
    std::map<AstPtr, SymbolPtr> symbols_;
    std::map<AstPtr, std::shared_ptr<SymbolTable>> scopes_;
    std::map<void*, std::shared_ptr<SymbolTable>> classScopes_;

    // Type substitution for template instantiation.
    std::map<std::string, TypePtr> typeSubstitution_;

    std::vector<SemanticError> errors_;

    // Function/loop context
    int loopDepth_ = 0;
    bool inFunction_ = false;
    TypePtr currentReturnType_;
    TypePtr currentFunctionType_;
    // Return type inference
    TypePtr inferredReturnType_;
    bool inferringReturn_ = false;

    // Scope management
    void pushScope(SymbolTable* scope);
    void popScope();

    // Pass 1: declarations
    void collect(const AstPtr& node, SymbolTable* scope);
    // Pass 2: checking
    void check(const AstPtr& node, SymbolTable* scope);

    // Helpers
    void error(const AstPtr& node, const std::string& message);
    Visibility parseVisibility(const AstPtr& node) const;
    TypePtr resolveType(const AstPtr& node, SymbolTable* scope);
    // Build a FunctionType from a node holding `( func_arg_declare_list ) [-> type]`.
    TypePtr functionTypeFromSig(const AstPtr& node, SymbolTable* scope);
    TypePtr checkExpr(const AstPtr& node, SymbolTable* scope);
    void checkStmt(const AstPtr& node, SymbolTable* scope);

    // Type helpers
    TypePtr primitiveFromName(const std::string& name) const;
};

} // namespace at
