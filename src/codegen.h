#pragma once

#include "ast.h"
#include "rtti.h"
#include "sig_binary.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace at {

class CodeGenLower;

// CodeGen: high-level code generation entry point.
// Uses CodeGenLower internally for AST -> LLVM IR lowering.
// Handles object file emission (compiler path).
class CodeGen {
public:
    CodeGen();
    ~CodeGen();

    bool compile(const AstPtr& root, const std::string& outputPath, bool isMainModule = false,
                 const std::vector<std::string>& initFuncs = {},
                 const std::string& initFuncName = "");

    void setSignatures(std::vector<BinSigDecl> sigs) { sigs_ = std::move(sigs); haveSigs_ = true; }

    void setModulePrefix(const std::string& prefix) { modulePrefix_ = prefix; }
    const std::string& modulePrefix() const { return modulePrefix_; }

    void registerImportedModule(const std::string& varName, const std::string& modName) {
        importedModules_[varName] = modName;
    }

    // Access the underlying CodeGenLower (for advanced use / interpreter sharing).
    CodeGenLower& lower() { return *lower_; }

private:
    std::vector<BinSigDecl> sigs_;
    bool haveSigs_ = false;
    std::string modulePrefix_;
    std::map<std::string, std::string> importedModules_;

    llvm::LLVMContext context_;
    std::unique_ptr<CodeGenLower> lower_;
};

static AstPtr findChild(const AstPtr& node, const std::string& kind) {
    if (!node) return nullptr;
    for (const auto& c : node->children) {
        if (c && c->kind == kind) return c;
    }
    return nullptr;
}

static AstPtr findNodeRecursive(const AstPtr& node, const std::string& kind) {
    if (!node) return nullptr;
    if (node->kind == kind) return node;
    for (const auto& c : node->children) {
        if (auto r = findNodeRecursive(c, kind)) return r;
    }
    return nullptr;
}

} // namespace at
