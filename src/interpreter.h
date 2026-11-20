#pragma once

#include "ast.h"
#include "bigint.h"

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

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
    // Destructor for opaque handles: called when refcount drops to 0
    std::function<void(void*)> opaqueDestructor;
    // General destructor hook: called for any Value kind when the containing
    // object map is destroyed (broader RC cleanup beyond opaque handles).
    std::function<void(Value&)> destructor;
};

// Custom deleter for object maps: calls destructors for any value kind
struct ObjectDeleter {
    void operator()(std::map<std::string, Value>* p) const {
        if (p) {
            for (const auto& [name, val] : *p) {
                // General destructor hook (broader RC cleanup beyond opaque handles)
                if (val.destructor) {
                    Value copy = val;
                    val.destructor(copy);
                }
                // Opaque handle destructor (existing mechanism)
                if (val.kind == Value::Kind::OpaquePtr && val.opaquePtrValue && val.opaqueDestructor) {
                    val.opaqueDestructor(val.opaquePtrValue);
                }
                // Recursively handle nested objects (shared_ptr handles refcount)
            }
            delete p;
        }
    }
};

// Helper to create an opaque handle Value
inline Value makeOpaquePtr(void* handle, std::function<void(void*)> destructor) {
    Value v;
    v.kind = Value::Kind::OpaquePtr;
    v.opaquePtrValue = handle;
    v.opaqueDestructor = std::move(destructor);
    return v;
}

// Helper to create a Value with a general destructor hook (broader RC cleanup)
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

class Interpreter {
public:
    Interpreter();
    Value run(const AstPtr& root);

    // Register a native function in the global environment.
    void registerNative(const std::string& name, NativeFn fn);

private:
    EnvironmentPtr globals_;
    EnvironmentPtr current_;
    Value lastValue_;
    std::shared_ptr<FunctionValue> currentFunction_;
    std::map<std::string, Value> modules_;
    // Operator/function overloads: op/function name -> function value
    std::map<std::string, Value> overloads_;
    // Currently active overload (to prevent infinite recursion)
    std::string activeOverload_;

    Value loadModule(const std::string& path);

    // Load an ELF object/shared library (.o or .so) and expose its exported
    // functions as native callable values via dlopen/dlsym FFI. Reads .at.sigs
    // for type signatures; calls use the i64 calling convention (all args and
    // return are int64). Returns a module object whose fields are the imported
    // functions.
    Value loadModuleFFI(const std::string& path);

    struct ReturnSignal { Value value; };
    struct BreakSignal {};
    struct ContinueSignal {};

    Value eval(const AstPtr& node);
    void exec(const AstPtr& node);

    EnvironmentPtr makeScope(EnvironmentPtr parent);
    Value truthy(const Value& v) const;
    Value applyBinary(const std::string& op, const Value& a, const Value& b);
    Value applyUnary(const std::string& op, const Value& a);
};

} // namespace at
