#include "at_runtime.h"
#include "interpreter.h"
#include "parser.h"
#include "semantic.h"

#include <cstdlib>
#include <cstring>
#include <dlfcn.h>
#include <functional>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace at {

static BigInt parseIntLiteral(const std::string& s) {
    if (s.rfind("0x", 0) == 0 || s.rfind("0X", 0) == 0) {
        return BigInt(std::stoll(s, nullptr, 16)); // TODO: proper hex BigInt parsing
    }
    if (s.rfind("0b", 0) == 0 || s.rfind("0B", 0) == 0) {
        return BigInt(std::stoll(s.substr(2), nullptr, 2));
    }
    if (s.rfind("0o", 0) == 0 || s.rfind("0O", 0) == 0) {
        return BigInt(std::stoll(s.substr(2), nullptr, 8));
    }
    return BigInt(s);
}

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
        // Check if parent has it
        if (parent->vars.count(name)) {
            parent->set(name, value);
            return;
        }
    }
    // Create in current scope
    vars[name] = value;
}

void Environment::define(const std::string& name, const Value& value) {
    vars[name] = value;
}

Interpreter::Interpreter() {
    globals_ = std::make_shared<Environment>();
    current_ = globals_;

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
}

void Interpreter::registerNative(const std::string& name, NativeFn fn) {
    auto fval = std::make_shared<FunctionValue>();
    fval->isNative = true;
    fval->native = fn;
    Value v;
    v.kind = Value::Kind::Function;
    v.functionValue = fval;
    globals_->define(name, v);
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

    // Semantic check (non-fatal).
    SemanticAnalyzer sem;
    sem.analyze(moduleAst);

    // Run module in its own scope.
    auto moduleEnv = makeScope(globals_);
    EnvironmentPtr prev = current_;
    current_ = moduleEnv;
    lastValue_ = Value{};
    Value result;
    try {
        exec(moduleAst);
        result = lastValue_;
    } catch (const ReturnSignal& r) {
        result = r.value;
    }
    current_ = prev;

    // Always create a module object with all exports
    auto moduleObj = std::shared_ptr<std::map<std::string, Value>>(new std::map<std::string, Value>(), ObjectDeleter());
    for (const auto& [name, val] : moduleEnv->vars) {
        (*moduleObj)[name] = val;
    }
    auto it2 = moduleEnv->vars.find("stdout");
    if (it2 != moduleEnv->vars.end()) {
    } else {
    }
    // If there's an explicit return value, use it; otherwise use the module object
    if (result.kind == Value::Kind::Null) {
        result.kind = Value::Kind::Object;
        result.objectValue = moduleObj;
    } else {
        // Attach exports to the result if it's an object, or wrap in module object
        if (result.kind != Value::Kind::Object) {
            // Store the return value as a special "__result__" key
            (*moduleObj)["__result__"] = result;
            result.kind = Value::Kind::Object;
            result.objectValue = moduleObj;
        } else if (result.objectValue) {
            for (const auto& [name, val] : *moduleObj) {
                (*result.objectValue)[name] = val;
            }
        }
    }

    modules_[path] = result;
    return result;
}

// Forward declaration: create a native function value that calls an FFI
// function address using the i64 calling convention.
static Value makeNativeFFIFn(void* addr);

Value Interpreter::loadModuleFFI(const std::string& path) {
    auto it = modules_.find(path);
    if (it != modules_.end()) return it->second;

    // .at source files: use the original source-level module loader.
    if (path.rfind(".at") == path.size() - 3) {
        return loadModule(path);
    }

    // Determine the shared library path: try .so first, then the path as-is.
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

    // Read .at.sigs from the object file for type info (even if we dlopen'd
    // the .so, the .o has the signatures).
    ObjectImporter importer;
    std::string objPath = path;
    if (objPath.rfind(".so") == objPath.size() - 3) {
        objPath = objPath.substr(0, objPath.size() - 3) + ".o";
    }
    bool haveSigs = objPath.empty() || importer.importObjectFile(objPath) > 0;

    // Build a module object with each imported function as a native wrapper.
    auto moduleObj = std::shared_ptr<std::map<std::string, Value>>(
        new std::map<std::string, Value>(), ObjectDeleter());

    if (haveSigs) {
        for (const auto& sym : importer.allSymbols()) {
            if (!sym) continue;
            if (sym->kind != SymbolKind::Function) continue;
            void* addr = dlsym(handle, sym->name.c_str());
            if (!addr) {
                // Try mangled name
                addr = dlsym(handle, ("_" + sym->name).c_str());
            }
            if (!addr) continue;

            Value fn = makeNativeFFIFn(addr);
            (*moduleObj)[sym->name] = fn;
        }
    }

    // Create module value.
    Value mod;
    mod.kind = Value::Kind::Object;
    mod.objectValue = moduleObj;
    modules_[path] = mod;
    return mod;
}

// Create a native function value that calls an FFI function address using the
// i64 calling convention (all args and return are int64).
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

Value Interpreter::run(const AstPtr& root) {
    if (!root) return Value{};
    lastValue_ = Value{};
    try {
        exec(root);
    } catch (const ReturnSignal& r) {
        return r.value;
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
            // convert
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
    if (op == "*" ) {
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
    if (op == "/" ) {
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
    if (op == "&&") { Value r; r.kind=Value::Kind::Bool; r.boolValue = truthy(a).boolValue && truthy(b).boolValue; return r; }
    if (op == "||") { Value r; r.kind=Value::Kind::Bool; r.boolValue = truthy(a).boolValue || truthy(b).boolValue; return r; }
    if (op == "^^") { Value r; r.kind=Value::Kind::Bool; r.boolValue = truthy(a).boolValue ^ truthy(b).boolValue; return r; }
    return Value{};
}

Value Interpreter::eval(const AstPtr& node) {
    if (!node) return Value{};
    if (node->kind == "RETURN") {
    }
    if (node->kind == "expr" && node->children.size() == 1) {
        return eval(node->children[0]);
    }
    if (node->kind == "SELF_FUNC") {
        if (currentFunction_) {
            Value v; v.kind = Value::Kind::Function; v.functionValue = currentFunction_;
            return v;
        }
        return Value{};
    }
    if (node->kind == "SELF_CLASS") {
        EnvironmentPtr p = current_;
        while (p) {
            auto it = p->vars.find("self");
            if (it != p->vars.end()) return it->second;
            p = p->parent;
        }
        return Value{};
    }
    if (node->kind == "INTEGER") {
        Value v; v.kind = Value::Kind::Int; v.intValue = parseIntLiteral(node->text); return v;
    }
    if (node->kind == "DECIMAL") {
        Value v; v.kind = Value::Kind::Decimal; v.decimalValue = std::stod(node->text); return v;
    }
    if (node->kind == "STRING") {
        Value v; v.kind = Value::Kind::String;
        std::string s = node->text;
        if (s.size() >= 2 && s.front() == '"') {
            s = s.substr(1, s.size() - 2);
        }
        // Process escape sequences: \n \t \\ \" \' \r
        std::string result;
        result.reserve(s.size());
        for (size_t i = 0; i < s.size(); ++i) {
            if (s[i] == '\\' && i + 1 < s.size()) {
                char next = s[++i];
                if (next == 'n') result.push_back('\n');
                else if (next == 't') result.push_back('\t');
                else if (next == 'r') result.push_back('\r');
                else if (next == '\\') result.push_back('\\');
                else if (next == '"') result.push_back('"');
                else if (next == '\'') result.push_back('\'');
                else { result.push_back('\\'); result.push_back(next); }
            } else {
                result.push_back(s[i]);
            }
        }
        s = result;
        v.stringValue = s;
        return v;
    }
    if (node->kind == "IMPORT") {
        std::string path = node->text;
        if (path.size() >= 2 && (path.front() == '\'' || path.front() == '"')) {
            path = path.substr(1, path.size() - 2);
        }
        return loadModuleFFI(path);
    }
    if (node->kind == "IDENTIFIER") {
        Value v = current_->get(node->text);
        if (node->text == "at_fwrite") {
        }
        return v;
    }
    // binary
    if (node->children.size() == 3 && node->children[1]) {
        std::string op = node->children[1]->text;
        bool isOp = (op == "+" || op == "-" || op == "*" || op == "/" || op == "%" || op == "^" || op == "|" ||
                     op == "&" || op == "<" || op == ">" || op == "<=" || op == ">=" || op == "==" ||
                     op == "!=" || op == "||" || op == "&&" || op == "^^");
        if (isOp) {
            Value a = eval(node->children[0]);
            Value b = eval(node->children[2]);
            // Check for registered overload
            auto it = overloads_.find(op);
            // Disable overload during its own execution to prevent infinite recursion
            bool overloadDisabled = (activeOverload_ == op);
            if (it != overloads_.end() && it->second.kind == Value::Kind::Function && !overloadDisabled) {
                auto fn = it->second.functionValue;
                auto scope = makeScope(fn->closure);
                for (size_t i = 0; i < fn->params.size() && i < 2; ++i) {
                    // params might be unnamed; define a, b
                }
                // Define args by position
                if (fn->params.size() >= 1) scope->define(fn->params[0], a);
                if (fn->params.size() >= 2) scope->define(fn->params[1], b);
                EnvironmentPtr prev = current_;
                std::string prevOverload = activeOverload_;
                activeOverload_ = op;
                current_ = scope;
                Value result;
                try {
                    if (fn->body) exec(fn->body);
                } catch (const ReturnSignal& r) {
                    result = r.value;
                } catch (...) {
                }
                current_ = prev;
                activeOverload_ = prevOverload;
                return result;
            }
            return applyBinary(op, a, b);
        }
    }
    // ternary
    {
        int q = -1, c = -1;
        for (size_t i = 0; i < node->children.size(); ++i) {
            if (node->children[i] && node->children[i]->text == "?") q = (int)i;
            if (node->children[i] && node->children[i]->text == ":") c = (int)i;
        }
        if (q >= 0 && c > q) {
            Value cond = (q > 0) ? eval(node->children[q-1]) : Value{};
            if (truthy(cond).boolValue) {
                if (q + 1 < (int)node->children.size()) return eval(node->children[q+1]);
            } else {
                if (c + 1 < (int)node->children.size()) return eval(node->children[c+1]);
            }
            return Value{};
        }
    }
    // unary
    if (node->children.size() == 2 && node->children[0] &&
        (node->children[0]->text == "-" || node->children[0]->text == "!")) {
        Value a = eval(node->children[1]);
        return applyUnary(node->children[0]->text, a);
    }
    // borrow &
    if (!node->children.empty() && node->children[0] && node->children[0]->text == "&") {
        if (node->children.size() >= 2) return eval(node->children[1]);
    }
    // cast expr -> type (but not a function literal `@ (...) -> (T) { }`)
    if (!(node->children.size() > 0 && node->children[0] && node->children[0]->text == "@")) {
        int arrow = -1;
        for (size_t i = 0; i < node->children.size(); ++i) {
            if (node->children[i] && node->children[i]->kind == "ARROW") { arrow = (int)i; break; }
        }
        if (arrow >= 1 && arrow + 1 < (int)node->children.size()) {
            return eval(node->children[arrow - 1]);
        }
    }
    // call
    {
        bool hasCall = false;
        for (const auto& c : node->children) if (c && c->text == "(") { hasCall = true; break; }
        if (hasCall && !(node->children[0] && node->children[0]->text == "@")) {
            AstPtr calleeNode = nullptr;
            for (const auto& c : node->children) if (c && c->kind == "expr") { if (!calleeNode) calleeNode = c; }
            // Detect method call (dot expression) and capture base object
            bool isDotCall = false;
            AstPtr dotBaseNode = nullptr;
            if (calleeNode) {
                for (const auto& c : calleeNode->children) {
                    if (c && c->text == ".") { isDotCall = true; }
                }
                if (isDotCall) {
                    for (const auto& c : calleeNode->children) {
                        if (c && c->kind == "expr") { if (!dotBaseNode) dotBaseNode = c; }
                    }
                }
            }
            Value callee = calleeNode ? eval(calleeNode) : Value{};
            Value dotBase;
            if (isDotCall && dotBaseNode) dotBase = eval(dotBaseNode);
            AstPtr argsNode = nullptr;
            for (const auto& c : node->children) if (c && c->kind == "expr_list") { argsNode = c; break; }
            std::vector<Value> args;
            // Recursively collect expr args from expr_list
            std::function<void(const AstPtr&)> collectArgs = [&](const AstPtr& n) {
                if (!n) return;
                if (n->kind == "expr_list") {
                    for (const auto& c : n->children) {
                        if (c && c->kind == "expr") args.push_back(eval(c));
                        else collectArgs(c);
                    }
                } else if (n->kind == "expr") {
                    args.push_back(eval(n));
                }
            };
            if (argsNode) collectArgs(argsNode);
            if (callee.kind == Value::Kind::Function && callee.functionValue) {
                auto fn = callee.functionValue;
                if (fn->isNative && fn->native) {
                    return fn->native(args);
                }
                auto scope = makeScope(fn->closure);
                if (isDotCall && dotBase.kind == Value::Kind::Object && dotBase.objectValue) {
                    scope->define("self", dotBase);
                } else if (fn->hasSelf && fn->selfObject) {
                    Value selfVal; selfVal.kind = Value::Kind::Object; selfVal.objectValue = fn->selfObject;
                    scope->define("self", selfVal);
                }
                for (size_t i = 0; i < fn->params.size(); ++i) {
                    if (i < args.size()) scope->define(fn->params[i], args[i]);
                }
                EnvironmentPtr prev = current_;
                auto prevFn = currentFunction_;
                currentFunction_ = fn;
                current_ = scope;
                Value result;
                try {
                    if (fn->body) exec(fn->body);
                } catch (const ReturnSignal& r) {
                    result = r.value;
                } catch (...) {
                }
                current_ = prev;
                currentFunction_ = prevFn;
                return result;
            }
            return Value{};
        }
    }
    // index
    {
        bool hasIndex = false;
        for (const auto& c : node->children) if (c && c->text == "[") { hasIndex = true; break; }
        if (hasIndex) {
            AstPtr baseNode = nullptr;
            for (const auto& c : node->children) if (c && c->kind == "expr") { if (!baseNode) baseNode = c; }
            Value base = baseNode ? eval(baseNode) : Value{};
            // evaluate index exprs
            AstPtr argsNode = nullptr;
            for (const auto& c : node->children) if (c && c->kind == "expr_list") { argsNode = c; break; }
            if (argsNode && !argsNode->children.empty()) {
                Value idx = eval(argsNode->children.back()->kind == "expr" ? argsNode->children.back() : argsNode->children[0]);
                // crude: last expr
                if (base.kind == Value::Kind::Array && idx.kind == Value::Kind::Int) {
                    size_t i = (size_t)idx.intValue.toInt64();
                    if (i < base.arrayValue.size()) return base.arrayValue[i];
                }
            }
            return Value{};
        }
    }
    // array literal
    if (!node->children.empty() && node->children[0] && node->children[0]->text == "[") {
        Value v; v.kind = Value::Kind::Array;
        for (const auto& c : node->children) {
            if (c && c->kind == "expr") v.arrayValue.push_back(eval(c));
        }
        return v;
    }
    // anonymous_template: "<|" ... "|>" '@'/'#' ...
    if (node->kind == "anonymous_template") {
        // Check if it contains a class or function
        bool hasClass = false, hasFunc = false;
        for (const auto& c : node->children) {
            if (c && c->text == "#") hasClass = true;
            if (c && c->text == "@") hasFunc = true;
        }
        if (hasClass) {
            // Find the class node (the '#' is inside a 'class' child node)
            // The class is a child with kind "class" or text "#"
            for (const auto& c : node->children) {
                if (c && (c->kind == "class" || c->text == "#")) {
                    return eval(c);
                }
            }
            // If no class child, try to find '#' in children
            for (const auto& c : node->children) {
                if (c && c->text == "#") {
                    // Construct a synthetic class expression
                    auto fn = std::make_shared<FunctionValue>();
                    AstPtr paramsNode = nullptr;
                    for (const auto& cc : node->children) if (cc && cc->kind == "func_arg_declare_list") { paramsNode = cc; break; }
                    if (paramsNode) {
                        for (const auto& p : paramsNode->children) {
                            if (p && p->kind == "func_arg_declare" && !p->children.empty() && p->children[0]->kind == "IDENTIFIER") {
                                fn->params.push_back(p->children[0]->text);
                            }
                        }
                    }
                    for (const auto& cc : node->children) {
                        if (cc && cc->kind == "scope") fn->body = cc;
                    }
                    fn->closure = current_;
                    Value v; v.kind = Value::Kind::Function; v.functionValue = fn;
                    return v;
                }
            }
        }
        if (hasFunc) {
            // Find function params and body
            auto fn = std::make_shared<FunctionValue>();
            AstPtr paramsNode = nullptr;
            for (const auto& c : node->children) if (c && c->kind == "func_arg_declare_list") { paramsNode = c; break; }
            if (paramsNode) {
                for (const auto& p : paramsNode->children) {
                    if (p && p->kind == "func_arg_declare" && !p->children.empty() && p->children[0]->kind == "IDENTIFIER") {
                        fn->params.push_back(p->children[0]->text);
                    }
                }
            }
            for (const auto& c : node->children) {
                if (c && c->kind == "scope") fn->body = c;
            }
            fn->closure = current_;
            Value v; v.kind = Value::Kind::Function; v.functionValue = fn;
            return v;
        }
        return Value{};
    }

    // class expression: contains '#' anywhere in subtree
    {
        bool hasClass = false;
        std::function<void(const AstPtr&)> findHash = [&](const AstPtr& n) {
            if (!n || hasClass) return;
            if (n->text == "#") { hasClass = true; return; }
            for (const auto& c : n->children) findHash(c);
        };
        findHash(node);
        if (hasClass) {
            // Find paramsNode and scopeNode recursively
            AstPtr paramsNode = nullptr;
            AstPtr scopeNode = nullptr;
            std::function<void(const AstPtr&)> findNodes = [&](const AstPtr& n) {
                if (!n) return;
                if (n->kind == "func_arg_declare_list" && !paramsNode) paramsNode = n;
                if (n->kind == "scope" && !scopeNode) scopeNode = n;
                for (const auto& c : n->children) findNodes(c);
            };
            findNodes(node);
        auto fn = std::make_shared<FunctionValue>();
        if (paramsNode) {
            for (const auto& p : paramsNode->children) {
                if (p && p->kind == "func_arg_declare" && !p->children.empty() && p->children[0]->kind == "IDENTIFIER") {
                    fn->params.push_back(p->children[0]->text);
                }
            }
        }
        if (scopeNode) fn->body = scopeNode;
        // Capture self if available in current scope chain
        EnvironmentPtr p = current_;
        int depth = 0;
        while (p) {
            auto it = p->vars.find("self");
            if (it != p->vars.end()) {
                fn->hasSelf = true;
                if (it->second.kind == Value::Kind::Object) {
                    fn->selfObject = it->second.objectValue;
                }
                break;
            }
            p = p->parent;
            depth++;
        }
        fn->closure = current_;
        Value v; v.kind = Value::Kind::Function; v.functionValue = fn;
        return v;
        }
    }

    // construction: base <{ args }>
    {
        bool hasConstruct = false;
        for (const auto& c : node->children) {
            if (c && c->kind == "CONSTRUCT_OPEN") { hasConstruct = true; break; }
        }
        if (hasConstruct) {
            for (const auto& c : node->children) {
            }
            AstPtr baseNode = nullptr;
            for (const auto& c : node->children) {
                if (c && (c->kind == "expr" || c->kind == "IDENTIFIER")) { if (!baseNode) baseNode = c; break; }
            }
            Value base = baseNode ? eval(baseNode) : Value{};
            if (base.kind == Value::Kind::Function && base.functionValue) {
                auto fn = base.functionValue;
                AstPtr argsNode = nullptr;
                for (const auto& c : node->children) {
                    if (c && c->kind == "expr_list") { argsNode = c; break; }
                }
                std::vector<Value> args;
                if (argsNode) {
                    for (const auto& c : argsNode->children) {
                        if (c && c->kind == "expr") args.push_back(eval(c));
                    }
                }
                auto self = std::shared_ptr<std::map<std::string, Value>>(new std::map<std::string, Value>(), ObjectDeleter());
                Value selfVal; selfVal.kind = Value::Kind::Object; selfVal.objectValue = self;
                auto scope = makeScope(fn->closure);
                scope->define("self", selfVal);
                for (size_t i = 0; i < fn->params.size(); ++i) {
                    if (i < args.size()) scope->define(fn->params[i], args[i]);
                }
                EnvironmentPtr prev = current_;
                current_ = scope;
                try {
                    if (fn->body) exec(fn->body);
                } catch (const ReturnSignal& r) {
                    if (r.value.kind != Value::Kind::Null) {
                        current_ = prev;
                        return r.value;
                    }
                }
                current_ = prev;
                return selfVal;
            }
            return Value{};
        }
    }

    // function expression
    if (!node->children.empty() && node->children[0] && node->children[0]->text == "@") {
        auto fn = std::make_shared<FunctionValue>();
        // params
        AstPtr paramsNode = nullptr;
        for (const auto& c : node->children) if (c && c->kind == "func_arg_declare_list") { paramsNode = c; break; }
        if (paramsNode) {
            for (const auto& p : paramsNode->children) {
                if (p && p->kind == "func_arg_declare" && !p->children.empty() && p->children[0]->kind == "IDENTIFIER") {
                    fn->params.push_back(p->children[0]->text);
                }
            }
        }
        for (const auto& c : node->children) {
            if (c && c->kind == "scope") fn->body = c;
        }
        fn->closure = current_;
        Value v; v.kind = Value::Kind::Function; v.functionValue = fn; return v;
    }
    // borrow &
    if (!node->children.empty() && node->children[0] && node->children[0]->text == "&") {
        if (node->children.size() >= 2) return eval(node->children[1]);
    }
    // dot
    {
        bool hasDot = false;
        for (const auto& c : node->children) if (c && c->text == ".") { hasDot = true; break; }
        if (hasDot) {
            AstPtr baseNode = nullptr;
            AstPtr memberNode = nullptr;
            for (const auto& c : node->children) {
                if (c && c->kind == "expr") { if (!baseNode) baseNode = c; }
                if (c && c->kind == "IDENTIFIER") memberNode = c;
            }
            Value base = baseNode ? eval(baseNode) : Value{};
            if (base.kind == Value::Kind::Object && memberNode && base.objectValue) {
                auto it = base.objectValue->find(memberNode->text);
                if (it != base.objectValue->end()) return it->second;
                for (const auto& kv : *base.objectValue) {
                }
                fflush(stderr);
            } else {
                fflush(stderr);
            }
            return Value{};
        }
    }
    // default: check children
    for (const auto& c : node->children) {
        if (c && c->kind == "expr") eval(c);
    }
    return Value{};
}

void Interpreter::exec(const AstPtr& node) {
    if (!node) return;
    if (node->kind == "scope") {
        // debug: print scope depth
    }
    if (node->kind == "scope") {
        bool hasReturn = false;
        for (const auto& c : node->children) {
            if (c && c->kind == "stmt" && !c->children.empty() && c->children[0] && c->children[0]->kind == "RETURN") hasReturn = true;
        }
        for (const auto& c : node->children) exec(c);
        return;
    }
    if (node->kind == "stmt") {
        if (node->children.empty()) return;
        const AstPtr& first = node->children[0];
        if (node->children.size() == 3 && node->children[1] && node->children[1]->kind == "CHAR(61)") {
            Value v = eval(node->children[2]);
            // member assignment: base.member = value
            AstPtr lhs = node->children[0];
            if (lhs && lhs->kind == "expr" && lhs->children.size() == 1 && lhs->children[0] && lhs->children[0]->kind == "IDENTIFIER") {
                lhs = lhs->children[0];
            }
            bool hasDot = false;
            if (lhs && lhs->kind == "expr") {
                for (const auto& c : lhs->children) {
                    if (c && c->text == ".") { hasDot = true; break; }
                }
            }
            if (hasDot) {
                AstPtr baseNode = nullptr;
                AstPtr memberNode = nullptr;
                for (const auto& c : lhs->children) {
                    if (c && c->kind == "expr") { if (!baseNode) baseNode = c; }
                    if (c && c->kind == "IDENTIFIER") memberNode = c;
                }
                if (baseNode && memberNode) {
                    Value base = eval(baseNode);
                    if (base.kind == Value::Kind::Object && base.objectValue) {
                        (*base.objectValue)[memberNode->text] = v;
                        return;
                    }
                }
            }
            if (lhs && lhs->kind == "IDENTIFIER") {
                current_->set(lhs->text, v);
            }
            return;
        }
        // block
        if (first && first->kind == "CHAR(123)") {
            auto scope = makeScope(current_);
            EnvironmentPtr prev = current_;
            current_ = scope;
            for (const auto& c : node->children) exec(c);
            current_ = prev;
            return;
        }
        // variable declaration
        if (first && first->kind == "IDENTIFIER" && node->children.size() >= 2 &&
            node->children[1] && node->children[1]->kind == "declare") {
            Value v;
            // initializer
            for (size_t i = 0; i < node->children.size(); ++i) {
                if (node->children[i] && node->children[i]->kind == "CHAR(61)" && i + 1 < node->children.size()) {
                    v = eval(node->children[i + 1]);
                }
            }
            current_->define(first->text, v);
            return;
        }
        // assignment
        if (node->children.size() == 3 && node->children[1] && node->children[1]->kind == "CHAR(61)") {
            Value v = eval(node->children[2]);
            // member assignment: base.member = value
            if (node->children[0] && node->children[0]->kind == "expr") {
                bool hasDot = false;
                for (const auto& c : node->children[0]->children) {
                    if (c && c->text == ".") { hasDot = true; break; }
                }
                if (hasDot) {
                    AstPtr baseNode = nullptr;
                    AstPtr memberNode = nullptr;
                    for (const auto& c : node->children[0]->children) {
                        if (c && c->kind == "expr") { if (!baseNode) baseNode = c; }
                        if (c && c->kind == "IDENTIFIER") memberNode = c;
                    }
                    if (baseNode && memberNode) {
                        Value base = eval(baseNode);
                        if (base.kind == Value::Kind::Object && base.objectValue) {
                            (*base.objectValue)[memberNode->text] = v;
                            return;
                        }
                    }
                }
            }
            // Handle IDENTIFIER or expr wrapping IDENTIFIER
            AstPtr lhs = node->children[0];
            if (lhs && lhs->kind == "expr" && lhs->children.size() == 1 && lhs->children[0] && lhs->children[0]->kind == "IDENTIFIER") {
                lhs = lhs->children[0];
            }
            if (lhs && lhs->kind == "IDENTIFIER") {
                current_->set(lhs->text, v);
            }
            return;
        }
        // if
        if (node->children.size() >= 2 && node->children[1] && node->children[1]->kind == "CHAR(63)") {
            Value cond = eval(node->children[0]);
            if (truthy(cond).boolValue) {
                for (const auto& c : node->children) {
                    if (c && c->kind == "scope") exec(c);
                }
            } else {
                // else scope is after ':' '{' ...; exec last scope
                // simple: exec all scopes? We'll exec scopes after colon.
                bool seenColon = false;
                for (const auto& c : node->children) {
                    if (c && c->text == ":") seenColon = true;
                    if (seenColon && c && c->kind == "scope") exec(c);
                }
            }
            return;
        }
        // while
        if (first && first->kind == "CHAR(36)") {
            // condition: first expr child
            while (true) {
                Value cond;
                bool hasCond = false;
                for (const auto& c : node->children) {
                    if (c && c->kind == "expr") { cond = eval(c); hasCond = true; break; }
                }
                if (hasCond && !truthy(cond).boolValue) break;
                try {
                    for (const auto& c : node->children) {
                        if (c && c->kind == "scope") exec(c);
                    }
                } catch (const BreakSignal&) { break; }
                catch (const ContinueSignal&) { continue; }
                catch (const ReturnSignal& r) { throw r; }
            }
            return;
        }
        // for loop: $ ( IDENTIFIER : expr ) { scope }
        // detect identifier child before ':'
        {
            AstPtr idNode = nullptr;
            AstPtr exprNode = nullptr;
            for (const auto& c : node->children) {
                if (c && c->kind == "IDENTIFIER" && !idNode) idNode = c;
                if (c && c->kind == "expr") { if (!exprNode) exprNode = c; }
            }
            // only treat as for if there's an identifier and expr and scope
            bool hasScope = false;
            for (const auto& c : node->children) if (c && c->kind == "scope") hasScope = true;
            if (first && first->kind == "CHAR(36)" && idNode && exprNode && hasScope) {
                Value range = eval(exprNode);
                if (range.kind == Value::Kind::Array) {
                    for (const auto& v : range.arrayValue) {
                        auto scope = makeScope(current_);
                        EnvironmentPtr prev = current_;
                        current_ = scope;
                        scope->define(idNode->text, v);
                        try {
                            for (const auto& c : node->children) {
                                if (c && c->kind == "scope") exec(c);
                            }
                        } catch (const BreakSignal&) { current_ = prev; break; }
                        catch (const ContinueSignal&) { current_ = prev; continue; }
                        catch (const ReturnSignal& r) { current_ = prev; throw r; }
                        current_ = prev;
                    }
                }
                return;
            }
        }
        // unwrap: '*' '(' expr_list ')' '{' scope '}' [':' '{' scope '}']
        if (first && first->kind == "CHAR(42)" && node->children.size() >= 2 &&
            node->children[1] && node->children[1]->kind == "CHAR(40)") {
            // Find expr_list node
            AstPtr listNode = nullptr;
            for (const auto& c : node->children) {
                if (c && c->kind == "expr_list") { listNode = c; break; }
            }
            // Evaluate each expr, derive alias name from last identifier component
            std::vector<std::pair<std::string, Value>> unwrapped;
            bool ok = true;
            if (listNode) {
                for (const auto& e : listNode->children) {
                    if (!e || e->kind != "expr") continue;
                    Value v = eval(e);
                    if (v.kind == Value::Kind::Null) ok = false;
                    // Derive name: last IDENTIFIER in the expr
                    std::string name;
                    std::function<void(const AstPtr&)> findLastId = [&](const AstPtr& n) {
                        if (!n) return;
                        if (n->kind == "IDENTIFIER") name = n->text;
                        for (const auto& c : n->children) findLastId(c);
                    };
                    findLastId(e);
                    if (name.empty()) name = "_unwrap";
                    unwrapped.push_back({name, v});
                }
            }
            // Find scopes
            std::vector<AstPtr> scopes;
            for (const auto& c : node->children) {
                if (c && c->kind == "scope") scopes.push_back(c);
            }
            if (ok && !scopes.empty()) {
                auto scope = makeScope(current_);
                EnvironmentPtr prev = current_;
                current_ = scope;
                for (auto& [name, v] : unwrapped) scope->define(name, v);
                try {
                    exec(scopes[0]);
                } catch (...) {
                    current_ = prev;
                    throw;
                }
                current_ = prev;
            } else if (!ok && scopes.size() >= 2) {
                // run else scope
                auto scope = makeScope(current_);
                EnvironmentPtr prev = current_;
                current_ = scope;
                try {
                    exec(scopes[1]);
                } catch (...) {
                    current_ = prev;
                    throw;
                }
                current_ = prev;
            }
            return;
        }

        // return
        if (first && first->kind == "RETURN") {
            Value v;
            if (node->children.size() >= 2) {
                v = eval(node->children[1]);
            }
            throw ReturnSignal{v};
        }
        // break/continue
        if (first && (first->kind == "CONTINUE" || first->kind == "BREAK")) {
            if (first->kind == "CONTINUE") throw ContinueSignal{};
            else throw BreakSignal{};
        }
        // block
        if (first && first->kind == "CHAR(123)") {
            auto scope = makeScope(current_);
            EnvironmentPtr prev = current_;
            current_ = scope;
            for (const auto& c : node->children) exec(c);
            current_ = prev;
            return;
        }
        // Overloading: expr "^=" expr  or  OP "^=" expr
        {
            bool hasOverload = false;
            for (const auto& c : node->children) {
                if (c && c->kind == "OVERLOAD") { hasOverload = true; break; }
            }
            if (hasOverload) {
                // Determine the overload key and the function value
                std::string key;
                Value fnVal;
                // Find the function (last expr)
                for (int i = (int)node->children.size() - 1; i >= 0; --i) {
                    if (node->children[i] && node->children[i]->kind == "expr") {
                        fnVal = eval(node->children[i]);
                        break;
                    }
                }
                // The key is the first non-OVERLOAD token's text
                for (const auto& c : node->children) {
                    if (c && c->kind != "OVERLOAD" && c->kind != "expr") {
                        key = c->text;
                        break;
                    }
                    if (c && c->kind == "expr" && c->children.size() == 1 && c->children[0] && c->children[0]->kind == "IDENTIFIER") {
                        // Could be function name like g
                        // Only use if it's the first expr
                    }
                }
                // If key is still empty, try first identifier
                if (key.empty()) {
                    for (const auto& c : node->children) {
                        if (c && c->kind == "IDENTIFIER") { key = c->text; break; }
                        if (c && c->kind == "expr" && c->children.size() == 1 && c->children[0] && c->children[0]->kind == "IDENTIFIER") {
                            key = c->children[0]->text; break;
                        }
                    }
                }
                if (!key.empty() && fnVal.kind == Value::Kind::Function) {
                    overloads_[key] = fnVal;
                }
                return;
            }
        }

        // alias: IDENTIFIER ':=' type/expr
        if (first && first->kind == "IDENTIFIER" && node->children.size() >= 3 &&
            node->children[1] && (node->children[1]->kind == "ALIAS_PUBLIC" || node->children[1]->kind == "ALIAS_PRIVATE")) {
            Value v = eval(node->children[2]);
            current_->define(first->text, v);
            lastValue_ = v;
            return;
        }
        // expression statement
        if (first && first->kind == "expr") {
            lastValue_ = eval(first);
            return;
        }
        // self function / self class as statement
        if (first && (first->kind == "SELF_FUNC" || first->kind == "SELF_CLASS")) {
            lastValue_ = eval(first);
            return;
        }
        // default: exec children
        for (const auto& c : node->children) exec(c);
        return;
    }
    if (node->kind == "start" || node->kind == "expr" || node->kind == "type") {
        for (const auto& c : node->children) exec(c);
        return;
    }
}

} // namespace at
