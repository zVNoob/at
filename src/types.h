#pragma once

#include <memory>
#include <string>
#include <vector>

namespace at {

struct AstNode;
using AstPtr = std::shared_ptr<AstNode>;

struct Type;
using TypePtr = std::shared_ptr<Type>;

struct Type {
    virtual ~Type() = default;
    std::string name;
    int templateParams = 0;
};

struct PrimitiveType : Type {
    enum class Kind { Int, Decimal, String, Bool, Void, Unknown };
    Kind kind = Kind::Unknown;
};

struct PointerType : Type {
    TypePtr pointee;
};

struct ReferenceType : Type {
    TypePtr referent;
    bool isMutable = false;
};

struct ArrayType : Type {
    TypePtr element;
};

struct FunctionType : Type {
    std::vector<TypePtr> params;
    std::vector<std::string> paramNames;
    TypePtr returnType;
    std::vector<std::string> templateParamNames;
    // Body scope node for constraint checking.
    void* body = nullptr;
};

struct ClassType : Type {
    // Filled in by semantic analyzer.
    void* scope = nullptr;
};

struct AliasType : Type {
    TypePtr target;
};

TypePtr makePrimitive(PrimitiveType::Kind kind, const std::string& name);
TypePtr makePointer(TypePtr pointee);
TypePtr makeReference(TypePtr referent, bool isMutable);
TypePtr makeArray(TypePtr element);
TypePtr makeFunction(std::vector<TypePtr> params, TypePtr returnType);

bool typesCompatible(const TypePtr& a, const TypePtr& b);

} // namespace at
