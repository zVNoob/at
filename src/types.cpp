#include "types.h"

namespace at {

TypePtr makePrimitive(PrimitiveType::Kind kind, const std::string& name) {
    auto t = std::make_shared<PrimitiveType>();
    t->kind = kind;
    t->name = name;
    return t;
}

TypePtr makePointer(TypePtr pointee) {
    auto t = std::make_shared<PointerType>();
    t->name = "*" + (pointee ? pointee->name : "?");
    t->pointee = std::move(pointee);
    return t;
}

TypePtr makeReference(TypePtr referent, bool isMutable) {
    auto t = std::make_shared<ReferenceType>();
    t->name = "&" + (referent ? referent->name : "?");
    t->referent = std::move(referent);
    t->isMutable = isMutable;
    return t;
}

TypePtr makeArray(TypePtr element) {
    auto t = std::make_shared<ArrayType>();
    t->name = (element ? element->name : "?") + "[]";
    t->element = std::move(element);
    return t;
}

TypePtr makeFunction(std::vector<TypePtr> params, TypePtr returnType) {
    auto t = std::make_shared<FunctionType>();
    t->params = std::move(params);
    t->returnType = std::move(returnType);
    t->name = "function";
    return t;
}

bool typesCompatible(const TypePtr& a, const TypePtr& b) {
    if (!a || !b) return true;
    if (a->name == "unknown" || b->name == "unknown") return true;
    if (a->name == b->name) return true;
    // numeric promotion: int -> decimal
    auto pa = std::dynamic_pointer_cast<PrimitiveType>(a);
    auto pb = std::dynamic_pointer_cast<PrimitiveType>(b);
    if (pa && pb) {
        if (pa->kind == PrimitiveType::Kind::Int && pb->kind == PrimitiveType::Kind::Decimal) return true;
    }
    return false;
}

} // namespace at
