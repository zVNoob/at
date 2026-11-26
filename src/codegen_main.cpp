#include "codegen.h"
#include "codegen_lower.h"

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

CodeGen::CodeGen() : lower_(nullptr) {}

CodeGen::~CodeGen() = default;

bool CodeGen::compile(const AstPtr& root, const std::string& outputPath, bool isMainModule,
                     const std::vector<std::string>& initFuncs, const std::string& initFuncName) {
    if (!lower_) {
        lower_ = std::make_unique<CodeGenLower>(context_, LowerConfig{
            .moduleName = "at_module",
            .modulePrefix = modulePrefix_,
        });
    }

    // Import registered modules into the lower
    for (const auto& [varName, modName] : importedModules_) {
        lower_->registerImportedModule(varName, modName);
    }

    // Pass signatures to CodeGenLower for .at.sigs section emission
    lower_->setSignatures(sigs_);

    // Lower AST to LLVM IR
    // Note: sem is not available here; we pass a default-constructed one
    SemanticAnalyzer dummySem;
    auto result = lower_->lower(root, dummySem, isMainModule, initFuncs, initFuncName);
    if (!result) {
        std::cerr << "codegen: lowering failed\n";
        return false;
    }

    // Set target triple and data layout
    std::string targetTriple = llvm::sys::getProcessTriple();
    result->module->setTargetTriple(targetTriple);

    std::string targetErr;
    auto target = llvm::TargetRegistry::lookupTarget(targetTriple, targetErr);
    if (!target) {
        std::cerr << "codegen: no target for " << targetTriple << "\n";
        return false;
    }

    llvm::TargetOptions opt;
    bool isSharedLib = outputPath.rfind(".so") == outputPath.size() - 3;
    auto rm = isSharedLib ? llvm::Reloc::PIC_ : llvm::Reloc::Static;
    auto targetMachine = target->createTargetMachine(targetTriple, "generic", "", opt, rm);
    if (!targetMachine) {
        std::cerr << "codegen: cannot create target machine\n";
        return false;
    }
    result->module->setDataLayout(targetMachine->createDataLayout());

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
    pass.run(*result->module);
    out.flush();
    return true;
}

} // namespace at
