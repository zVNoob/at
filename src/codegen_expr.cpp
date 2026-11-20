#include "codegen.h"
#include "ast.h"

namespace at {

llvm::Value* CodeGen::codegenExpr(const AstPtr& node) {
    if (!node) return nullptr;
    if (node->kind == "expr" && node->children.size() == 1) {
        return codegenExpr(node->children[0]);
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
        // Process escape sequences: \n \t \\ \" \' \r
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
        // All values in AT are i64; bitcast pointer to i64
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
    // binary ops
    if (node->children.size() == 3 && node->children[1]) {
        std::string op = node->children[1]->text;
        if (op == "+" || op == "-" || op == "*" || op == "/" || op == "%" ||
            op == "^" || op == "|" || op == "&" ||
            op == "==" || op == "!=" || op == "<" || op == "<=" || op == ">" || op == ">=" ||
            op == "&&" || op == "||" || op == "^^") {
            llvm::Value* l = codegenExpr(node->children[0]);
            llvm::Value* r = codegenExpr(node->children[2]);
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

    // Dot member access: base.IDENTIFIER
    {
        bool hasDot = false;
        for (const auto& c : node->children) if (c && c->text == ".") { hasDot = true; break; }
        if (hasDot) {
            // Skip if this is also a call expression (module.func(args)) —
            // let the Call handler below deal with it.
            bool hasCall = false;
            for (const auto& c : node->children) if (c && c->text == "(") { hasCall = true; break; }
            if (hasCall) {
                // fall through to Call handler
            } else {
            AstPtr baseNode = nullptr;
            std::string memberName;
            for (const auto& c : node->children) {
                if (c && c->kind == "expr" && !baseNode) baseNode = c;
                if (c && c->kind == "IDENTIFIER") memberName = c->text;
            }
            llvm::Value* base = baseNode ? codegenExpr(baseNode) : nullptr;
            if (!base || memberName.empty()) return nullptr;

            // Check if base is an imported module (function call)
            if (importedModules_.count(baseNode->children[0]->text)) {
                std::string modName = importedModules_[baseNode->children[0]->text];
                // Look up the method as a global function
                if (auto* fn = module_->getFunction(memberName)) {
                    std::vector<llvm::Value*> args;
                    for (const auto& c : node->children) {
                        if (c && c->kind == "expr" && c != baseNode) {
                            llvm::Value* v = codegenExpr(c);
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

    // Function expression as value: @ (...) {...}
    if (!node->children.empty() && node->children[0] && node->children[0]->text == "@") {
        llvm::Function* fn = emitTemplateMetaFunc(node->text, node);
        if (fn) {
            // Return function pointer as i64
            return builder_.CreatePtrToInt(fn, builder_.getInt64Ty(), "fnptr");
        }
    }

    // Call: callee '(' args ')'
    {
        bool hasCall = false;
        for (const auto& c : node->children) if (c && c->text == "(") { hasCall = true; break; }
        if (hasCall) {
            std::string calleeName;
            // Check for module.function() call where module is an imported module IDENTIFIER
            if (node->children.size() >= 2 &&
                node->children[0] && node->children[0]->kind == "expr") {
                AstPtr baseExpr = node->children[0];
                // baseExpr should be a dot access: module.func
                bool hasDot = false;
                for (const auto& c : baseExpr->children) {
                    if (c && c->text == ".") { hasDot = true; break; }
                }
                if (hasDot) {
                    // baseExpr structure: [expr(module)] [CHAR(46) "."] [IDENTIFIER(func)]
                    std::string moduleName;
                    std::string funcName;
                    // Extract module name from the base expr child
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
                    // Extract function name (IDENTIFIER after the dot)
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
                // Extract callee name from the first child (callee expression)
                // The IDENTIFIER is nested inside expr: expr(IDENTIFIER(name))
                if (node->children[0] && node->children[0]->kind == "expr" && !node->children[0]->children.empty()) {
                    if (node->children[0]->children[0] && node->children[0]->children[0]->kind == "IDENTIFIER") {
                        calleeName = node->children[0]->children[0]->text;
                    }
                }
                // Fallback: look for direct IDENTIFIER child
                if (calleeName.empty()) {
                    for (const auto& c : node->children) {
                        if (c && c->kind == "IDENTIFIER") { calleeName = c->text; break; }
                    }
                }
            }
            std::vector<llvm::Value*> args;
            for (const auto& c : node->children) {
                if (c && c->kind == "expr") {
                    // Skip the callee expression (module.func) — it's not an argument
                    if (c == node->children[0]) continue;
                    if (c->children.size() == 1 && c->children[0] && c->children[0]->kind == "IDENTIFIER" && c->children[0]->text == calleeName) continue;
                    llvm::Value* v = codegenExpr(c);
                    if (v) args.push_back(v);
                }
                if (c && c->kind == "expr_list") {
                    for (const auto& a : c->children) {
                        if (a && a->kind == "expr") {
                            llvm::Value* v = codegenExpr(a);
                            if (v) args.push_back(v);
                        }
                    }
                }
            }
            if (auto* fn = module_->getFunction(mangleName(calleeName))) {
                return builder_.CreateCall(fn, args, "call");
            }
            // Declare as external function (imported from another module .o/.so)
            if (!calleeName.empty()) {
                std::string mangled = mangleName(calleeName);
                llvm::FunctionType* ft = llvm::FunctionType::get(builder_.getInt64Ty(), true); // varargs
                llvm::Function* fn = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, mangled, module_.get());
                return builder_.CreateCall(fn, args, "call");
            }
        }
    }

    // Array literal: [e1, e2, ...]
    if (!node->children.empty() && node->children[0] && node->children[0]->text == "[") {
        std::vector<llvm::Value*> elems;
        for (const auto& c : node->children) {
            if (c && c->kind == "expr") {
                llvm::Value* v = codegenExpr(c);
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

    // Indexing: base[ index ]
    if (!node->children.empty() && node->children[0] && node->children[0]->text == "[") {
        AstPtr baseNode = nullptr;
        AstPtr idxNode = nullptr;
        for (const auto& c : node->children) {
            if (c && c->kind == "expr") {
                if (!baseNode) baseNode = c;
                else if (!idxNode) idxNode = c;
            }
        }
        llvm::Value* base = baseNode ? codegenExpr(baseNode) : nullptr;
        if (!base) return nullptr;
        llvm::Value* idx = idxNode ? codegenExpr(idxNode) : builder_.getInt64(0);
        if (!idx) return nullptr;
        llvm::Value* ptr = builder_.CreateIntToPtr(base, builder_.getPtrTy(), "arrptr");
        llvm::Value* slot = builder_.CreateGEP(builder_.getInt64Ty(), ptr, idx, "idxslot");
        return builder_.CreateLoad(builder_.getInt64Ty(), slot, "idxval");
    }

    // Construction: IDENTIFIER <{ expr_list }>
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
                    llvm::Value* arg = codegenExpr(a);
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

    // Default
    for (const auto& c : node->children) {
        if (c && c->kind == "expr") {
            llvm::Value* v = codegenExpr(c);
            if (v) return v;
        }
    }
    return nullptr;
}

} // namespace at
