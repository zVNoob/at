#include "codegen.h"
#include "ast.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/IR/LegacyPassManager.h>

#include <iostream>

namespace at {

CodeGen::CodeGen() : builder_(context_), rtti_(nullptr) {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
}

CodeGen::~CodeGen() = default;

llvm::AllocaInst* CodeGen::createEntryBlockAlloca(llvm::Function* function,
                                                 const std::string& varName,
                                                 llvm::Type* type) {
    llvm::IRBuilder<> tmpBuilder(&function->getEntryBlock(),
                                 function->getEntryBlock().begin());
    return tmpBuilder.CreateAlloca(type, nullptr, varName);
}

void CodeGen::declareRuntimeOps() {
    llvm::FunctionType* mallocType = llvm::FunctionType::get(
        builder_.getPtrTy(), {builder_.getInt64Ty()}, false);
    module_->getOrInsertFunction("at_malloc", mallocType);

    llvm::FunctionType* freeType = llvm::FunctionType::get(
        builder_.getVoidTy(), {builder_.getPtrTy()}, false);
    module_->getOrInsertFunction("at_free", freeType);

    llvm::FunctionType* printIntType = llvm::FunctionType::get(
        builder_.getVoidTy(), {builder_.getInt64Ty()}, false);
    module_->getOrInsertFunction("at_print_int", printIntType);

    llvm::FunctionType* printDecType = llvm::FunctionType::get(
        builder_.getVoidTy(), {builder_.getDoubleTy()}, false);
    module_->getOrInsertFunction("at_print_decimal", printDecType);

    llvm::FunctionType* printStrType = llvm::FunctionType::get(
        builder_.getVoidTy(), {builder_.getInt8PtrTy()}, false);
    module_->getOrInsertFunction("at_print_str", printStrType);
}

bool CodeGen::compile(const AstPtr& root, const std::string& outputPath, bool isMainModule,
                     const std::vector<std::string>& initFuncs) {
    module_ = std::make_unique<llvm::Module>("at_module", context_);
    templateShapes_.clear();
    rtti_ = std::make_unique<RttiManager>(module_.get());
    declareRuntimeOps();

    std::vector<AstPtr> allStmts;
    std::function<void(const AstPtr&)> collectStmts = [&](const AstPtr& node) {
        if (!node) return;
        if (node->kind == "stmt") {
            allStmts.push_back(node);
            return;
        }
        for (const auto& c : node->children) collectStmts(c);
    };
    collectStmts(root);

    // First pass: emit classes
    for (const auto& s : allStmts) {
        if (s->children.size() >= 3 &&
            s->children[0] && s->children[0]->kind == "IDENTIFIER" &&
            s->children[1] && (s->children[1]->kind == "ALIAS_PUBLIC" || s->children[1]->kind == "ALIAS_PRIVATE")) {
            AstPtr classNode = findNodeRecursive(s, "class");
            if (classNode) {
                std::string className = s->children[0]->text;
                if (!classStructTypes_.count(className)) {
                    emitClass(className, classNode);
                }
            }
        }
    }

    // Second pass: emit template meta-functions
    for (const auto& s : allStmts) {
        if (s->children.size() >= 3 &&
            s->children[0] && s->children[0]->kind == "IDENTIFIER" &&
            s->children[1] && (s->children[1]->kind == "ALIAS_PUBLIC" || s->children[1]->kind == "ALIAS_PRIVATE")) {
            AstPtr tmplNode = findNodeRecursive(s, "anonymous_template");
            if (tmplNode) {
                std::string funcName = s->children[0]->text;
                if (!templateFunctions_.count(funcName)) {
                    emitTemplateMetaFunc(funcName, tmplNode);
                }
            }
        }
    }

    // Save builder insert point before template emission
    llvm::BasicBlock* savedBB = builder_.GetInsertBlock();

    if (isMainModule) {
        // Main module: call imported module inits, then top-level code in main

        llvm::FunctionType* mainType = llvm::FunctionType::get(builder_.getInt64Ty(), false);
        llvm::Function* mainFn = llvm::Function::Create(mainType, llvm::Function::ExternalLinkage, "main", module_.get());
        llvm::BasicBlock* mainEntry = llvm::BasicBlock::Create(context_, "entry", mainFn);
        builder_.SetInsertPoint(mainEntry);
        currentFunction_ = mainFn;

        // Call each imported module's init function
        for (const auto& initName : initFuncs) {
            if (auto* initFn = module_->getFunction(initName)) {
                builder_.CreateCall(initFn, {}, initName + "_call");
            } else {
                // Declare external function
                llvm::FunctionType* initType = llvm::FunctionType::get(builder_.getInt64Ty(), false);
                llvm::Function* extFn = llvm::Function::Create(initType, llvm::Function::ExternalLinkage, initName, module_.get());
                builder_.CreateCall(extFn, {}, initName + "_call");
            }
        }

        // Codegen all top-level statements into main
        for (const auto& c : root->children) {
            if (c && c->kind == "scope") {
                for (const auto& s : c->children) {
                    codegenStmt(s);
                }
            }
        }
        if (!builder_.GetInsertBlock()->getTerminator()) {
            builder_.CreateRet(builder_.getInt64(0));
        }
        currentFunction_ = nullptr;
    } else {
        // Imported module: main just returns 0, top-level code goes into module_init
        llvm::FunctionType* mainType = llvm::FunctionType::get(builder_.getInt64Ty(), false);
        llvm::Function* mainFn = llvm::Function::Create(mainType, llvm::Function::ExternalLinkage, "main", module_.get());
        llvm::BasicBlock* mainEntry = llvm::BasicBlock::Create(context_, "entry", mainFn);
        builder_.SetInsertPoint(mainEntry);
        builder_.CreateRet(builder_.getInt64(0));
        currentFunction_ = nullptr;

        llvm::FunctionType* initType = llvm::FunctionType::get(builder_.getInt64Ty(), false);
        llvm::Function* initFn = llvm::Function::Create(initType, llvm::Function::ExternalLinkage, "module_init", module_.get());
        llvm::BasicBlock* initEntry = llvm::BasicBlock::Create(context_, "entry", initFn);
        builder_.SetInsertPoint(initEntry);
        currentFunction_ = initFn;

        for (const auto& c : root->children) {
            if (c && c->kind == "scope") {
                for (const auto& s : c->children) {
                    codegenStmt(s);
                }
            }
        }
        if (!builder_.GetInsertBlock()->getTerminator()) {
            builder_.CreateRet(builder_.getInt64(0));
        }
        currentFunction_ = nullptr;
    }

    // Restore builder insert point
    if (savedBB) builder_.SetInsertPoint(savedBB);

    // Emit signatures for template meta-functions
    for (const auto& d : sigs_) {
        if (d.kind == at::BinSigDecl::Meta) {
            std::string symName = mangleName(d.name + "_meta");
            if (!module_->getFunction(symName)) {
                llvm::FunctionType* ft = llvm::FunctionType::get(builder_.getInt64Ty(), false);
                llvm::Function* fn = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, symName, module_.get());
                llvm::BasicBlock* bb = llvm::BasicBlock::Create(context_, "entry", fn);
                llvm::IRBuilder<> b(bb);
                b.CreateRet(b.getInt64(0));
            }
        }
    }
    emitSignatureSection();

    std::string targetTriple = llvm::sys::getProcessTriple();
    module_->setTargetTriple(targetTriple);

    std::string err;
    auto target = llvm::TargetRegistry::lookupTarget(targetTriple, err);
    if (!target) {
        std::cerr << "codegen: no target for " << targetTriple << ": " << err << "\n";
        return false;
    }

    llvm::TargetOptions opt;
    bool isSharedLib = outputPath.rfind(".so") == outputPath.size() - 3;
    auto rm = isSharedLib ? llvm::Reloc::PIC_ : llvm::Reloc::Static;
    auto targetMachine = target->createTargetMachine(targetTriple, "generic", "", opt, rm);
    module_->setDataLayout(targetMachine->createDataLayout());

    llvm::CodeGenFileType fileType = llvm::CGFT_ObjectFile;
    if (outputPath.rfind(".ll") == outputPath.size() - 3) {
        fileType = llvm::CGFT_AssemblyFile;
    }

    std::error_code ec;
    llvm::raw_fd_ostream out(outputPath, ec);
    if (ec) {
        std::cerr << "codegen: cannot open " << outputPath << ": " << ec.message() << "\n";
        return false;
    }

    llvm::legacy::PassManager pass;
    if (targetMachine->addPassesToEmitFile(pass, out, nullptr, fileType)) {
        std::cerr << "codegen: cannot emit file\n";
        return false;
    }
    pass.run(*module_);
    out.flush();
    return true;
}

} // namespace at
