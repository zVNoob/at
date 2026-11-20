---
title: Semantic Analyzer
tags: [compiler, semantic-analysis, type-checking]
sourceRefs: [src/semantic.cpp, src/symbols.h, src/types.h]
lastReviewed: 2026-10-25
---

# Semantic Analyzer

The semantic analyzer performs static analysis on the Abstract Syntax Tree (AST) to ensure code correctness and perform type checking.

## Purpose

The semantic analyzer:
- Validates that all identifiers are declared before use
- Performs type checking and inference  
- Ensures expressions are used with compatible types
- Checks for semantic errors like undeclared variables or incorrect function calls
- Builds symbol tables for scope management

## Key Components

### Symbol Table Management
The analyzer maintains symbol tables for different scopes:
- Global scope
- Function scopes  
- Class scopes
- Block scopes

### Type System
The type system supports:
- Primitive types (int, float, string, bool)
- Function types
- Class types
- Template/generic types
- Pointer types

## Core Functions

### Analysis Process
1. **Collection Phase**: Traverse AST to collect declarations and build symbol tables
2. **Checking Phase**: Validate usage of symbols against their declarations
3. **Type Checking**: Verify expression types match expected types

### Error Reporting
The analyzer collects semantic errors during analysis:
```cpp
void SemanticAnalyzer::error(const AstPtr& node, const std::string& message) {
    // Record error with location information
}
```

## Integration

The semantic analyzer is called after parsing and before code generation:
```cpp
at::AstPtr ast = at::parseSource(input);
at::SemanticAnalyzer sem;
sem.analyze(ast);
// Check for errors: sem.errors().size()
```