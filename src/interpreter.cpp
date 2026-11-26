#include "at_runtime.h"
#include "interpreter.h"
#include "parser.h"
#include "semantic.h"
#include "codegen_lower.h"

#include <cstdlib>
#include <cstring>
#include <dlfcn.h>
#include <functional>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <map>
#include <vector>
#include <mutex>

#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/ExecutionEngine/Orc/CompileUtils.h>
#include <llvm/ExecutionEngine/Orc/IRCompileLayer.h>
#include <llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h>
#include <llvm/ExecutionEngine/Orc/EPCIndirectionUtils.h>
#include <llvm/ExecutionEngine/Orc/ExecutorProcessControl.h>
#include <llvm/ExecutionEngine/Orc/IRTransformLayer.h>
#include <llvm/ExecutionEngine/Orc/CompileOnDemandLayer.h>
#include <llvm/ExecutionEngine/Orc/ExecutionUtils.h>
#include <llvm/ExecutionEngine/Orc/Shared/ExecutorAddress.h>
#include <llvm/ExecutionEngine/SectionMemoryManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/DynamicLibrary.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>

namespace at {

//------------------------------------------------------------------------------
// Forward declaration
//------------------------------------------------------------------------------
static Value makeNativeFFIFn(void* addr);

//------------------------------------------------------------------------------
// Global native function registry (for JIT dispatch)
//------------------------------------------------------------------------------

static std::map<std::string, NativeFn>* g_nativeFns = nullptr;

extern "C" int64_t at_native_dispatch(int64_t fnIndex, int64_t a0, int64_t a1,
                                     int64_t a2, int64_t a3, int64_t a4) {
    if (!g_nativeFns) return 0;
    auto it = g_nativeFns->begin();
    std::advance(it, (size_t)fnIndex);
    if (it == g_nativeFns->end()) return 0;

    auto& fn = it->second;
    std::vector<Value> args;
    auto tv = [](int64_t v) -> Value {
        Value val; val.kind = Value::Kind::Int; val.intValue = v; return val;
    };
    args.push_back(tv(a0));
    args.push_back(tv(a1));
    args.push_back(tv(a2));
    args.push_back(tv(a3));
    args.push_back(tv(a4));
    Value result = fn(args);
    return result.intValue.toInt64();
}

//------------------------------------------------------------------------------
// Environment
//------------------------------------------------------------------------------

Value Environment::get(const std::string& name) const {
    auto it = vars.find(name);
    if (it != vars.end()) return it->second;
    if (parent) return parent->get(name);
    return Value{};
}

void Environment::set(const std::string& name, const Value& value) {
    auto it = vars.find(name);
    if (it != vars.end()) { it->second = value; return; }
    if (parent) {
        if (parent->vars.count(name)) {
            parent->set(name, value);
            return;
        }
    }
    vars[name] = value;
}

void Environment::define(const std::string& name, const Value& value) {
    vars[name] = value;
}

//------------------------------------------------------------------------------
// Interpreter implementation (ORC JIT)
//------------------------------------------------------------------------------

struct Interpreter::JitImpl {
    std::unique_ptr<llvm::orc::LLJIT> jit;
    bool initialized = false;
};

Interpreter::Interpreter()
    : globals_(std::make_shared<Environment>())
    , current_(globals_)
    , jitContext_(std::make_unique<llvm::LLVMContext>())
    , lower_(std::make_unique<CodeGenLower>(*jitContext_, LowerConfig{
        .moduleName = "at_interp_module",
    }))
{
    if (!g_nativeFns) {
        g_nativeFns = new std::map<std::string, NativeFn>();
    }

    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
}

Interpreter::~Interpreter() = default;

bool Interpreter::ensureJit() {
    if (jitImpl_ && jitImpl_->initialized) return true;

    auto jitOrErr = llvm::orc::LLJITBuilder().create();
    if (!jitOrErr) {
        std::cerr << "interpreter: failed to create ORC JIT\n";
        return false;
    }
    jitImpl_ = std::make_unique<JitImpl>();
    jitImpl_->jit = std::move(*jitOrErr);

    auto genOrErr = llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(
        jitImpl_->jit->getDataLayout().getGlobalPrefix());
    if (!genOrErr) {
        std::cerr << "interpreter: failed to create dynamic library generator\n";
        return false;
    }
    jitImpl_->jit->getMainJITDylib().addGenerator(std::move(*genOrErr));

    jitImpl_->initialized = true;
    return true;
}

void Interpreter::registerNativesToJit() {
    if (!jitImpl_ || !jitImpl_->initialized) return;

    // Build index -> name mapping for the dispatcher
    std::vector<std::string> fnNames;
    for (const auto& [name, fn] : *g_nativeFns) {
        fnNames.push_back(name);
    }

    // Create a module that declares at_native_dispatch and index globals
    auto dispatchModule = std::make_unique<llvm::Module>("native_dispatch", *jitContext_);
    llvm::FunctionType* dispatchType = llvm::FunctionType::get(
        llvm::Type::getInt64Ty(*jitContext_),
        {llvm::Type::getInt64Ty(*jitContext_),  // fnIndex
         llvm::Type::getInt64Ty(*jitContext_),  // a0
         llvm::Type::getInt64Ty(*jitContext_),  // a1
         llvm::Type::getInt64Ty(*jitContext_),  // a2
         llvm::Type::getInt64Ty(*jitContext_),  // a3
         llvm::Type::getInt64Ty(*jitContext_)}, // a4
        false);
    llvm::Function* dispatchFn = llvm::Function::Create(
        dispatchType, llvm::Function::ExternalLinkage, "at_native_dispatch", dispatchModule.get());

    // Register each native function as a global symbol holding its index
    for (size_t i = 0; i < fnNames.size(); ++i) {
        const auto& name = fnNames[i];
        auto* gv = new llvm::GlobalVariable(
            *dispatchModule, llvm::Type::getInt64Ty(*jitContext_), false,
            llvm::GlobalValue::ExternalLinkage,
            llvm::ConstantInt::get(*jitContext_, llvm::APInt(64, (int64_t)i)),
            "native_fn_idx_" + name);
        (void)gv;
    }

    // Wrap in ThreadSafeModule and add to JIT
    auto tsm = std::make_unique<llvm::orc::ThreadSafeModule>(
        std::move(dispatchModule),
        std::make_unique<llvm::LLVMContext>());
    if (auto err = jitImpl_->jit->addIRModule(std::move(*tsm))) {
        std::cerr << "interpreter: failed to add dispatch module: " << llvm::toString(std::move(err)) << "\n";
    }
}

void Interpreter::registerNative(const std::string& name, NativeFn fn) {
    (*g_nativeFns)[name] = fn;

    auto fval = std::make_shared<FunctionValue>();
    fval->isNative = true;
    fval->native = fn;
    Value v;
    v.kind = Value::Kind::Function;
    v.functionValue = fval;
    globals_->define(name, v);
}

bool Interpreter::jitModule(const AstPtr& root, SemanticAnalyzer& sem,
                             bool isMainModule,
                             const std::vector<std::string>& initFuncs,
                             const std::string& initFuncName) {
    if (!ensureJit()) return false;

    registerNativesToJit();

    auto result = lower_->lower(root, sem, isMainModule, initFuncs, initFuncName);
    if (!result) {
        std::cerr << "interpreter: JIT lowering failed\n";
        return false;
    }

    if (llvm::verifyModule(*result->module, &llvm::errs())) {
        std::cerr << "interpreter: module verification failed\n";
        return false;
    }

    // Wrap in ThreadSafeModule and add to JIT
    auto tsm = std::make_unique<llvm::orc::ThreadSafeModule>(
        std::move(result->module),
        std::make_unique<llvm::LLVMContext>());
    if (auto err = jitImpl_->jit->addIRModule(std::move(*tsm))) {
        std::cerr << "interpreter: failed to add module to JIT: " << llvm::toString(std::move(err)) << "\n";
        return false;
    }

    return true;
}

int64_t Interpreter::callJitSymbol(const std::string& name) {
    if (!jitImpl_ || !jitImpl_->initialized) return 0;

    auto symOrErr = jitImpl_->jit->lookup(name);
    if (!symOrErr) {
        std::cerr << "interpreter: symbol '" << name << "' not found in JIT\n";
        return 0;
    }

    using FnType = int64_t (*)();
    FnType fn = symOrErr->toPtr<FnType>();
    return fn();
}

// Check if AST contains any IMPORT statements
static bool hasImports(const AstPtr& node) {
    if (!node) return false;
    if (node->kind == "expr" && !node->children.empty() &&
        node->children[0] && node->children[0]->kind == "IMPORT") {
        return true;
    }
    for (const auto& c : node->children) {
        if (hasImports(c)) return true;
    }
    return false;
}

// Tree-walking execution (fallback for imports and complex cases)
Value Interpreter::evalTree(const AstPtr& node) {
    if (!node) return Value{};
    if (node->kind == "RETURN") {
    }
    if (node->kind == "expr" && node->children.size() == 1) {
        return evalTree(node->children[0]);
    }
    if (node->kind == "SELF_FUNC") {
        if (currentFunction_) {
            Value v;
            return v;
        }
        return Value{};
    }
    if (node->kind == "SELF_CLASS") {
        Value v;
        return v;
    }
    if (node->kind == "INTEGER") {
        Value v;
        v.kind = Value::Kind::Int;
        try { v.intValue = BigInt(std::stoll(node->text)); }
        catch (...) { v.intValue = BigInt(0); }
        return v;
    }
    if (node->kind == "DECIMAL") {
        Value v;
        v.kind = Value::Kind::Decimal;
        try { v.decimalValue = std::stod(node->text); }
        catch (...) { v.decimalValue = 0.0; }
        return v;
    }
    if (node->kind == "STRING") {
        Value v;
        v.kind = Value::Kind::String;
        std::string s = node->text;
        if (s.size() >= 2 && s.front() == '"' && s.back() == '"') {
            s = s.substr(1, s.size() - 2);
        }
        v.stringValue = s;
        return v;
    }
    if (node->kind == "IDENTIFIER") {
        return current_->get(node->text);
    }
    // Binary ops
    if (node->children.size() == 3 && node->children[1]) {
        std::string op = node->children[1]->text;
        if (op == "+" || op == "-" || op == "*" || op == "/" || op == "%" ||
            op == "^" || op == "|" || op == "&" ||
            op == "==" || op == "!=" || op == "<" || op == "<=" || op == ">" || op == ">=" ||
            op == "&&" || op == "||" || op == "^^") {
            Value l = evalTree(node->children[0]);
            Value r = evalTree(node->children[2]);
            return applyBinary(op, l, r);
        }
    }
    // Unary ops
    if (node->children.size() == 2 && node->children[0]) {
        std::string op = node->children[0]->text;
        if (op == "-" || op == "!" || op == "~") {
            Value r = evalTree(node->children[1]);
            return applyUnary(op, r);
        }
    }
    // Function call
    if (node->kind == "expr" && !node->children.empty()) {
        // Check for call: callee(args)
        for (const auto& c : node->children) {
            if (c && c->text == "(") {
                // Callee is first child
                Value callee = evalTree(node->children[0]);
                if (callee.kind == Value::Kind::Function && callee.functionValue) {
                    // Collect args
                    std::vector<Value> args;
                    for (size_t i = 1; i < node->children.size(); ++i) {
                        if (node->children[i] && node->children[i]->kind == "expr") {
                            args.push_back(evalTree(node->children[i]));
                        } else if (node->children[i] && node->children[i]->kind == "expr_list") {
                            for (const auto& a : node->children[i]->children) {
                                if (a && a->kind == "expr") args.push_back(evalTree(a));
                            }
                        }
                    }
                    if (callee.functionValue->isNative && callee.functionValue->native) {
                        return callee.functionValue->native(args);
                    }
                    // User function - execute body
                    if (callee.functionValue->body) {
                        auto saved = current_;
                        auto newScope = std::make_shared<Environment>();
                        newScope->parent = callee.functionValue->closure ? callee.functionValue->closure : globals_;
                        current_ = newScope;
                        // Bind params
                        for (size_t i = 0; i < callee.functionValue->params.size() && i < args.size(); ++i) {
                            current_->define(callee.functionValue->params[i], args[i]);
                        }
                        Value result;
                        try {
                            execTree(callee.functionValue->body);
                            result = lastValue_;
                        } catch (const ReturnSignal& rs) {
                            result = rs.value;
                        }
                        current_ = saved;
                        return result;
                    }
                }
                return Value{};
            }
        }
    }
    // Default: evaluate first child
    for (const auto& c : node->children) {
        if (c) {
            Value v = evalTree(c);
            if (v.kind != Value::Kind::Null) return v;
        }
    }
    return Value{};
}

// Tree-walking statement execution
void Interpreter::execTree(const AstPtr& node) {
    if (!node) return;
    if (node->kind == "scope" || node->kind == "start") {
        for (const auto& c : node->children) execTree(c);
        return;
    }
    if (node->kind != "stmt") return;
    if (node->children.empty()) return;
    const AstPtr& first = node->children[0];
    
    // Assignment: IDENTIFIER = expr
    if (node->children.size() == 3 && node->children[1] && node->children[1]->kind == "CHAR(61)") {
        std::string varName;
        if (node->children[0] && node->children[0]->kind == "expr" &&
            node->children[0]->children.size() >= 1 &&
            node->children[0]->children[0] && node->children[0]->children[0]->kind == "IDENTIFIER") {
            varName = node->children[0]->children[0]->text;
        } else if (node->children[0] && node->children[0]->kind == "IDENTIFIER") {
            varName = node->children[0]->text;
        }
        if (!varName.empty()) {
            Value v = evalTree(node->children[2]);
            current_->set(varName, v);
        }
        return;
    }
    
    // Return
    if (first && first->kind == "RETURN") {
        Value v;
        if (node->children.size() >= 2) v = evalTree(node->children[1]);
        throw ReturnSignal{v};
    }
    
    // If: expr ? { scope } : { scope }
    if (first && first->kind == "CHAR(63)") {
        Value cond = evalTree(node->children[0]);
        if (truthy(cond).boolValue) {
            for (const auto& c : node->children) {
                if (c && c->kind == "scope") { execTree(c); break; }
            }
        } else {
            bool seenColon = false;
            for (const auto& c : node->children) {
                if (c && c->text == ":") { seenColon = true; continue; }
                if (seenColon && c && c->kind == "scope") { execTree(c); break; }
            }
        }
        return;
    }
    
    // While: $ ( expr ) { scope }
    if (first && first->kind == "CHAR(36)") {
        while (true) {
            Value cond;
            bool hasCond = false;
            for (const auto& c : node->children) {
                if (c && c->kind == "expr") { cond = evalTree(c); hasCond = true; break; }
            }
            if (hasCond && !truthy(cond).boolValue) break;
            for (const auto& c : node->children) {
                if (c && c->kind == "scope") execTree(c);
            }
        }
        return;
    }
    
    // Block: { scope }
    if (first && first->kind == "CHAR(123)") {
        for (const auto& c : node->children) execTree(c);
        return;
    }
    
    // Import: 'path' (can be wrapped in stmt or expr)
    {
        const AstPtr& importExpr = nullptr;
        const AstPtr* found = nullptr;
        for (const auto& c : node->children) {
            if (c && c->kind == "expr" && !c->children.empty() &&
                c->children[0] && c->children[0]->kind == "IMPORT") {
                found = &c;
                break;
            }
        }
        if (found) {
            const AstPtr& ie = *found;
            std::string path = ie->children[0]->text;
            if (path.size() >= 2 && (path.front() == '"' || path.front() == '\'')) {
                path = path.substr(1, path.size() - 2);
            }
            Value mod = loadModule(path);
            lastValue_ = mod;
            return;
        }
    }
    
    // Default: exec children
    for (const auto& c : node->children) execTree(c);
}

Value Interpreter::run(const AstPtr& root) {
    if (!root) return Value{};

    SemanticAnalyzer sem;
    sem.analyze(root);
    if (!sem.errors().empty()) {
        for (const auto& e : sem.errors()) {
            std::cerr << "semantic error: " << e.message << "\n";
        }
        return Value{};
    }

    // If AST contains imports, fall back to tree-walking execution
    // because JIT cannot easily return module objects for import expressions
    if (hasImports(root)) {
        lastValue_ = Value{};
        try {
            execTree(root);
        } catch (const ReturnSignal& rs) {
            return rs.value;
        }
        return lastValue_;
    }

    if (!jitModule(root, sem, true)) {
        std::cerr << "interpreter: JIT compilation failed\n";
        return Value{};
    }

    int64_t result = callJitSymbol("main");
    Value v;
    v.kind = Value::Kind::Int;
    v.intValue = result;
    return v;
}

EnvironmentPtr Interpreter::makeScope(EnvironmentPtr parent) {
    auto e = std::make_shared<Environment>();
    e->parent = parent;
    return e;
}

Value Interpreter::truthy(const Value& v) const {
    switch (v.kind) {
        case Value::Kind::Null: return Value{.kind = Value::Kind::Bool, .boolValue = false};
        case Value::Kind::Bool: return v;
        case Value::Kind::Int: {
            Value r; r.kind = Value::Kind::Bool; r.boolValue = !v.intValue.isZero(); return r;
        }
        case Value::Kind::Decimal: {
            Value r; r.kind = Value::Kind::Bool; r.boolValue = (v.decimalValue != 0.0); return r;
        }
        case Value::Kind::String: {
            Value r; r.kind = Value::Kind::Bool; r.boolValue = !v.stringValue.empty(); return r;
        }
        default: {
            Value r; r.kind = Value::Kind::Bool; r.boolValue = true; return r;
        }
    }
}

Value Interpreter::applyUnary(const std::string& op, const Value& a) {
    if (op == "-") {
        if (a.kind == Value::Kind::Int) { Value r; r.kind = Value::Kind::Int; r.intValue = -a.intValue; return r; }
        if (a.kind == Value::Kind::Decimal) { Value r; r.kind = Value::Kind::Decimal; r.decimalValue = -a.decimalValue; return r; }
    }
    if (op == "!") {
        Value r; r.kind = Value::Kind::Bool; r.boolValue = !truthy(a).boolValue; return r;
    }
    return Value{};
}

Value Interpreter::applyBinary(const std::string& op, const Value& a, const Value& b) {
    if (op == "+") {
        if (a.kind == Value::Kind::Int && b.kind == Value::Kind::Int) {
            Value r; r.kind = Value::Kind::Int; r.intValue = a.intValue + b.intValue; return r;
        }
        if (a.kind == Value::Kind::Decimal || b.kind == Value::Kind::Decimal) {
            Value r; r.kind = Value::Kind::Decimal;
            double av = (a.kind == Value::Kind::Int) ? a.intValue.toDouble() : a.decimalValue;
            double bv = (b.kind == Value::Kind::Int) ? b.intValue.toDouble() : b.decimalValue;
            r.decimalValue = av + bv; return r;
        }
        if (a.kind == Value::Kind::String || b.kind == Value::Kind::String) {
            Value r; r.kind = Value::Kind::String;
            auto toStr = [](const Value& v) {
                if (v.kind == Value::Kind::Int) return v.intValue.toString();
                if (v.kind == Value::Kind::Decimal) return std::to_string(v.decimalValue);
                if (v.kind == Value::Kind::Bool) return v.boolValue ? std::string("true") : std::string("false");
                if (v.kind == Value::Kind::String) return v.stringValue;
                return std::string();
            };
            r.stringValue = toStr(a) + toStr(b);
            return r;
        }
    }
    if (op == "-") {
        if (a.kind == Value::Kind::Int && b.kind == Value::Kind::Int) {
            Value r; r.kind = Value::Kind::Int; r.intValue = a.intValue - b.intValue; return r;
        }
        if (a.kind == Value::Kind::Decimal || b.kind == Value::Kind::Decimal) {
            Value r; r.kind = Value::Kind::Decimal;
            double av = (a.kind == Value::Kind::Int) ? a.intValue.toDouble() : a.decimalValue;
            double bv = (b.kind == Value::Kind::Int) ? b.intValue.toDouble() : b.decimalValue;
            r.decimalValue = av - bv; return r;
        }
    }
    if (op == "*") {
        if (a.kind == Value::Kind::Int && b.kind == Value::Kind::Int) {
            Value r; r.kind = Value::Kind::Int; r.intValue = a.intValue * b.intValue; return r;
        }
        if (a.kind == Value::Kind::Decimal || b.kind == Value::Kind::Decimal) {
            Value r; r.kind = Value::Kind::Decimal;
            double av = (a.kind == Value::Kind::Int) ? a.intValue.toDouble() : a.decimalValue;
            double bv = (b.kind == Value::Kind::Int) ? b.intValue.toDouble() : b.decimalValue;
            r.decimalValue = av * bv; return r;
        }
    }
    if (op == "/") {
        if (a.kind == Value::Kind::Int && b.kind == Value::Kind::Int) {
            Value r; r.kind = Value::Kind::Int; r.intValue = a.intValue / b.intValue; return r;
        }
        if (a.kind == Value::Kind::Decimal || b.kind == Value::Kind::Decimal) {
            Value r; r.kind = Value::Kind::Decimal;
            double av = (a.kind == Value::Kind::Int) ? a.intValue.toDouble() : a.decimalValue;
            double bv = (b.kind == Value::Kind::Int) ? b.intValue.toDouble() : b.decimalValue;
            r.decimalValue = av / bv; return r;
        }
    }
    if (op == "%") {
        if (a.kind == Value::Kind::Int && b.kind == Value::Kind::Int) {
            Value r; r.kind = Value::Kind::Int; r.intValue = a.intValue % b.intValue; return r;
        }
    }
    if (op == "<" || op == ">" || op == "<=" || op == ">=" || op == "==" || op == "!=") {
        Value r; r.kind = Value::Kind::Bool;
        if (a.kind == Value::Kind::Int && b.kind == Value::Kind::Int) {
            if (op == "<") r.boolValue = a.intValue < b.intValue;
            else if (op == ">") r.boolValue = a.intValue > b.intValue;
            else if (op == "<=") r.boolValue = a.intValue <= b.intValue;
            else if (op == ">=") r.boolValue = a.intValue >= b.intValue;
            else if (op == "==") r.boolValue = a.intValue == b.intValue;
            else if (op == "!=") r.boolValue = a.intValue != b.intValue;
            return r;
        }
        if (a.kind == Value::Kind::String && b.kind == Value::Kind::String) {
            if (op == "==") r.boolValue = a.stringValue == b.stringValue;
            else if (op == "!=") r.boolValue = a.stringValue != b.stringValue;
            else if (op == "<") r.boolValue = a.stringValue < b.stringValue;
            else if (op == ">") r.boolValue = a.stringValue > b.stringValue;
            return r;
        }
        if (a.kind == Value::Kind::Bool && b.kind == Value::Kind::Bool) {
            if (op == "==") r.boolValue = a.boolValue == b.boolValue;
            else if (op == "!=") r.boolValue = a.boolValue != b.boolValue;
            return r;
        }
    }
    if (op == "&&") { Value r; r.kind = Value::Kind::Bool; r.boolValue = truthy(a).boolValue && truthy(b).boolValue; return r; }
    if (op == "||") { Value r; r.kind = Value::Kind::Bool; r.boolValue = truthy(a).boolValue || truthy(b).boolValue; return r; }
    if (op == "^^") { Value r; r.kind = Value::Kind::Bool; r.boolValue = truthy(a).boolValue ^ truthy(b).boolValue; return r; }
    return Value{};
}

Value Interpreter::loadModule(const std::string& path) {
    auto it = modules_.find(path);
    if (it != modules_.end()) return it->second;

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "import: cannot open '" << path << "'\n";
        return Value{};
    }
    std::stringstream ss;
    ss << file.rdbuf();
    std::string source = ss.str();

    AstPtr moduleAst = parseSource(source);
    if (!moduleAst) {
        std::cerr << "import: parse failed for '" << path << "'\n";
        return Value{};
    }

    SemanticAnalyzer sem;
    sem.analyze(moduleAst);

    size_t slash = path.find_last_of("/\\");
    std::string modName = (slash != std::string::npos) ? path.substr(slash + 1) : path;
    size_t dot = modName.find_last_of('.');
    if (dot != std::string::npos) modName = modName.substr(0, dot);
    std::string initFuncName = modName + "_init";

    if (!jitModule(moduleAst, sem, false, {}, initFuncName)) {
        std::cerr << "import: JIT failed for '" << path << "'\n";
        return Value{};
    }

    int64_t result = callJitSymbol(initFuncName);

    auto moduleObj = std::shared_ptr<std::map<std::string, Value>>(
        new std::map<std::string, Value>(), ObjectDeleter());
    
    // Store __result__ from the module's init function return value
    Value resultVal;
    resultVal.kind = Value::Kind::Int;
    resultVal.intValue = result;
    (*moduleObj)["__result__"] = resultVal;

    Value mod;
    mod.kind = Value::Kind::Object;
    mod.objectValue = moduleObj;
    modules_[path] = mod;
    return mod;
}

Value Interpreter::loadModuleFFI(const std::string& path) {
    auto it = modules_.find(path);
    if (it != modules_.end()) return it->second;

    if (path.rfind(".at") == path.size() - 3) {
        return loadModule(path);
    }

    std::string soPath = path;
    if (soPath.rfind(".o") == soPath.size() - 2) {
        soPath = soPath.substr(0, soPath.size() - 2) + ".so";
    }

    void* handle = nullptr;
    if (!soPath.empty()) {
        handle = dlopen(soPath.c_str(), RTLD_NOW | RTLD_LOCAL);
    }
    if (!handle && !path.empty()) {
        handle = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
    }
    if (!handle) {
        std::cerr << "import: cannot open shared library '" << path << "' (" << dlerror() << ")\n";
        return Value{};
    }

    ObjectImporter importer;
    std::string objPath = path;
    if (objPath.rfind(".so") == objPath.size() - 3) {
        objPath = objPath.substr(0, objPath.size() - 3) + ".o";
    }
    bool haveSigs = objPath.empty() || importer.importObjectFile(objPath) > 0;

    auto moduleObj = std::shared_ptr<std::map<std::string, Value>>(
        new std::map<std::string, Value>(), ObjectDeleter());

    if (haveSigs) {
        for (const auto& sym : importer.allSymbols()) {
            if (!sym) continue;
            if (sym->kind != SymbolKind::Function) continue;
            void* addr = dlsym(handle, sym->name.c_str());
            if (!addr) {
                addr = dlsym(handle, ("_" + sym->name).c_str());
            }
            if (!addr) continue;

            Value fn = makeNativeFFIFn(addr);
            (*moduleObj)[sym->name] = fn;
        }
    }

    Value mod;
    mod.kind = Value::Kind::Object;
    mod.objectValue = moduleObj;
    modules_[path] = mod;
    return mod;
}

static Value makeNativeFFIFn(void* addr) {
    auto fval = std::make_shared<FunctionValue>();
    fval->isNative = true;
    fval->native = [addr](const std::vector<Value>& args) -> Value {
        using FnType = int64_t (*)(int64_t, int64_t, int64_t, int64_t, int64_t);
        FnType fn = reinterpret_cast<FnType>(addr);

        int64_t a0 = 0, a1 = 0, a2 = 0, a3 = 0, a4 = 0;
        if (args.size() > 0 && args[0].kind == Value::Kind::Int) a0 = args[0].intValue.toInt64();
        if (args.size() > 1 && args[1].kind == Value::Kind::Int) a1 = args[1].intValue.toInt64();
        if (args.size() > 2 && args[2].kind == Value::Kind::Int) a2 = args[2].intValue.toInt64();
        if (args.size() > 3 && args[3].kind == Value::Kind::Int) a3 = args[3].intValue.toInt64();
        if (args.size() > 4 && args[4].kind == Value::Kind::Int) a4 = args[4].intValue.toInt64();

        int64_t result = fn(a0, a1, a2, a3, a4);
        Value v; v.kind = Value::Kind::Int; v.intValue = result;
        return v;
    };
    Value v; v.kind = Value::Kind::Function; v.functionValue = fval;
    return v;
}

} // namespace at
