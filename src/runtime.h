#pragma once

#include <cstdint>

namespace at {

// Runtime type descriptor passed as hidden parameter for template parameters.
// This is the "meta ABI": template code operates on opaque pointers + descriptors,
// and LTO/link time can specialize when concrete types are known.
struct TypeDescriptor {
    int64_t size;          // size of the type in bytes
    int64_t alignment;     // alignment requirement
    int64_t type_id;       // primitive type id (0=int, 1=decimal, 2=string, ...)
    // operation slots (may be null for types that don't support them)
    void* add;             // (void* dst, const void* a, const void* b)
    void* eq;              // (int  dst, const void* a, const void* b) -> bool
    void* destroy;         // (void* obj)
};

// Built-in type descriptors, defined in runtime.cpp
extern TypeDescriptor g_type_int;
extern TypeDescriptor g_type_decimal;
extern TypeDescriptor g_type_string;

const TypeDescriptor* typeDescriptorForId(int64_t typeId);
int64_t typeIdForPrimitive(const char* name);

// Runtime helper calls used by generated template code
extern "C" void* at_template_add(int64_t typeId, void* a, const void* b);
extern "C" int  at_template_eq (int64_t typeId, const void* a, const void* b);
extern "C" void* at_template_alloc(int64_t size, int64_t alignment);
extern "C" void  at_template_destroy(const TypeDescriptor* desc, void* obj);

} // namespace at
