#include "codegen.h"
#include "ast.h"

namespace at {

llvm::Function* CodeGen::emitTemplateMetaFunc(const std::string& name, const AstPtr& tmplNode) {
    // Save current insert point so we can restore it after emitting the template function
    llvm::BasicBlock* savedBB = builder_.GetInsertBlock();
    AstPtr tpNode = findChild(tmplNode, "template_arg_declare_list");
    unsigned numTmplParams = 0;
    if (tpNode) {
        for (const auto& p : tpNode->children) {
            if (p && p->kind == "IDENTIFIER") ++numTmplParams;
        }
    }

    AstPtr funcExpr = nullptr;
    // Case 1: tmplNode is anonymous_template containing @ (...) {...}
    for (const auto& c : tmplNode->children) {
        if (c && c->kind == "expr") {
            if (!c->children.empty() && c->children[0] && c->children[0]->text == "@") {
                funcExpr = c;
                break;
            }
        }
    }
    // Case 2: tmplNode IS the @ (...) {...} expression directly
    if (!funcExpr && tmplNode->kind == "expr" && !tmplNode->children.empty() &&
        tmplNode->children[0] && tmplNode->children[0]->text == "@") {
        funcExpr = tmplNode;
    }
    if (!funcExpr) {
        if (savedBB) builder_.SetInsertPoint(savedBB);
        return nullptr;
    }

    AstPtr paramsNode = findChild(funcExpr, "func_arg_declare_list");
    std::vector<llvm::Type*> paramTys;
    std::vector<std::string> paramNames;
    if (paramsNode) {
        for (const auto& p : paramsNode->children) {
            if (!p || p->kind != "func_arg_declare") continue;
            if (!p->children.empty() && p->children[0] && p->children[0]->kind == "IDENTIFIER") {
                paramNames.push_back(p->children[0]->text);
                paramTys.push_back(builder_.getInt64Ty());
            }
        }
    }

    for (unsigned i = 0; i < numTmplParams; ++i) {
        paramTys.push_back(builder_.getInt8PtrTy());
        paramNames.push_back("T" + std::to_string(i) + "_rtti");
    }

    llvm::FunctionType* ft = llvm::FunctionType::get(builder_.getInt64Ty(), paramTys, false);
    std::string mangledName = name;
    if (!modulePrefix_.empty()) mangledName = modulePrefix_ + "_" + mangledName;
    llvm::Function* fn = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, mangledName, module_.get());
    llvm::BasicBlock* bb = llvm::BasicBlock::Create(context_, "entry", fn);
    builder_.SetInsertPoint(bb);

    unsigned argIdx = 0;
    for (auto& arg : fn->args()) {
        arg.setName(paramNames[argIdx]);
        llvm::AllocaInst* alloca = createEntryBlockAlloca(fn, paramNames[argIdx], arg.getType());
        builder_.CreateStore(&arg, alloca);
        namedValues_[paramNames[argIdx]] = alloca;
        ++argIdx;
    }

    // Find body scope from template node
    AstPtr bodyNode = findChild(funcExpr, "scope");
    if (bodyNode) {
        codegenStmt(bodyNode);
    }

    if (!builder_.GetInsertBlock()->getTerminator()) {
        builder_.CreateRet(builder_.getInt64(0));
    }
    currentFunction_ = nullptr;
    // Restore builder insert point
    if (savedBB) builder_.SetInsertPoint(savedBB);
    return fn;
}

} // namespace at
