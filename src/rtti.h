#pragma once

#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

#include <map>
#include <string>
#include <vector>

namespace at {

// RTTI descriptor layout (matches C struct in runtime):
//   struct AtRtti {
//       uint64_t size;
//       uint64_t align;
//       const char* name;
//       void (*drop)(void*);
//       void (*copy)(void*, const void*);
//       void (*print)(const void*);
//   };
struct RttiType {
    std::string name;
    uint64_t size = 0;
    uint64_t align = 0;
    bool hasDrop = false;
    bool hasCopy = false;
    bool hasPrint = false;
};

class RttiManager {
public:
    explicit RttiManager(llvm::Module* module);

    // Get or create an RTTI global for a primitive/named type.
    llvm::Constant* getRtti(const std::string& name, uint64_t size, uint64_t align);

    // Declare runtime helper functions (drop, copy, print) for a type.
    void declareRuntimeOps(const std::string& typeName);

    // Get the section name used for RTTI globals.
    static const char* rttiSection() { return ".rodata.rtti"; }

private:
    llvm::Module* module_;
    llvm::LLVMContext& ctx_;
    std::map<std::string, llvm::Constant*> rttiCache_;
    llvm::StructType* rttiStructTy_ = nullptr;
};

} // namespace at
