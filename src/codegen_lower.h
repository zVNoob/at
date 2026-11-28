#pragma once

#include "ast.h"
#include "rtti.h"
#include "semantic.h"
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

struct LowerConfig {
    std::string moduleName = "at_module";
    std::string modulePrefix;
    bool optimize = false;
    std::string targetTriple;
    bool jitMode = false; // when true, emit at_native_dispatch calls for native functions
    std::map<std::string, int64_t> nativeFnIndices; // function name -> index in g_nativeFns
};

struct LoweredResult {
    std::unique_ptr<llvm::Module> module;
    std::string mainSymbol;
    std::vector<std::string> exportedSymbols;
};

class CodeGenLower {
public:
    CodeGenLower(llvm::LLVMContext& ctx, const LowerConfig& cfg);
    ~CodeGenLower();

    std::unique_ptr<LoweredResult> lower(const AstPtr& root,
                                         SemanticAnalyzer& sem,
                                         bool isMainModule = false,
                                         const std::vector<std::string>& initFuncs = {},
                                         const std::string& initFuncName = "");

    llvm::IRBuilder<>& builder() { return builder_; }
    llvm::LLVMContext& context() { return context_; }

    void declareRuntimeOps();

    void setSignatures(std::vector<BinSigDecl> sigs) { sigs_ = std::move(sigs); }

    std::string mangleName(const std::string& name) const {
        if (publicGlobals_.count(name)) return name;
        if (!modulePrefix_.empty()) return modulePrefix_ + "_" + name;
        return name;
    }

    void addPublicGlobal(const std::string& name) { publicGlobals_.insert(name); }

    void pushScope();
    void popScope();
    void setNamedValue(const std::string& name, llvm::Value* val);
    llvm::Value* getNamedValue(const std::string& name) const;

    void registerImportedModule(const std::string& varName, const std::string& modName) {
        importedModules_[varName] = modName;
    }

    void setNativeFnIndices(const std::map<std::string, int64_t>& indices) {
        cfg_.nativeFnIndices = indices;
    }

    const std::map<std::string, std::string>& importedModules() const {
        return importedModules_;
    }

private:
    llvm::LLVMContext& context_;
    llvm::IRBuilder<> builder_;
    LowerConfig cfg_;

    std::unique_ptr<llvm::Module> module_;
    std::unique_ptr<RttiManager> rtti_;

    std::vector<std::map<std::string, llvm::Value*>> scopeStack_;
    std::map<std::string, llvm::Value*> namedValues_;

    std::map<std::string, llvm::Function*> functions_;
    std::map<std::string, llvm::StructType*> classStructTypes_;
    std::map<std::string, std::vector<std::string>> classFieldNames_;
    std::map<std::string, llvm::Function*> templateFunctions_;
    std::map<std::string, std::pair<std::vector<std::string>, unsigned>> templateShapes_;
    std::set<std::string> publicGlobals_;
    std::map<std::string, std::string> importedModules_;
    std::map<std::string, uint64_t> pinnedAddrs_;

    std::vector<BinSigDecl> sigs_;

    std::vector<llvm::BasicBlock*> breakStack_;
    std::vector<llvm::BasicBlock*> continueStack_;

    llvm::Function* currentFunction_ = nullptr;
    std::string modulePrefix_;
    SemanticAnalyzer* sem_ = nullptr;

    llvm::AllocaInst* createEntryBlockAlloca(llvm::Function* function,
                                             const std::string& varName,
                                             llvm::Type* type);
    llvm::Type* llvmTypeForAst(const AstPtr& typeNode);
    llvm::Constant* getRttiForType(const AstPtr& typeNode);

    llvm::Value* lowerExpr(const AstPtr& node);
    void lowerStmt(const AstPtr& node);

    llvm::Function* emitTemplateMetaFunc(const std::string& name, const AstPtr& tmplNode);
    llvm::Function* emitFunction(const std::string& name, const AstPtr& funcExpr);
    llvm::Function* emitClass(const std::string& name, const AstPtr& classNode);

    void collectStmts(const AstPtr& node, std::vector<AstPtr>& out);
};

} // namespace at
