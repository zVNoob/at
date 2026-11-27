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

    // Register I/O native functions
    registerNative("at_fopen", [](const std::vector<Value>& args) -> Value {
        if (args.size() < 2) return Value{};
        std::string path = (args[0].kind == Value::Kind::String) ? args[0].stringValue : "";
        int mode = (args[1].kind == Value::Kind::Int) ? (int)args[1].intValue.toInt64() : 0;
        FILE* f = at_fopen(path.c_str(), mode);
        Value v; v.kind = Value::Kind::Int; v.intValue = (long long)(size_t)f; return v;
    });
    registerNative("at_fclose", [](const std::vector<Value>& args) -> Value {
        if (args.empty()) return Value{};
        FILE* f = (FILE*)(size_t)args[0].intValue.toInt64();
        int r = at_fclose(f);
        Value v; v.kind = Value::Kind::Int; v.intValue = r; return v;
    });
    registerNative("at_fread", [](const std::vector<Value>& args) -> Value {
        if (args.size() < 2) return Value{};
        FILE* f = (FILE*)(size_t)args[0].intValue.toInt64();
        int size = (args[1].kind == Value::Kind::Int) ? (int)args[1].intValue.toInt64() : 1024;
        if (size <= 0) size = 1024;
        std::string buf(size, '\0');
        size_t n = at_fread4(&buf[0], 1, size, f);
        buf.resize(n);
        Value v; v.kind = Value::Kind::String; v.stringValue = buf; return v;
    });
    registerNative("at_fwrite", [](const std::vector<Value>& args) -> Value {
        if (args.size() < 2) return Value{};
        FILE* f = (FILE*)(size_t)args[0].intValue.toInt64();
        std::string s = (args[1].kind == Value::Kind::String) ? args[1].stringValue : "";
        size_t n = at_fwrite4(s.c_str(), 1, s.size(), f);
        fflush(f);
        Value v; v.kind = Value::Kind::Int; v.intValue = (long long)n; return v;
    });
    registerNative("at_feof", [](const std::vector<Value>& args) -> Value {
        if (args.empty()) return Value{};
        FILE* f = (FILE*)(size_t)args[0].intValue.toInt64();
        int r = at_feof(f);
        Value v; v.kind = Value::Kind::Bool; v.boolValue = (r != 0); return v;
    });
    registerNative("at_fflush", [](const std::vector<Value>& args) -> Value {
        if (args.empty()) return Value{};
        FILE* f = (FILE*)(size_t)args[0].intValue.toInt64();
        int r = at_fflush(f);
        Value v; v.kind = Value::Kind::Int; v.intValue = r; return v;
    });
    registerNative("at_ftell", [](const std::vector<Value>& args) -> Value {
        if (args.empty()) return Value{};
        FILE* f = (FILE*)(size_t)args[0].intValue.toInt64();
        long r = at_ftell(f);
        Value v; v.kind = Value::Kind::Int; v.intValue = r; return v;
    });
    registerNative("at_fseek", [](const std::vector<Value>& args) -> Value {
        if (args.size() < 3) return Value{};
        FILE* f = (FILE*)(size_t)args[0].intValue.toInt64();
        long offset = (args[1].kind == Value::Kind::Int) ? args[1].intValue.toInt64() : 0;
        int whence = (args[2].kind == Value::Kind::Int) ? (int)args[2].intValue.toInt64() : 0;
        int r = at_fseek(f, offset, whence);
        Value v; v.kind = Value::Kind::Int; v.intValue = r; return v;
    });
    registerNative("at_fgets", [](const std::vector<Value>& args) -> Value {
        if (args.size() < 2) return Value{};
        FILE* f = (FILE*)(size_t)args[0].intValue.toInt64();
        int size = (args[1].kind == Value::Kind::Int) ? (int)args[1].intValue.toInt64() : 256;
        if (size <= 0) size = 256;
        std::string buf(size, '\0');
        char* r = at_fgets(&buf[0], size, f);
        if (!r) { Value v; v.kind = Value::Kind::Null; return v; }
        buf.resize(strlen(buf.c_str()));
        Value v; v.kind = Value::Kind::String; v.stringValue = buf; return v;
    });
    registerNative("at_fputs", [](const std::vector<Value>& args) -> Value {
        if (args.size() < 2) return Value{};
        FILE* f = (FILE*)(size_t)args[1].intValue.toInt64();
        std::string s = (args[0].kind == Value::Kind::String) ? args[0].stringValue : "";
        int r = at_fputs(s.c_str(), f);
        Value v; v.kind = Value::Kind::Int; v.intValue = r; return v;
    });
    registerNative("at_fputc", [](const std::vector<Value>& args) -> Value {
        if (args.size() < 2) return Value{};
        FILE* f = (FILE*)(size_t)args[1].intValue.toInt64();
        int c = (args[0].kind == Value::Kind::Int) ? (int)args[0].intValue.toInt64() : 0;
        int r = at_fputc(c, f);
        Value v; v.kind = Value::Kind::Int; v.intValue = r; return v;
    });
    registerNative("at_fgetc", [](const std::vector<Value>& args) -> Value {
        if (args.empty()) return Value{};
        FILE* f = (FILE*)(size_t)args[0].intValue.toInt64();
        int r = at_fgetc(f);
        Value v; v.kind = Value::Kind::Int; v.intValue = r; return v;
    });
    registerNative("at_stdin", [](const std::vector<Value>&) -> Value {
        Value v; v.kind = Value::Kind::Int; v.intValue = (long long)(size_t)at_stdin(); return v;
    });
    registerNative("at_stdout", [](const std::vector<Value>&) -> Value {
        Value v; v.kind = Value::Kind::Int; v.intValue = (long long)(size_t)at_stdout(); return v;
    });
    registerNative("at_stderr", [](const std::vector<Value>&) -> Value {
        Value v; v.kind = Value::Kind::Int; v.intValue = (long long)(size_t)at_stderr(); return v;
    });
    registerNative("at_strlen", [](const std::vector<Value>& args) -> Value {
        if (args.empty() || args[0].kind != Value::Kind::String) return Value{};
        Value v; v.kind = Value::Kind::Int; v.intValue = (long long)at_strlen(args[0].stringValue.c_str()); return v;
    });

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
        Value v = current_->get(node->text);
        if (node->text == "stdout" || node->text == "io") {
        }
        return v;
    }
    if (node->kind == "IMPORT") {
        std::string path = node->text;
        if (path.size() >= 2 && (path.front() == '\'' || path.front() == '"')) {
            path = path.substr(1, path.size() - 2);
        }
        return loadModuleFFI(path);
    }
    // Member access: obj.method
    if (node->kind == "expr" && !node->children.empty()) {
        bool hasDot = false;
        for (const auto& c : node->children) {
            if (c && c->text == ".") { hasDot = true; break; }
        }
        if (hasDot && node->children.size() >= 3) {
            Value obj = evalTree(node->children[0]);
            if (obj.kind == Value::Kind::Object && obj.objectValue) {
                for (const auto& [k, v] : *obj.objectValue) {
                }
                for (const auto& [k, v] : *obj.objectValue) {
                }
                for (size_t i = 1; i < node->children.size(); ++i) {
                    if (node->children[i] && node->children[i]->kind == "IDENTIFIER") {
                        auto it = obj.objectValue->find(node->children[i]->text);
                        if (it != obj.objectValue->end()) {
                            return it->second;
                        }
                    }
                }
            }
        }
    }
    // Anonymous template / function expression: @ (params) { body }
    if (node->kind == "expr" && !node->children.empty() &&
        node->children[0] && node->children[0]->text == "@") {
        auto fn = std::make_shared<FunctionValue>();
        // Find params: func_arg_declare_list child
        for (const auto& c : node->children) {
            if (c && c->kind == "func_arg_declare_list") {
                for (const auto& p : c->children) {
                    if (p && p->kind == "func_arg_declare" && !p->children.empty() &&
                        p->children[0]->kind == "IDENTIFIER") {
                        fn->params.push_back(p->children[0]->text);
                    }
                }
            }
            if (c && c->kind == "scope") {
                fn->body = c;
            }
        }
        fn->closure = current_;
        Value v; v.kind = Value::Kind::Function; v.functionValue = fn;
        return v;
    }
    // Object construction: ClassName <{ args }>
    if (node->kind == "expr" && !node->children.empty() &&
        node->children[0] && node->children[0]->kind == "IDENTIFIER" &&
        node->children.size() >= 4 &&
        node->children[1] && node->children[1]->text == "<{" &&
        node->children[2] && node->children[2]->kind == "expr_list" &&
        node->children[3] && node->children[3]->text == "}>") {
        std::string className = node->children[0]->text;
        Value cls = current_->get(className);
        if (cls.kind == Value::Kind::Function && cls.functionValue) {
            // Create new object
            auto obj = std::shared_ptr<std::map<std::string, Value>>(
                new std::map<std::string, Value>(), ObjectDeleter());
            Value objVal; objVal.kind = Value::Kind::Object; objVal.objectValue = obj;

            // Collect constructor args from expr_list
            std::vector<Value> args;
            for (const auto& a : node->children[2]->children) {
                if (a && a->kind == "expr") args.push_back(evalTree(a));
            }

            // Execute constructor body in a scope with self = object
            auto fn = cls.functionValue;
            auto newScope = std::make_shared<Environment>();
            newScope->parent = fn->closure ? fn->closure : globals_;
            newScope->define("self", objVal);
            for (size_t i = 0; i < fn->params.size() && i < args.size(); ++i) {
                newScope->define(fn->params[i], args[i]);
            }
            EnvironmentPtr prev = current_;
            current_ = newScope;
            try {
                if (fn->body) execTree(fn->body);
            } catch (const ReturnSignal& rs) {
                objVal = rs.value;
            }
            current_ = prev;

            // Copy constructor scope vars (except "self") into the object map
            // so member access (obj.field) can find them
            if (objVal.kind == Value::Kind::Object && objVal.objectValue) {
                for (const auto& [name, val] : newScope->vars) {
                    if (name != "self") {
                        (*objVal.objectValue)[name] = val;
                    }
                }
            }

            return objVal;
        }
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
    
    // Class definition: IDENTIFIER := # (params) { body }
    if (node->children.size() >= 3 &&
        node->children[0] && node->children[0]->kind == "IDENTIFIER" &&
        node->children[1] && node->children[1]->kind == "ALIAS_PUBLIC") {
        std::string className = node->children[0]->text;
        // Find the class node inside child[2] (type > base_type > complex_type > class)
        AstPtr classNode = nullptr;
        std::function<void(const AstPtr&)> findClass = [&](const AstPtr& n) {
            if (!n || classNode) return;
            if (n->kind == "class") { classNode = n; return; }
            for (const auto& c : n->children) findClass(c);
        };
        if (node->children[2]) findClass(node->children[2]);
        if (classNode) {
            auto fn = std::make_shared<FunctionValue>();
            // Extract constructor params from func_arg_declare_list
            for (const auto& c : classNode->children) {
                if (c && c->kind == "func_arg_declare_list") {
                    for (const auto& p : c->children) {
                        if (p && p->kind == "func_arg_declare" && !p->children.empty() &&
                            p->children[0]->kind == "IDENTIFIER") {
                            fn->params.push_back(p->children[0]->text);
                        }
                    }
                }
                if (c && c->kind == "scope") fn->body = c;
            }
            fn->closure = current_;
            fn->hasSelf = true;
            Value v; v.kind = Value::Kind::Function; v.functionValue = fn;
            current_->define(className, v);
        }
        return;
    }
    
    // Assignment: IDENTIFIER = expr or obj.field = expr
    // The '=' may be a direct child of stmt (e.g. children[1]) or nested inside an expr child
    {
        // Case 1: '=' is a direct child of stmt (found at any index)
        {
            size_t eqIdx = 0;
            bool foundEq = false;
            for (size_t i = 0; i < node->children.size(); ++i) {
                if (node->children[i] && node->children[i]->kind == "CHAR(61)") {
                    eqIdx = i;
                    foundEq = true;
                    break;
                }
            }
            if (foundEq) {
                const AstPtr& left = node->children[0];
                // Check if LHS is member access: obj.field
                if (left->kind == "expr" && !left->children.empty()) {
                    bool hasDot = false;
                    for (const auto& c : left->children) {
                        if (c && c->text == ".") { hasDot = true; break; }
                    }
                    if (hasDot && left->children.size() >= 3) {
                        Value obj = evalTree(left->children[0]);
                        if (obj.kind == Value::Kind::Object && obj.objectValue) {
                            for (size_t i = 1; i < left->children.size(); ++i) {
                                if (left->children[i] && left->children[i]->kind == "IDENTIFIER") {
                                    const AstPtr* rhs = nullptr;
                                    for (size_t j = eqIdx + 1; j < node->children.size(); ++j) {
                                        if (node->children[j] && node->children[j]->kind == "expr") {
                                            rhs = &node->children[j];
                                            break;
                                        }
                                    }
                                    if (rhs) {
                                        Value v = evalTree(*rhs);
                                        (*obj.objectValue)[left->children[i]->text] = v;
                                    }
                                    return;
                                }
                            }
                        }
                    }
                }
                // Simple variable assignment
                std::string varName;
                if (left->kind == "IDENTIFIER") {
                    varName = left->text;
                } else if (left->kind == "expr" && !left->children.empty() &&
                           left->children[0]->kind == "IDENTIFIER") {
                    varName = left->children[0]->text;
                }
                // Right side is the expr child after '='
                const AstPtr* rhs = nullptr;
                for (size_t i = eqIdx + 1; i < node->children.size(); ++i) {
                    if (node->children[i] && node->children[i]->kind == "expr") {
                        rhs = &node->children[i];
                        break;
                    }
                }
                if (!varName.empty() && rhs) {
                    Value v = evalTree(*rhs);
                    current_->set(varName, v);
                }
                return;
            }
        }

        // Case 2: '=' is nested inside an expr child
        const AstPtr* assignExpr = nullptr;
        for (const auto& c : node->children) {
            if (c && c->kind == "expr") {
                for (size_t i = 0; i + 1 < c->children.size(); ++i) {
                    if (c->children[i] && c->children[i]->kind == "CHAR(61)") {
                        assignExpr = &c;
                        break;
                    }
                }
                if (assignExpr) break;
            }
        }
        if (assignExpr) {
            const AstPtr& ae = *assignExpr;
            size_t eqIdx = 0;
            for (size_t i = 0; i < ae->children.size(); ++i) {
                if (ae->children[i] && ae->children[i]->kind == "CHAR(61)") {
                    eqIdx = i;
                    break;
                }
            }
            // Left side: child before '='
            const AstPtr& left = ae->children[eqIdx - 1];
            // Check if LHS is member access: obj.field
            if (left->kind == "expr" && !left->children.empty()) {
                bool hasDot = false;
                for (const auto& c : left->children) {
                    if (c && c->text == ".") { hasDot = true; break; }
                }
                if (hasDot && left->children.size() >= 3) {
                    Value obj = evalTree(left->children[0]);
                    if (obj.kind == Value::Kind::Object && obj.objectValue) {
                        // Find the field name (last IDENTIFIER child)
                        for (size_t i = 1; i < left->children.size(); ++i) {
                            if (left->children[i] && left->children[i]->kind == "IDENTIFIER") {
                                if (eqIdx + 1 < ae->children.size()) {
                                    Value v = evalTree(ae->children[eqIdx + 1]);
                                    (*obj.objectValue)[left->children[i]->text] = v;
                                }
                                return;
                            }
                        }
                    }
                }
            }
            // Simple variable assignment
            std::string varName;
            if (eqIdx > 0) {
                if (left->kind == "IDENTIFIER") {
                    varName = left->text;
                } else if (left->kind == "expr" && !left->children.empty() &&
                           left->children[0]->kind == "IDENTIFIER") {
                    varName = left->children[0]->text;
                }
            }
            // Right side: child after '='
            if (!varName.empty() && eqIdx + 1 < ae->children.size()) {
                Value v = evalTree(ae->children[eqIdx + 1]);
                current_->set(varName, v);
            }
            return;
        }
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
    
    // Default: evaluate expression statements, recurse into others
    for (const auto& c : node->children) {
        if (c && c->kind == "expr") {
            lastValue_ = evalTree(c);
        } else {
            execTree(c);
        }
    }
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

    // Always use tree-walking execution for the interpreter.
    // JIT path (jitModule/callJitSymbol) is still available via direct calls
    // but does not resolve native functions at runtime, so tree-walking is used
    // here to guarantee compiled and interpreted code behave identically.
    lastValue_ = Value{};
    try {
        execTree(root);
    } catch (const ReturnSignal& rs) {
        return rs.value;
    }
    return lastValue_;
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

    // Run module in its own scope using tree-walking execution
    auto moduleEnv = makeScope(globals_);
    EnvironmentPtr prev = current_;
    current_ = moduleEnv;
    lastValue_ = Value{};
    Value result;
    try {
        execTree(moduleAst);
        result = lastValue_;
    } catch (const ReturnSignal& r) {
        result = r.value;
    }
    current_ = prev;

    // Create module object with all exported variables
    auto moduleObj = std::shared_ptr<std::map<std::string, Value>>(
        new std::map<std::string, Value>(), ObjectDeleter());
    for (const auto& [name, val] : moduleEnv->vars) {
        (*moduleObj)[name] = val;
    }

    // Store module init result if available
    if (result.kind != Value::Kind::Null) {
        (*moduleObj)["__result__"] = result;
    }

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
