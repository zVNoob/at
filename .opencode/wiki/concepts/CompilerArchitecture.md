---
title: Compiler Architecture
tags: [compiler, architecture, design-patterns]
sourceRefs: [src/main.cpp, src/parser.h, src/semantic.h, src/codegen.h, src/interpreter.h]
lastReviewed: 2026-10-25
---

# Compiler Architecture

The AT language implementation follows a classic multi-pass compiler architecture with distinct phases that work together to transform source code into executable code.

## Phases Overview

### 1. Lexical Analysis
Converts source text into tokens using flex (not directly shown in the codebase but integrated).

### 2. Parsing  
Constructs an Abstract Syntax Tree (AST) from tokens according to grammar rules defined in `src/syntax.ast.y`.

### 3. Semantic Analysis
Validates the AST for semantic correctness including:
- Type checking
- Scope resolution  
- Symbol table construction
- Error detection

### 4. Code Generation
Transforms the validated AST into executable code using LLVM.

### 5. Execution/Linking
Either execute directly (interpreter) or generate object files for linking.

## Architecture Components

### Frontend (Source Processing)
- **Parser**: Converts source to AST
- **Semantic Analyzer**: Validates and enriches AST with type information
- **Intermediate Representation**: AST serves as IR between parsing and code generation

### Backend (Code Generation)  
- **Code Generator**: Transforms AST to LLVM IR
- **LLVM Optimizer**: Applies optimizations to IR
- **Code Emitter**: Generates machine code or object files

## Design Patterns Used

### Visitor Pattern
The semantic analyzer and code generator use visitor-like patterns to traverse the AST:
```cpp
// Semantic analysis visits each node
void SemanticAnalyzer::check(const AstPtr& node, SymbolTable* scope) {
    // Handle different node types based on kind
}

// Code generation visits each node  
llvm::Value* CodeGen::codegenExpr(const AstPtr& node) {
    // Generate IR for expression nodes
}
```

### Component-Based Design
Each phase is implemented as a separate component:
- Parser component
- Semantic analyzer component  
- Interpreter component
- Code generator component

## Pipeline Integration

The main execution pipeline in `src/main.cpp` demonstrates the flow:

```cpp
// 1. Parse source
at::AstPtr ast = at::parseSource(input);

// 2. Analyze semantics  
at::SemanticAnalyzer sem;
sem.analyze(ast);

// 3. Execute or compile based on mode
if (compileMode) {
    at::CodeGen cg;
    cg.compile(ast, outputFile);
} else {
    at::Interpreter interp;
    interp.run(ast);
}
```

This architecture enables:
- Easy modification of individual phases
- Reuse of components (AST can be used for both interpretation and compilation)
- Clear separation of concerns
- Extensibility for adding new features