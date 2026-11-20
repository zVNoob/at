#include <iostream>
#include "codegen.h"
#include "ast.h"

namespace at {

void CodeGen::codegenStmt(const AstPtr& node) {
    if (!node) return;

    if (node->kind == "scope" || node->kind == "start") {
        for (const auto& c : node->children) codegenStmt(c);
        return;
    }
    if (node->kind != "stmt") return;

    if (node->children.empty()) return;
    const AstPtr& first = node->children[0];
    // Block: '{' scope '}'
    if (first && first->kind == "CHAR(123)") {
        auto savedNamed = namedValues_;
        for (const auto& c : node->children) codegenStmt(c);
        namedValues_ = std::move(savedNamed);
        return;
    }

    // Variable declaration: IDENTIFIER declare type = expr
    if (first && first->kind == "IDENTIFIER" && node->children.size() >= 2 &&
        node->children[1] && node->children[1]->kind == "declare") {
        std::string varName = first->text;
        llvm::Type* varType = builder_.getInt64Ty();
        llvm::Function* fn = builder_.GetInsertBlock()->getParent();
        llvm::AllocaInst* alloca = createEntryBlockAlloca(fn, varName, varType);

        for (size_t i = 0; i < node->children.size(); ++i) {
            if (node->children[i] && node->children[i]->kind == "CHAR(61)" &&
                i + 1 < node->children.size()) {
                AstPtr initNode = node->children[i + 1];
                if (initNode && initNode->kind == "expr" && !initNode->children.empty() &&
                    initNode->children[0] && initNode->children[0]->text == "@") {
                    std::string fname = varName;
                    if (functions_.count(fname)) {
                        builder_.CreateStore(functions_[fname], alloca);
                    }
                } else {
                    llvm::Value* initVal = codegenExpr(node->children[i + 1]);
                    if (initVal) builder_.CreateStore(initVal, alloca);
                }
            }
        }
        namedValues_[varName] = alloca;
        return;
    }

    // Template function declaration: IDENTIFIER := anonymous_template
    if (first && first->kind == "IDENTIFIER" && node->children.size() >= 3 &&
        node->children[1] && node->children[1]->kind == "ALIAS_PUBLIC") {
        AstPtr tmplNode = nullptr;
        for (size_t i = 2; i < node->children.size(); ++i) {
            if (node->children[i] && node->children[i]->kind == "anonymous_template") {
                tmplNode = node->children[i];
                break;
            }
        }
        if (tmplNode) {
            std::string funcName = first->text;
            if (templateFunctions_.count(funcName)) return; // already emitted
            llvm::Function* tmplFn = emitTemplateMetaFunc(funcName, tmplNode);
            if (tmplFn) {
                templateFunctions_[funcName] = tmplFn;
                namedValues_[funcName] = tmplFn;
            }
            return;
        }
    }

    // IDENTIFIER := @ (...) {...}  (function expression assigned via :=)
    if (node->children.size() == 3 && node->children[0] && node->children[0]->kind == "IDENTIFIER" &&
        node->children[1] && node->children[1]->kind == "ALIAS_PUBLIC" &&
        node->children[2] && node->children[2]->kind == "expr") {
        AstPtr rhsExpr = node->children[2];
        if (rhsExpr && rhsExpr->kind == "expr" && !rhsExpr->children.empty() &&
            rhsExpr->children[0] && rhsExpr->children[0]->text == "@") {
            std::string funcName = node->children[0]->text;
            llvm::Function* fn = emitTemplateMetaFunc(funcName, rhsExpr);
            if (fn) {
                templateFunctions_[funcName] = fn;
                namedValues_[funcName] = fn;
            }
        }
        return;
    }

    // Member function assignment: module.func = @ (...) {...}
    // e.g. IO["println"] = @(s) { at_fwrite(...) }
    // AST: stmt -> expr -> [IDENTIFIER(module), CHAR(46) [.], IDENTIFIER(func), CHAR(61) [=], expr(@ {...})]
    if (node->children.size() == 1 &&
        node->children[0] && node->children[0]->kind == "expr" &&
        node->children[0]->children.size() >= 5 &&
        node->children[0]->children[0] && node->children[0]->children[0]->kind == "IDENTIFIER" &&
        node->children[0]->children[1] && node->children[0]->children[1]->text == "." &&
        node->children[0]->children[2] && node->children[0]->children[2]->kind == "IDENTIFIER" &&
        node->children[0]->children[3] && node->children[0]->children[3]->kind == "CHAR(61)" &&
        node->children[0]->children[4] && node->children[0]->children[4]->kind == "expr") {
        // Extract module and function names
        std::string moduleName = node->children[0]->children[0]->text;
        std::string funcName = node->children[0]->children[2]->text;
        // Emit the RHS function expression as a template meta function
        AstPtr rhsExpr = node->children[0]->children[4];
        if (rhsExpr && rhsExpr->kind == "expr" && !rhsExpr->children.empty() &&
            rhsExpr->children[0] && rhsExpr->children[0]->text == "@") {
            llvm::Function* fn = emitTemplateMetaFunc(funcName, rhsExpr);
            if (fn) {
                templateFunctions_[funcName] = fn;
                namedValues_[funcName] = fn;
            }
        }
        return;
    }

    // Assignment: IDENTIFIER = expr
    // Note: node->children[0] is expr wrapping IDENTIFIER (e.g. "x = expr")
    if (node->children.size() == 3 && node->children[1] && node->children[1]->kind == "CHAR(61)") {
        // Extract variable name from expr-wrapped IDENTIFIER
        std::string varName;
        if (node->children[0] && node->children[0]->kind == "expr" &&
            node->children[0]->children.size() >= 1 &&
            node->children[0]->children[0] && node->children[0]->children[0]->kind == "IDENTIFIER") {
            varName = node->children[0]->children[0]->text;
        } else if (node->children[0] && node->children[0]->kind == "IDENTIFIER") {
            varName = node->children[0]->text;
        }
        if (varName.empty()) return;

        auto it = namedValues_.find(varName);
        if (it != namedValues_.end()) {
            llvm::Value* val = codegenExpr(node->children[2]);
            if (val) builder_.CreateStore(val, it->second);
        } else {
            // Check if RHS is an IMPORT expression (import 'file.at')
            AstPtr rhs = node->children[2];
            if (rhs && rhs->kind == "expr" && rhs->children.size() == 1 &&
                rhs->children[0] && rhs->children[0]->kind == "IMPORT") {
                std::string path = rhs->children[0]->text;
                if (path.size() >= 2 && (path.front() == '"' || path.front() == '\'')) {
                    path = path.substr(1, path.size() - 2);
                }
                // Register as imported module
                size_t slash = path.find_last_of("/\\");
                std::string modName = (slash != std::string::npos) ? path.substr(slash + 1) : path;
                size_t dot = modName.find_last_of('.');
                if (dot != std::string::npos) modName = modName.substr(0, dot);
                importedModules_[varName] = modName;

                // Create alloca and store the import path string
                llvm::Function* fn = builder_.GetInsertBlock()->getParent();
                llvm::AllocaInst* alloca = createEntryBlockAlloca(fn, varName, builder_.getInt64Ty());
                llvm::Value* pathVal = builder_.CreateGlobalStringPtr(path, "importstr");
                builder_.CreateStore(pathVal, alloca);
                namedValues_[varName] = alloca;
            } else {
                // Check if RHS is a function expression: @ (...) {...}
                AstPtr rhsExpr = node->children[2];
                if (rhsExpr && rhsExpr->kind == "expr" && !rhsExpr->children.empty() &&
                    rhsExpr->children[0] && rhsExpr->children[0]->text == "@") {
                    std::string funcName = varName;
                    llvm::Function* fn = emitTemplateMetaFunc(funcName, rhsExpr);
                    if (fn) {
                        templateFunctions_[funcName] = fn;
                        namedValues_[varName] = fn;
                    }
                } else {
                    // Simple assignment: create global variable, codegen RHS, store
                    llvm::GlobalVariable* gv = new llvm::GlobalVariable(
                        *module_, builder_.getInt64Ty(), false,
                        llvm::GlobalValue::ExternalLinkage,
                        llvm::ConstantInt::get(module_->getContext(), llvm::APInt(64, 0)), varName);
                    llvm::Value* rhsVal = codegenExpr(node->children[2]);
                    if (rhsVal) builder_.CreateStore(rhsVal, gv);
                    namedValues_[varName] = gv;
                }
            }
        }
        return;
    }

    // Return
    if (first && first->kind == "RETURN") {
        llvm::Value* retVal = builder_.getInt64(0);
        if (node->children.size() >= 2) {
            llvm::Value* v = codegenExpr(node->children[1]);
            if (v) retVal = v;
        }
        builder_.CreateRet(retVal);
        return;
    }

    // Unwrap: '*' '(' expr_list ')' '{' scope '}' [':' '{' scope '}']
    if (first && first->kind == "CHAR(42)" && node->children.size() >= 2 && node->children[1] && node->children[1]->kind == "CHAR(40)") {
        for (const auto& c : node->children) {
            if (c && c->kind == "scope") codegenStmt(c);
        }
        return;
    }

    // Try-cast: '?' '(' cast_stmt_list ')' '{' scope '}' [':' '{' scope '}']
    if (first && first->kind == "CHAR(63)" && node->children.size() >= 2 && node->children[1] && node->children[1]->kind == "CHAR(40)") {
        for (const auto& c : node->children) {
            if (c && c->kind == "scope") codegenStmt(c);
        }
        return;
    }

    // While: '$' '(' expr ')' '{' scope '}'
    if (first && first->kind == "CHAR(36)") {
        bool isFor = false;
        for (const auto& c : node->children) {
            if (c && c->kind == "IDENTIFIER") { isFor = true; break; }
        }
        if (isFor) {
            // For loop: $ ( IDENTIFIER : expr ) { scope }
            AstPtr idNode = nullptr;
            AstPtr exprNode = nullptr;
            for (const auto& c : node->children) {
                if (c && c->kind == "IDENTIFIER" && !idNode) idNode = c;
                if (c && c->kind == "expr" && !exprNode) exprNode = c;
            }
            llvm::Value* rangeVal = exprNode ? codegenExpr(exprNode) : nullptr;
            if (!rangeVal) return;
            llvm::Function* fn = builder_.GetInsertBlock()->getParent();
            llvm::BasicBlock* loopBB = llvm::BasicBlock::Create(context_, "forloop", fn);
            llvm::BasicBlock* bodyBB = llvm::BasicBlock::Create(context_, "forbody", fn);
            llvm::BasicBlock* endBB = llvm::BasicBlock::Create(context_, "forend", fn);
            llvm::Value* dataPtr = builder_.CreateIntToPtr(rangeVal, builder_.getPtrTy(), "forarr");
            llvm::Value* lenSlot = builder_.CreateGEP(builder_.getInt64Ty(), dataPtr, builder_.getInt64(-1), "forlen");
            llvm::Value* len = builder_.CreateLoad(builder_.getInt64Ty(), lenSlot, "forlenval");
            llvm::AllocaInst* counter = createEntryBlockAlloca(fn, "__fori", builder_.getInt64Ty());
            builder_.CreateStore(builder_.getInt64(0), counter);
            llvm::BasicBlock* contBB = bodyBB;
            llvm::BasicBlock* breakBB = endBB;
            breakStack_.push_back(breakBB);
            continueStack_.push_back(contBB);
            builder_.CreateBr(loopBB);
            builder_.SetInsertPoint(loopBB);
            llvm::Value* i = builder_.CreateLoad(builder_.getInt64Ty(), counter, "fori");
            llvm::Value* cond = builder_.CreateICmpSLT(i, len, "forcond");
            builder_.CreateCondBr(cond, bodyBB, endBB);
            builder_.SetInsertPoint(bodyBB);
            llvm::Value* slot = builder_.CreateGEP(builder_.getInt64Ty(), dataPtr, i, "forslot");
            llvm::Value* val = builder_.CreateLoad(builder_.getInt64Ty(), slot, "forval");
            if (idNode) {
                llvm::AllocaInst* varAlloca = createEntryBlockAlloca(fn, idNode->text, builder_.getInt64Ty());
                builder_.CreateStore(val, varAlloca);
                namedValues_[idNode->text] = varAlloca;
            }
            for (const auto& c : node->children) {
                if (c && c->kind == "scope") codegenStmt(c);
            }
            llvm::Value* i2 = builder_.CreateLoad(builder_.getInt64Ty(), counter, "fori2");
            llvm::Value* i3 = builder_.CreateAdd(i2, builder_.getInt64(1), "fori3");
            builder_.CreateStore(i3, counter);
            builder_.CreateBr(loopBB);
            builder_.SetInsertPoint(endBB);
            breakStack_.pop_back();
            continueStack_.pop_back();
            return;
        } else {
            // While loop: $ ( expr ) { scope }
            llvm::Function* fn = builder_.GetInsertBlock()->getParent();
            llvm::BasicBlock* loopBB = llvm::BasicBlock::Create(context_, "loop", fn);
            llvm::BasicBlock* loopEndBB = llvm::BasicBlock::Create(context_, "loopend", fn);
            llvm::Value* cond = nullptr;
            for (const auto& c : node->children) {
                if (c && c->kind == "expr") { cond = codegenExpr(c); break; }
            }
            if (!cond) cond = builder_.getInt64(1);
            cond = builder_.CreateICmpNE(cond, builder_.getInt64(0), "loopcond");
            llvm::BasicBlock* bodyBB = llvm::BasicBlock::Create(context_, "loopbody", fn);
            builder_.CreateCondBr(cond, bodyBB, loopEndBB);
            builder_.SetInsertPoint(bodyBB);
            for (const auto& c : node->children) {
                if (c && c->kind == "scope") codegenStmt(c);
            }
            builder_.CreateBr(loopBB);
            builder_.SetInsertPoint(loopEndBB);
            breakStack_.pop_back();
            continueStack_.pop_back();
            return;
        }
    }

    // Break
    if (first && first->kind == "BREAK") {
        if (!breakStack_.empty()) {
            builder_.CreateBr(breakStack_.back());
        }
        return;
    }

    // Continue
    if (first && first->kind == "CONTINUE") {
        if (!continueStack_.empty()) {
            builder_.CreateBr(continueStack_.back());
        }
        return;
    }

    // Default: execute expr statements
    for (const auto& c : node->children) {
        if (c && c->kind == "expr") {
            llvm::Value* v = codegenExpr(c);
            if (v) { }
        }
    }
    return;
}

} // namespace at
