#include "ast.h"
#include "codegen.h"
#include "ast.h"

namespace at {

llvm::Function* CodeGen::emitClass(const std::string& name, const AstPtr& classNode) {
    std::vector<std::string> fieldNames;
    AstPtr paramsNode = findChild(classNode, "func_arg_declare_list");
    if (paramsNode) {
        for (const auto& p : paramsNode->children) {
            if (!p || p->kind != "func_arg_declare") continue;
            if (!p->children.empty() && p->children[0] && p->children[0]->kind == "IDENTIFIER")
                fieldNames.push_back(p->children[0]->text);
        }
    }

    for (const auto& c : classNode->children) {
        if (c && c->kind == "scope") {
            for (const auto& s : c->children) {
                if (s && s->kind == "stmt" && !s->children.empty() &&
                    s->children[0] && s->children[0]->kind == "IDENTIFIER" &&
                    s->children.size() >= 2 && s->children[1] && s->children[1]->kind == "declare") {
                    fieldNames.push_back(s->children[0]->text);
                }
            }
        }
    }
    classFieldNames_[name] = fieldNames;

    std::vector<llvm::Type*> memberTys;
    for (size_t i = 0; i < fieldNames.size(); ++i) memberTys.push_back(builder_.getInt64Ty());
    llvm::StructType* structTy = llvm::StructType::create(context_, memberTys, name + "_struct");
    classStructTypes_[name] = structTy;

    std::vector<llvm::Type*> paramTys;
    for (size_t i = 0; i < fieldNames.size(); ++i) paramTys.push_back(builder_.getInt64Ty());
    llvm::FunctionType* ctorFT = llvm::FunctionType::get(builder_.getInt64Ty(), paramTys, false);
    std::string ctorName = mangleName(name);
    llvm::Function* ctor = llvm::Function::Create(ctorFT, llvm::Function::ExternalLinkage, ctorName, module_.get());
    llvm::BasicBlock* bb = llvm::BasicBlock::Create(context_, "entry", ctor);
    builder_.SetInsertPoint(bb);
    currentFunction_ = ctor;

    llvm::Value* size = builder_.getInt64(fieldNames.size() * 8);
    llvm::Function* mallocFn = module_->getFunction("at_malloc");
    if (!mallocFn) {
        llvm::FunctionType* mallocType = llvm::FunctionType::get(builder_.getPtrTy(), {builder_.getInt64Ty()}, false);
        mallocFn = llvm::Function::Create(mallocType, llvm::Function::ExternalLinkage, "at_malloc", module_.get());
    }
    llvm::Value* rawPtr = builder_.CreateCall(mallocFn, {size}, "objptr");
    llvm::Value* objPtr = builder_.CreateBitCast(rawPtr, builder_.getPtrTy(), "obj");

    unsigned argIdx = 0;
    for (size_t i = 0; i < fieldNames.size(); ++i) {
        llvm::Value* fieldPtr = builder_.CreateGEP(builder_.getInt64Ty(), objPtr, builder_.getInt64((int64_t)i), "fieldptr");
        llvm::Value* val = nullptr;
        if (argIdx < ctor->arg_size()) {
            val = &*std::next(ctor->arg_begin(), argIdx);
        } else {
            val = builder_.getInt64(0);
        }
        builder_.CreateStore(val, fieldPtr);
        ++argIdx;
    }
    builder_.CreateRet(builder_.CreatePtrToInt(objPtr, builder_.getInt64Ty(), "objint"));
    functions_[ctorName] = ctor;
    return ctor;
}

} // namespace at
