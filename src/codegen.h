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

private:
    std::vector<BinSigDecl> sigs_;
    bool haveSigs_ = false;
    std::map<std::string, std::pair<std::vector<std::string>, unsigned>> templateShapes_;
    void emitSignatureSection();

    llvm::LLVMContext context_;
    std::unique_ptr<llvm::Module> module_;
    llvm::IRBuilder<> builder_;
    std::unique_ptr<RttiManager> rtti_;

    std::map<std::string, llvm::Value*> namedValues_;
    std::map<std::string, uint64_t> pinnedAddrs_;
    std::map<std::string, llvm::Function*> templateFunctions_;
    std::map<std::string, llvm::Function*> functions_;
    std::map<std::string, llvm::StructType*> classStructTypes_;
    std::map<std::string, std::vector<std::string>> classFieldNames_;
    std::map<std::string, std::string> overloads_;
    std::map<std::string, std::string> importedModules_;
    std::set<std::string> publicGlobals_;
    llvm::Function* currentFunction_ = nullptr;

    std::vector<llvm::BasicBlock*> breakStack_;
    std::vector<llvm::BasicBlock*> continueStack_;

    std::string modulePrefix_;

    std::string mangleName(const std::string& name) const {
        if (publicGlobals_.count(name)) return name;
        if (!modulePrefix_.empty()) return modulePrefix_ + "_" + name;
        return name;
    }

    void addPublicGlobal(const std::string& name) { publicGlobals_.insert(name); }

    llvm::AllocaInst* createEntryBlockAlloca(llvm::Function* function,
                                             const std::string& varName,
                                             llvm::Type* type);

    llvm::Type* llvmTypeForAst(const AstPtr& typeNode);
    llvm::Constant* getRttiForType(const AstPtr& typeNode);

    llvm::Value* codegenExpr(const AstPtr& node);
    void codegenStmt(const AstPtr& node);

    llvm::Function* emitTemplateMetaFunc(const std::string& name, const AstPtr& tmplNode);
    llvm::Function* emitFunction(const std::string& name, const AstPtr& funcExpr);
    llvm::Function* emitClass(const std::string& name, const AstPtr& classNode);
    void declareRuntimeOps();
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
