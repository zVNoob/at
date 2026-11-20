---
title: Code Generator
tags: [compiler, code-generation, llvm]
sourceRefs: [src/codegen.cpp, src/codegen.h]
lastReviewed: 2026-10-25
---

# Code Generator

The code generator compiles AT language code into LLVM IR (Intermediate Representation) which can then be optimized and compiled to machine code.

## Purpose

The code generator transforms the AST into executable code using LLVM, enabling:
- High-performance compilation
- Platform-specific optimizations  
- Integration with standard toolchains
- Generation of object files for linking

## Architecture

### LLVM Integration
The generator uses LLVM's C++ API to:
- Create LLVM IR instructions
- Manage function and basic block construction
- Handle type mapping between AT types and LLVM types
- Generate optimized code

### Compilation Process
1. **Type Translation**: Convert AT types to LLVM types
2. **Function Generation**: Create LLVM functions for AT functions
3. **Expression Codegen**: Generate IR for expressions  
4. **Control Flow**: Handle conditionals, loops, etc.
5. **Module Output**: Write final LLVM IR or object file

## Key Components

### Type Mapping
```cpp
llvm::Type* CodeGen::llvmTypeForAst(const AstPtr& typeNode) {
    // Map AT types to LLVM types (int -> i32, etc.)
}
```

### Expression Generation
```cpp
llvm::Value* CodeGen::codegenExpr(const AstPtr& node) {
    // Generate LLVM IR for expressions
}
```

### Statement Generation
```cpp
void CodeGen::codegenStmt(const AstPtr& node) {
    // Generate IR for statements
}
```

## Compilation Workflow

The compilation process:
1. Parse source code into AST
2. Perform semantic analysis  
3. Generate LLVM IR from AST
4. Optimize the IR
5. Emit object file or bitcode

## Integration

```cpp
at::CodeGen cg;
bool success = cg.compile(ast, "output.o");
// Generates executable object file
```

The generated code can be linked with other object files and libraries.