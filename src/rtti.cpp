#include "rtti.h"
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <iostream>
#include <string>

namespace at {

static const char* kRttiSection = ".rodata.rtti";

RttiManager::RttiManager(llvm::Module* module)
    : module_(module), ctx_(module->getContext()) {
    // Build RTTI struct type: { i64 size, i64 align, ptr name, ptr drop, ptr copy, ptr print }
    auto i64 = llvm::Type::getInt64Ty(ctx_);
    auto ptr = llvm::Type::getInt8PtrTy(ctx_);
    rttiStructTy_ = llvm::StructType::create(ctx_, {i64, i64, ptr, ptr, ptr, ptr}, "AtRtti");
}

llvm::Constant* RttiManager::getRtti(const std::string& name, uint64_t size, uint64_t align) {
    auto it = rttiCache_.find(name);
    if (it != rttiCache_.end()) return it->second;

    auto i64 = llvm::Type::getInt64Ty(ctx_);
    auto ptr = llvm::Type::getInt8PtrTy(ctx_);

    // Global string for type name
    std::string nameStr = name + ".rtti.name";
    llvm::Constant* nameGlob = module_->getOrInsertGlobal(nameStr, ptr);
    llvm::GlobalVariable* nameGv = llvm::dyn_cast< llvm::GlobalVariable>(module_->getNamedGlobal(nameStr));
    if (!nameGv->hasInitializer()) {
        nameGv->setInitializer(llvm::ConstantDataArray::getString(ctx_, name, /*nullTerm=*/true));
        nameGv->setConstant(true);
        nameGv->setSection(kRttiSection);
        nameGv->setLinkage(llvm::GlobalValue::InternalLinkage);
    }
    llvm::Constant* namePtr = llvm::ConstantExpr::getBitCast(nameGv, ptr);

    // Null function pointers for drop/copy/print
    llvm::Constant* nullFn = llvm::ConstantPointerNull::get(ptr);

    // Build RTTI constant struct
    std::vector<llvm::Constant*> fields = {
        llvm::ConstantInt::get(i64, size),
        llvm::ConstantInt::get(i64, align),
        namePtr,
        nullFn, // drop
        nullFn, // copy
        nullFn  // print
    };
    auto* rttiConst = llvm::ConstantStruct::get(rttiStructTy_, fields);

    // Create global in .rodata.rtti
    std::string gvName = "@rtti_" + name;
    auto* gv = new llvm::GlobalVariable(
        *module_, rttiStructTy_, /*constant=*/true,
        llvm::GlobalValue::InternalLinkage, rttiConst, gvName);
    gv->setSection(kRttiSection);

    rttiCache_[name] = gv;
    return gv;
}

void RttiManager::declareRuntimeOps(const std::string& /*typeName*/) {
    // No-op: runtime ops left null for now
}

} // namespace at