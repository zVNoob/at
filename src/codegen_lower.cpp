#include "codegen_lower.h"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/MC/TargetRegistry.h>

#include <iostream>
#include <algorithm>

#include "../std_modules/at_sigs.h"

namespace at {

CodeGenLower::CodeGenLower(llvm::LLVMContext& ctx, const LowerConfig& cfg)
    : context_(ctx), builder_(context_), cfg_(cfg), rtti_(nullptr) {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
}

CodeGenLower::~CodeGenLower() = default;

llvm::AllocaInst* CodeGenLower::createEntryBlockAlloca(llvm::Function* function,
                                                       const std::string& varName,
                                                       llvm::Type* type) {
    llvm::IRBuilder<> tmpBuilder(&function->getEntryBlock(),
                                 function->getEntryBlock().begin());
    return tmpBuilder.CreateAlloca(type, nullptr, varName);
}

void CodeGenLower::declareRuntimeOps() {
    llvm::FunctionType* mallocType = llvm::FunctionType::get(
        builder_.getPtrTy(), {builder_.getInt64Ty()}, false);
    module_->getOrInsertFunction("at_malloc", mallocType);

    // Global to hold the last expression value (for interpreter compatibility)
    new llvm::GlobalVariable(
        *module_, builder_.getInt64Ty(), false,
        llvm::GlobalValue::ExternalLinkage,
        llvm::ConstantInt::get(module_->getContext(), llvm::APInt(64, 0)),
        "__at_last_value");

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

    // In JIT mode, declare at_native_dispatch for native function calls
    if (cfg_.jitMode) {
        llvm::FunctionType* dispatchType = llvm::FunctionType::get(
            builder_.getInt64Ty(),
            {builder_.getInt64Ty(),  // fnIndex
             builder_.getInt64Ty(),  // a0
             builder_.getInt64Ty(),  // a1
             builder_.getInt64Ty(),  // a2
             builder_.getInt64Ty(),  // a3
             builder_.getInt64Ty()}, // a4
            false);
        module_->getOrInsertFunction("at_native_dispatch", dispatchType);
    }
}

void CodeGenLower::pushScope() {
    scopeStack_.push_back(std::move(namedValues_));
    namedValues_.clear();
}

void CodeGenLower::popScope() {
    namedValues_ = std::move(scopeStack_.back());
    scopeStack_.pop_back();
}

void CodeGenLower::setNamedValue(const std::string& name, llvm::Value* val) {
    namedValues_[name] = val;
}

llvm::Value* CodeGenLower::getNamedValue(const std::string& name) const {
    auto it = namedValues_.find(name);
    if (it != namedValues_.end()) return it->second;
    return nullptr;
}

void CodeGenLower::collectStmts(const AstPtr& node, std::vector<AstPtr>& out) {
    if (!node) return;
    if (node->kind == "stmt") {
        out.push_back(node);
        return;
    }
    for (const auto& c : node->children) collectStmts(c, out);
}

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

std::unique_ptr<LoweredResult> CodeGenLower::lower(const AstPtr& root,
                                                    SemanticAnalyzer& sem,
                                                    bool isMainModule,
                                                    const std::vector<std::string>& initFuncs,
                                                    const std::string& initFuncName) {
    module_ = std::make_unique<llvm::Module>(cfg_.moduleName, context_);
    templateShapes_.clear();
    rtti_ = std::make_unique<RttiManager>(module_.get());
    declareRuntimeOps();

    std::vector<AstPtr> allStmts;
    collectStmts(root, allStmts);

    // First pass: emit classes
    for (const auto& s : allStmts) {
        if (s->children.size() >= 3 &&
            s->children[0] && s->children[0]->kind == "IDENTIFIER" &&
            s->children[1] && (s->children[1]->kind == "ALIAS_PUBLIC" || s->children[1]->kind == "ALIAS_PRIVATE")) {
            AstPtr classNode = nullptr;
            for (const auto& c : s->children) {
                if (c && c->kind == "class") { classNode = c; break; }
            }
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
            AstPtr tmplNode = nullptr;
            for (const auto& c : s->children) {
                if (c && c->kind == "anonymous_template") { tmplNode = c; break; }
            }
            if (tmplNode) {
                std::string funcName = s->children[0]->text;
                if (!templateFunctions_.count(funcName)) {
                    emitTemplateMetaFunc(funcName, tmplNode);
                }
            }
        }
    }

    llvm::BasicBlock* savedBB = builder_.GetInsertBlock();

    if (isMainModule) {
        llvm::FunctionType* mainType = llvm::FunctionType::get(builder_.getInt64Ty(), false);
        llvm::Function* mainFn = llvm::Function::Create(mainType, llvm::Function::ExternalLinkage, "main", module_.get());
        llvm::BasicBlock* mainEntry = llvm::BasicBlock::Create(context_, "entry", mainFn);
        builder_.SetInsertPoint(mainEntry);
        currentFunction_ = mainFn;

        for (const auto& initName : initFuncs) {
            if (auto* initFn = module_->getFunction(initName)) {
                builder_.CreateCall(initFn, {}, initName + "_call");
            } else {
                llvm::FunctionType* initType = llvm::FunctionType::get(builder_.getInt64Ty(), false);
                llvm::Function* extFn = llvm::Function::Create(initType, llvm::Function::ExternalLinkage, initName, module_.get());
                builder_.CreateCall(extFn, {}, initName + "_call");
            }
        }

        for (const auto& c : root->children) {
            if (c && c->kind == "scope") {
                for (const auto& s : c->children) {
                    lowerStmt(s);
                }
            }
        }
        if (!builder_.GetInsertBlock()->getTerminator()) {
            if (auto* gv = module_->getGlobalVariable("__at_last_value")) {
                builder_.CreateRet(builder_.CreateLoad(builder_.getInt64Ty(), gv, "lastval"));
            } else {
                builder_.CreateRet(builder_.getInt64(0));
            }
        }
        currentFunction_ = nullptr;
    } else {
        currentFunction_ = nullptr;

        llvm::FunctionType* initType = llvm::FunctionType::get(builder_.getInt64Ty(), false);
        llvm::Function* initFn = llvm::Function::Create(initType, llvm::Function::ExternalLinkage, initFuncName, module_.get());
        llvm::BasicBlock* initEntry = llvm::BasicBlock::Create(context_, "entry", initFn);
        builder_.SetInsertPoint(initEntry);
        currentFunction_ = initFn;

        for (const auto& c : root->children) {
            if (c && c->kind == "scope") {
                for (const auto& s : c->children) {
                    lowerStmt(s);
                }
            }
        }
        if (!builder_.GetInsertBlock()->getTerminator()) {
            if (auto* gv = module_->getGlobalVariable("__at_last_value")) {
                builder_.CreateRet(builder_.CreateLoad(builder_.getInt64Ty(), gv, "lastval"));
            } else {
                builder_.CreateRet(builder_.getInt64(0));
            }
        }
        currentFunction_ = nullptr;
    }

    if (savedBB) builder_.SetInsertPoint(savedBB);

    // Emit .at.sigs section
    {
        std::vector<BinSigDecl> decls = sigs_;
        if (!decls.empty()) {
            std::vector<uint8_t> bytes = BinSigWriter().write(decls);
            auto* init = llvm::ConstantDataArray::get(context_, llvm::ArrayRef<uint8_t>(bytes));
            auto* gv = new llvm::GlobalVariable(*module_, init->getType(), true,
                llvm::GlobalValue::PrivateLinkage, init, "__at_sigs");
            gv->setSection(".at.sigs");
            gv->setAlignment(llvm::Align(1));
        }
    }

    std::string targetTriple = cfg_.targetTriple.empty()
        ? llvm::sys::getProcessTriple()
        : cfg_.targetTriple;
    module_->setTargetTriple(targetTriple);

    std::string targetErr;
    auto target = llvm::TargetRegistry::lookupTarget(targetTriple, targetErr);
    if (target) {
        llvm::TargetOptions opt;
        auto rm = llvm::Reloc::Static;
        auto targetMachine = target->createTargetMachine(targetTriple, "generic", "", opt, rm);
        if (targetMachine) {
            module_->setDataLayout(targetMachine->createDataLayout());
        }
    }

    auto result = std::make_unique<LoweredResult>();
    result->module = std::move(module_);
    result->mainSymbol = isMainModule ? "main" : initFuncName;

    for (auto& f : result->module->functions()) {
        if (!f.isDeclaration()) {
            result->exportedSymbols.push_back(f.getName().str());
        }
    }

    return result;
}

//------------------------------------------------------------------------------
// Expression lowering
//------------------------------------------------------------------------------

llvm::Value* CodeGenLower::lowerExpr(const AstPtr& node) {
    if (!node) return nullptr;
    if (node->kind == "expr" && node->children.size() == 1) {
        return lowerExpr(node->children[0]);
    }
    if (node->kind == "INTEGER") {
        try { return builder_.getInt64(std::stoll(node->text)); }
        catch (...) { return builder_.getInt64(0); }
    }
    if (node->kind == "DECIMAL") {
        try { return llvm::ConstantFP::get(builder_.getContext(), llvm::APFloat(std::stod(node->text))); }
        catch (...) { return llvm::ConstantFP::get(builder_.getContext(), llvm::APFloat(0.0)); }
    }
    if (node->kind == "STRING") {
        std::string s = node->text;
        if (s.size() >= 2 && s.front() == '"' && s.back() == '"') {
            s = s.substr(1, s.size() - 2);
        }
        std::string result;
        result.reserve(s.size());
        for (size_t i = 0; i < s.size(); ++i) {
            if (s[i] == '\\' && i + 1 < s.size()) {
                char next = s[++i];
                switch (next) {
                    case 'n': result.push_back('\n'); break;
                    case 't': result.push_back('\t'); break;
                    case 'r': result.push_back('\r'); break;
                    case '\\': result.push_back('\\'); break;
                    case '"': result.push_back('"'); break;
                    case '\'': result.push_back('\''); break;
                    default: result.push_back('\\'); result.push_back(next); break;
                }
            } else {
                result.push_back(s[i]);
            }
        }
        s = result;
        llvm::Value* strPtr = builder_.CreateGlobalStringPtr(s, "str");
        return builder_.CreatePtrToInt(strPtr, builder_.getInt64Ty(), "strint");
    }
    if (node->kind == "IDENTIFIER") {
        auto pit = pinnedAddrs_.find(node->text);
        if (pit != pinnedAddrs_.end()) {
            llvm::Value* addr = builder_.getInt64(pit->second);
            llvm::Value* p = builder_.CreateIntToPtr(addr, builder_.getPtrTy(), "pinptr");
            return builder_.CreateLoad(builder_.getInt64Ty(), p, "pinload");
        }
        auto it = namedValues_.find(node->text);
        if (it != namedValues_.end()) {
            return builder_.CreateLoad(builder_.getInt64Ty(), it->second, node->text);
        }
        return builder_.getInt64(0);
    }
    if (node->children.size() == 3 && node->children[1]) {
        std::string op = node->children[1]->text;
        if (op == "+" || op == "-" || op == "*" || op == "/" || op == "%" ||
            op == "^" || op == "|" || op == "&" ||
            op == "==" || op == "!=" || op == "<" || op == "<=" || op == ">" || op == ">=" ||
            op == "&&" || op == "||" || op == "^^") {
            llvm::Value* l = lowerExpr(node->children[0]);
            llvm::Value* r = lowerExpr(node->children[2]);
            if (!l || !r) return nullptr;
            if (op == "+") return builder_.CreateAdd(l, r, "add");
            if (op == "-") return builder_.CreateSub(l, r, "sub");
            if (op == "*") return builder_.CreateMul(l, r, "mul");
            if (op == "/") return builder_.CreateSDiv(l, r, "div");
            if (op == "%") return builder_.CreateSRem(l, r, "rem");
            if (op == "==") return builder_.CreateICmpEQ(l, r, "eq");
            if (op == "!=") return builder_.CreateICmpNE(l, r, "ne");
            if (op == "<")  return builder_.CreateICmpSLT(l, r, "lt");
            if (op == "<=") return builder_.CreateICmpSLE(l, r, "le");
            if (op == ">")  return builder_.CreateICmpSGT(l, r, "gt");
            if (op == ">=") return builder_.CreateICmpSGE(l, r, "ge");
            if (op == "&&") return builder_.CreateSelect(builder_.CreateICmpNE(l, builder_.getInt64(0), "l"),
                                builder_.CreateSelect(builder_.CreateICmpNE(r, builder_.getInt64(0), "r"),
                                    builder_.getInt64(1), builder_.getInt64(0)), builder_.getInt64(0));
            if (op == "||") return builder_.CreateSelect(builder_.CreateICmpNE(l, builder_.getInt64(0), "l"),
                                builder_.getInt64(1),
                                builder_.CreateSelect(builder_.CreateICmpNE(r, builder_.getInt64(0), "r"),
                                    builder_.getInt64(1), builder_.getInt64(0)));
            if (op == "^^") return builder_.CreateXor(l, r, "xor");
            if (op == "&") return builder_.CreateAnd(l, r, "band");
            if (op == "|") return builder_.CreateOr(l, r, "bor");
            if (op == "^") return builder_.CreateXor(l, r, "bxor");
        }
    }

    {
        bool hasDot = false;
        for (const auto& c : node->children) if (c && c->text == ".") { hasDot = true; break; }
        if (hasDot) {
            bool hasCall = false;
            for (const auto& c : node->children) if (c && c->text == "(") { hasCall = true; break; }
            if (!hasCall) {
                AstPtr baseNode = nullptr;
                std::string memberName;
                for (const auto& c : node->children) {
                    if (c && c->kind == "expr" && !baseNode) baseNode = c;
                    if (c && c->kind == "IDENTIFIER") memberName = c->text;
                }
                llvm::Value* base = baseNode ? lowerExpr(baseNode) : nullptr;
                if (!base || memberName.empty()) return nullptr;

                if (baseNode && importedModules_.count(baseNode->children[0]->text)) {
                    std::string modName = importedModules_[baseNode->children[0]->text];
                    if (auto* fn = module_->getFunction(memberName)) {
                        std::vector<llvm::Value*> args;
                        for (const auto& c : node->children) {
                            if (c && c->kind == "expr" && c != baseNode) {
                                llvm::Value* v = lowerExpr(c);
                                if (v) args.push_back(v);
                            }
                        }
                        return builder_.CreateCall(fn, args, memberName);
                    }
                }
                return builder_.getInt64(0);
            }
        }
    }

    if (!node->children.empty() && node->children[0] && node->children[0]->text == "@") {
        llvm::Function* fn = emitTemplateMetaFunc(node->text, node);
        if (fn) {
            return builder_.CreatePtrToInt(fn, builder_.getInt64Ty(), "fnptr");
        }
    }

    {
        bool hasCall = false;
        for (const auto& c : node->children) if (c && c->text == "(") { hasCall = true; break; }
        if (hasCall) {
            std::string calleeName;
            if (node->children.size() >= 2 &&
                node->children[0] && node->children[0]->kind == "expr") {
                AstPtr baseExpr = node->children[0];
                bool hasDot = false;
                for (const auto& c : baseExpr->children) {
                    if (c && c->text == ".") { hasDot = true; break; }
                }
                if (hasDot) {
                    std::string moduleName;
                    std::string funcName;
                    for (const auto& c : baseExpr->children) {
                        if (c && c->kind == "expr") {
                            for (const auto& bc : c->children) {
                                if (bc && bc->kind == "IDENTIFIER") {
                                    moduleName = bc->text;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                    bool foundDot = false;
                    for (const auto& c : baseExpr->children) {
                        if (c && c->text == ".") { foundDot = true; continue; }
                        if (foundDot && c && c->kind == "IDENTIFIER") {
                            funcName = c->text;
                            break;
                        }
                    }
                    auto it = importedModules_.find(moduleName);
                    if (it != importedModules_.end() && !funcName.empty()) {
                        calleeName = funcName;
                    }
                }
            }
            if (calleeName.empty()) {
                if (node->children[0] && node->children[0]->kind == "expr" && !node->children[0]->children.empty()) {
                    if (node->children[0]->children[0] && node->children[0]->children[0]->kind == "IDENTIFIER") {
                        calleeName = node->children[0]->children[0]->text;
                    }
                }
                if (calleeName.empty()) {
                    for (const auto& c : node->children) {
                        if (c && c->kind == "IDENTIFIER") { calleeName = c->text; break; }
                    }
                }
            }
            std::vector<llvm::Value*> args;
            for (const auto& c : node->children) {
                if (c && c->kind == "expr") {
                    if (c == node->children[0]) continue;
                    if (c->children.size() == 1 && c->children[0] && c->children[0]->kind == "IDENTIFIER" && c->children[0]->text == calleeName) continue;
                    llvm::Value* v = lowerExpr(c);
                    if (v) args.push_back(v);
                }
                if (c && c->kind == "expr_list") {
                    for (const auto& a : c->children) {
                        if (a && a->kind == "expr") {
                            llvm::Value* v = lowerExpr(a);
                            if (v) args.push_back(v);
                        }
                    }
                }
            }
            if (auto* fn = module_->getFunction(mangleName(calleeName))) {
                return builder_.CreateCall(fn, args, "call");
            }
            // Check namedValues_ for template meta functions (e.g., f_meta stored as "f")
            if (auto it = namedValues_.find(calleeName); it != namedValues_.end()) {
                if (auto* fn = llvm::dyn_cast<llvm::Function>(it->second)) {
                    return builder_.CreateCall(fn, args, "call");
                }
            }
            // In JIT mode, call native functions via at_native_dispatch trampoline
            if (cfg_.jitMode) {
                auto it = cfg_.nativeFnIndices.find(calleeName);
                if (it != cfg_.nativeFnIndices.end()) {
                    int64_t fnIndex = it->second;
                    std::vector<llvm::Value*> dispatchArgs;
                    dispatchArgs.push_back(builder_.getInt64(fnIndex));
                    // Pad args to 5 (a0..a4), zero-extend each to i64
                    for (size_t i = 0; i < 5; ++i) {
                        if (i < args.size()) {
                            dispatchArgs.push_back(args[i]);
                        } else {
                            dispatchArgs.push_back(builder_.getInt64(0));
                        }
                    }
                    llvm::FunctionType* dispatchType = llvm::FunctionType::get(
                        builder_.getInt64Ty(),
                        {builder_.getInt64Ty(), builder_.getInt64Ty(), builder_.getInt64Ty(),
                         builder_.getInt64Ty(), builder_.getInt64Ty(), builder_.getInt64Ty()},
                        false);
                    llvm::Function* dispatchFn = module_->getFunction("at_native_dispatch");
                    if (!dispatchFn) {
                        dispatchFn = llvm::Function::Create(dispatchType,
                            llvm::Function::ExternalLinkage, "at_native_dispatch", module_.get());
                    }
                    return builder_.CreateCall(dispatchFn, dispatchArgs, "native_call");
                }
            }
            if (!calleeName.empty()) {
                std::string mangled = mangleName(calleeName);
                llvm::FunctionType* ft = llvm::FunctionType::get(builder_.getInt64Ty(), true);
                llvm::Function* fn = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, mangled, module_.get());
                return builder_.CreateCall(fn, args, "call");
            }
        }
    }

    if (!node->children.empty() && node->children[0] && node->children[0]->text == "[") {
        std::vector<llvm::Value*> elems;
        for (const auto& c : node->children) {
            if (c && c->kind == "expr") {
                llvm::Value* v = lowerExpr(c);
                if (!v) return nullptr;
                elems.push_back(v);
            }
        }
        if (elems.empty()) {
            return builder_.getInt64(0);
        }
        llvm::Value* n = builder_.getInt64(elems.size());
        llvm::Value* size = builder_.CreateMul(n, builder_.getInt64(8), "arrsize");
        llvm::Function* mallocFn = module_->getFunction("at_malloc");
        if (!mallocFn) {
            llvm::FunctionType* mallocType = llvm::FunctionType::get(builder_.getPtrTy(), {builder_.getInt64Ty()}, false);
            mallocFn = llvm::Function::Create(mallocType, llvm::Function::ExternalLinkage, "at_malloc", module_.get());
        }
        llvm::Value* rawPtr = builder_.CreateCall(mallocFn, {size}, "arrptr");
        llvm::Value* arrPtr = builder_.CreateBitCast(rawPtr, builder_.getPtrTy(), "arr");
        for (size_t i = 0; i < elems.size(); ++i) {
            llvm::Value* idx = builder_.getInt64(i);
            llvm::Value* slot = builder_.CreateGEP(builder_.getInt64Ty(), arrPtr, idx, "slot");
            builder_.CreateStore(elems[i], slot);
        }
        return builder_.CreatePtrToInt(arrPtr, builder_.getInt64Ty(), "arrint");
    }

    if (!node->children.empty() && node->children[0] && node->children[0]->text == "[") {
        AstPtr baseNode = nullptr;
        AstPtr idxNode = nullptr;
        for (const auto& c : node->children) {
            if (c && c->kind == "expr") {
                if (!baseNode) baseNode = c;
                else if (!idxNode) idxNode = c;
            }
        }
        llvm::Value* base = baseNode ? lowerExpr(baseNode) : nullptr;
        if (!base) return nullptr;
        llvm::Value* idx = idxNode ? lowerExpr(idxNode) : builder_.getInt64(0);
        if (!idx) return nullptr;
        llvm::Value* ptr = builder_.CreateIntToPtr(base, builder_.getPtrTy(), "arrptr");
        llvm::Value* slot = builder_.CreateGEP(builder_.getInt64Ty(), ptr, idx, "idxslot");
        return builder_.CreateLoad(builder_.getInt64Ty(), slot, "idxval");
    }

    if (!node->children.empty() && node->children[0] && node->children[0]->text == "<{") {
        std::string className;
        for (const auto& c : node->children) {
            if (c && c->kind == "IDENTIFIER") { className = c->text; break; }
        }
        std::vector<llvm::Value*> args;
        AstPtr argsNode = nullptr;
        for (const auto& c : node->children) {
            if (c && c->kind == "expr_list") { argsNode = c; break; }
        }
        if (argsNode) {
            for (const auto& a : argsNode->children) {
                if (a && a->kind == "expr") {
                    llvm::Value* arg = lowerExpr(a);
                    if (arg) args.push_back(arg);
                }
            }
            if (functions_.count(className)) {
                return builder_.CreateCall(functions_[className], args, "newobj");
            }
            if (auto* fn = module_->getFunction(className)) {
                return builder_.CreateCall(fn, args, "newobj");
            }
            return builder_.getInt64(0);
        }
    }

    for (const auto& c : node->children) {
        if (c && c->kind == "expr") {
            llvm::Value* v = lowerExpr(c);
            if (v) return v;
        }
    }
    return nullptr;
}

//------------------------------------------------------------------------------
// Statement lowering
//------------------------------------------------------------------------------

void CodeGenLower::lowerStmt(const AstPtr& node) {
    if (!node) return;

    if (node->kind == "scope" || node->kind == "start") {
        for (const auto& c : node->children) lowerStmt(c);
        return;
    }
    if (node->kind != "stmt") return;

    if (node->children.empty()) return;
    const AstPtr& first = node->children[0];
    if (first && first->kind == "CHAR(123)") {
        auto savedNamed = namedValues_;
        for (const auto& c : node->children) lowerStmt(c);
        namedValues_ = std::move(savedNamed);
        return;
    }

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
                    // Function literal assignment: @ (...) {...}
                    std::string fname = varName;
                    llvm::Function* tmplFn = emitTemplateMetaFunc(fname, initNode);
                    if (tmplFn) {
                        templateFunctions_[fname] = tmplFn;
                        namedValues_[fname] = tmplFn;
                        builder_.CreateStore(tmplFn, alloca);
                    }
                } else {
                    llvm::Value* initVal = lowerExpr(node->children[i + 1]);
                    if (initVal) builder_.CreateStore(initVal, alloca);
                }
            }
        }
        namedValues_[varName] = alloca;
        return;
    }

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
            if (templateFunctions_.count(funcName)) return;
            llvm::Function* tmplFn = emitTemplateMetaFunc(funcName, tmplNode);
            if (tmplFn) {
                templateFunctions_[funcName] = tmplFn;
                namedValues_[funcName] = tmplFn;
            }
            return;
        }
    }

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

    if (node->children.size() == 1 &&
        node->children[0] && node->children[0]->kind == "expr" &&
        node->children[0]->children.size() >= 5 &&
        node->children[0]->children[0] && node->children[0]->children[0]->kind == "IDENTIFIER" &&
        node->children[0]->children[1] && node->children[0]->children[1]->text == "." &&
        node->children[0]->children[2] && node->children[0]->children[2]->kind == "IDENTIFIER" &&
        node->children[0]->children[3] && node->children[0]->children[3]->kind == "CHAR(61)" &&
        node->children[0]->children[4] && node->children[0]->children[4]->kind == "expr") {
        std::string funcName = node->children[0]->children[2]->text;
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

    if (node->children.size() == 3 && node->children[1] && node->children[1]->kind == "CHAR(61)") {
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
            llvm::Value* val = lowerExpr(node->children[2]);
            if (val) builder_.CreateStore(val, it->second);
        } else {
            AstPtr rhs = node->children[2];
            if (rhs && rhs->kind == "expr" && rhs->children.size() == 1 &&
                rhs->children[0] && rhs->children[0]->kind == "IMPORT") {
                std::string path = rhs->children[0]->text;
                if (path.size() >= 2 && (path.front() == '"' || path.front() == '\'')) {
                    path = path.substr(1, path.size() - 2);
                }
                size_t slash = path.find_last_of("/\\");
                std::string modName = (slash != std::string::npos) ? path.substr(slash + 1) : path;
                size_t dot = modName.find_last_of('.');
                if (dot != std::string::npos) modName = modName.substr(0, dot);
                importedModules_[varName] = modName;

                llvm::Function* fn = builder_.GetInsertBlock()->getParent();
                llvm::AllocaInst* alloca = createEntryBlockAlloca(fn, varName, builder_.getInt64Ty());
                llvm::Value* pathVal = builder_.CreateGlobalStringPtr(path, "importstr");
                builder_.CreateStore(pathVal, alloca);
                namedValues_[varName] = alloca;
            } else {
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
                    llvm::GlobalVariable* gv = new llvm::GlobalVariable(
                        *module_, builder_.getInt64Ty(), false,
                        llvm::GlobalValue::ExternalLinkage,
                        llvm::ConstantInt::get(module_->getContext(), llvm::APInt(64, 0)), varName);
                    llvm::Value* rhsVal = lowerExpr(node->children[2]);
                    if (rhsVal) builder_.CreateStore(rhsVal, gv);
                    namedValues_[varName] = gv;
                }
            }
        }
        return;
    }

    if (first && first->kind == "RETURN") {
        llvm::Value* retVal = builder_.getInt64(0);
        if (node->children.size() >= 2) {
            llvm::Value* v = lowerExpr(node->children[1]);
            if (v) retVal = v;
        }
        builder_.CreateRet(retVal);
        return;
    }

    if (first && first->kind == "CHAR(42)" && node->children.size() >= 2 && node->children[1] && node->children[1]->kind == "CHAR(40)") {
        for (const auto& c : node->children) {
            if (c && c->kind == "scope") lowerStmt(c);
        }
        return;
    }

    if (first && first->kind == "CHAR(63)" && node->children.size() >= 2 && node->children[1] && node->children[1]->kind == "CHAR(40)") {
        for (const auto& c : node->children) {
            if (c && c->kind == "scope") lowerStmt(c);
        }
        return;
    }

    if (first && first->kind == "CHAR(36)") {
        bool isFor = false;
        for (const auto& c : node->children) {
            if (c && c->kind == "IDENTIFIER") { isFor = true; break; }
        }
        if (isFor) {
            AstPtr idNode = nullptr;
            AstPtr exprNode = nullptr;
            for (const auto& c : node->children) {
                if (c && c->kind == "IDENTIFIER" && !idNode) idNode = c;
                if (c && c->kind == "expr" && !exprNode) exprNode = c;
            }
            llvm::Value* rangeVal = exprNode ? lowerExpr(exprNode) : nullptr;
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
                if (c && c->kind == "scope") lowerStmt(c);
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
            llvm::Function* fn = builder_.GetInsertBlock()->getParent();
            llvm::BasicBlock* loopBB = llvm::BasicBlock::Create(context_, "loop", fn);
            llvm::BasicBlock* loopEndBB = llvm::BasicBlock::Create(context_, "loopend", fn);
            llvm::Value* cond = nullptr;
            for (const auto& c : node->children) {
                if (c && c->kind == "expr") { cond = lowerExpr(c); break; }
            }
            if (!cond) cond = builder_.getInt64(1);
            cond = builder_.CreateICmpNE(cond, builder_.getInt64(0), "loopcond");
            llvm::BasicBlock* bodyBB = llvm::BasicBlock::Create(context_, "loopbody", fn);
            builder_.CreateCondBr(cond, bodyBB, loopEndBB);
            builder_.SetInsertPoint(bodyBB);
            for (const auto& c : node->children) {
                if (c && c->kind == "scope") lowerStmt(c);
            }
            builder_.CreateBr(loopBB);
            builder_.SetInsertPoint(loopEndBB);
            breakStack_.pop_back();
            continueStack_.pop_back();
            return;
        }
    }

    if (first && first->kind == "BREAK") {
        if (!breakStack_.empty()) {
            builder_.CreateBr(breakStack_.back());
        }
        return;
    }

    if (first && first->kind == "CONTINUE") {
        if (!continueStack_.empty()) {
            builder_.CreateBr(continueStack_.back());
        }
        return;
    }

    for (const auto& c : node->children) {
        if (c && c->kind == "expr") {
            llvm::Value* v = lowerExpr(c);
            if (v) {
                if (auto* gv = module_->getGlobalVariable("__at_last_value")) {
                    builder_.CreateStore(v, gv);
                }
            }
        }
    }
    return;
}

//------------------------------------------------------------------------------
// Template meta functions, classes
//------------------------------------------------------------------------------

llvm::Function* CodeGenLower::emitTemplateMetaFunc(const std::string& name, const AstPtr& tmplNode) {
    llvm::BasicBlock* savedBB = builder_.GetInsertBlock();
    AstPtr tpNode = findChild(tmplNode, "template_arg_declare_list");
    unsigned numTmplParams = 0;
    if (tpNode) {
        for (const auto& p : tpNode->children) {
            if (p && p->kind == "IDENTIFIER") ++numTmplParams;
        }
    }

    AstPtr funcExpr = nullptr;
    // @ can be a direct child of anonymous_template (CHAR(64)) or wrapped in expr
    for (const auto& c : tmplNode->children) {
        if (c && c->text == "@") {
            funcExpr = tmplNode;  // Use tmplNode directly as the funcExpr
            break;
        }
        if (c && c->kind == "expr") {
            if (!c->children.empty() && c->children[0] && c->children[0]->text == "@") {
                funcExpr = c;
                break;
            }
        }
    }
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
    // Template meta functions are emitted as <name>_meta ELF symbols,
    // matching elfSymbolNameFor() in sig_bridge.cpp
    // Regular functions (no template params) use just <name>.
    std::string mangledName = numTmplParams > 0 ? name + "_meta" : name;
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

    AstPtr bodyNode = findChild(funcExpr, "scope");
    if (bodyNode) {
        lowerStmt(bodyNode);
    }

    if (!builder_.GetInsertBlock()->getTerminator()) {
        builder_.CreateRet(builder_.getInt64(0));
    }
    currentFunction_ = nullptr;
    if (savedBB) builder_.SetInsertPoint(savedBB);
    return fn;
}

llvm::Function* CodeGenLower::emitFunction(const std::string& name, const AstPtr& funcExpr) {
    (void)name; (void)funcExpr;
    return nullptr;
}

llvm::Function* CodeGenLower::emitClass(const std::string& name, const AstPtr& classNode) {
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

llvm::Type* CodeGenLower::llvmTypeForAst(const AstPtr& typeNode) {
    (void)typeNode;
    return builder_.getInt64Ty();
}

llvm::Constant* CodeGenLower::getRttiForType(const AstPtr& typeNode) {
    (void)typeNode;
    return nullptr;
}

} // namespace at
