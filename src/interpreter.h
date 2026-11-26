#pragma once

#include "ast.h"
#include "bigint.h"
#include "semantic.h"

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <llvm/IR/LLVMContext.h>

namespace at {

struct Value;
struct Environment;

using EnvironmentPtr = std::shared_ptr<Environment>;

// Native function type: takes vector of args, returns Value
using NativeFn = std::function<Value(const std::vector<Value>&)>;

struct FunctionValue {
    std::vector<std::string> params;
    AstPtr body; // scope node
    EnvironmentPtr closure;
    NativeFn native; // if non-null, this is a native function
    bool isNative = false;
    bool hasSelf = false;
    std::shared_ptr<std::map<std::string, Value>> selfObject;
};

struct Value {
    enum class Kind { Null, Int, Decimal, Bool, String, Array, Function, Object, OpaquePtr };
    Kind kind = Kind::Null;
    BigInt intValue = 0;  // BigInt for arbitrary precision
    double decimalValue = 0.0;
    bool boolValue = false;
    std::string stringValue;
    std::vector<Value> arrayValue;
    std::shared_ptr<FunctionValue> functionValue;
    std::shared_ptr<std::map<std::string, Value>> objectValue;
    void* opaquePtrValue = nullptr; // for C library handles
    std::function<void(void*)> opaqueDestructor;
    std::function<void(Value&)> destructor;
};

struct ObjectDeleter {
    void operator()(std::map<std::string, Value>* p) const {
        if (p) {
            for (const auto& [name, val] : *p) {
                if (val.destructor) {
                    Value copy = val;
                    val.destructor(copy);
                }
                if (val.kind == Value::Kind::OpaquePtr && val.opaquePtrValue && val.opaqueDestructor) {
                    val.opaqueDestructor(val.opaquePtrValue);
                }
            }
            delete p;
        }
    }
};

inline Value makeOpaquePtr(void* handle, std::function<void(void*)> destructor) {
    Value v;
    v.kind = Value::Kind::OpaquePtr;
    v.opaquePtrValue = handle;
    v.opaqueDestructor = std::move(destructor);
    return v;
}

inline Value makeDestructible(Value v, std::function<void(Value&)> destructor) {
    v.destructor = std::move(destructor);
    return v;
}

struct Environment {
    std::map<std::string, Value> vars;
    EnvironmentPtr parent;
    Value get(const std::string& name) const;
    void set(const std::string& name, const Value& value);
    void define(const std::string& name, const Value& value);
};

// Forward declaration
class CodeGenLower;

class Interpreter {
public:
    Interpreter();
    ~Interpreter();

    Value run(const AstPtr& root);

    void registerNative(const std::string& name, NativeFn fn);

    bool jitModule(const AstPtr& root, SemanticAnalyzer& sem,
                   bool isMainModule = false,
                   const std::vector<std::string>& initFuncs = {},
                   const std::string& initFuncName = "");

    int64_t callJitSymbol(const std::string& name);

private:
    EnvironmentPtr globals_;
    EnvironmentPtr current_;
    Value lastValue_;
    std::shared_ptr<FunctionValue> currentFunction_;
    std::map<std::string, Value> modules_;
    std::map<std::string, Value> overloads_;
    std::string activeOverload_;

    std::unique_ptr<llvm::LLVMContext> jitContext_;
    std::unique_ptr<CodeGenLower> lower_;
    struct JitImpl;
    std::unique_ptr<JitImpl> jitImpl_;

    bool ensureJit();
    void registerNativesToJit();

    struct ReturnSignal { Value value; };
    struct BreakSignal {};
    struct ContinueSignal {};

    Value loadModule(const std::string& path);
    Value loadModuleFFI(const std::string& path);

    EnvironmentPtr makeScope(EnvironmentPtr parent);
    Value truthy(const Value& v) const;
    Value applyBinary(const std::string& op, const Value& a, const Value& b);
    // Tree-walking execution (fallback for imports)
    Value evalTree(const AstPtr& node);
    void execTree(const AstPtr& node);

    Value applyUnary(const std::string& op, const Value& a);
};

} // namespace at
