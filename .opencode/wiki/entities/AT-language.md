---
title: AT Language
tags: [language, programming-language, compiler]
sourceRefs: [src/syntax.ast.y, src/main.cpp, tests/test_simple.at]
lastReviewed: 2026-10-25
---

# AT Programming Language

The AT language is a statically-typed programming language with features including:

## Key Features
- **Static Typing**: Variables must be explicitly typed or inferred
- **Function Syntax**: Functions defined using `@` syntax
- **Class System**: Classes defined using `#` syntax  
- **Templates/Generics**: Support for generic programming constructs
- **Operator Overloading**: Custom operators like `||`, `&&`, `^^`
- **Type Casting**: Explicit casting between types

## Syntax Examples

### Variable Declaration and Assignment
```at
x : int = 42;
y : int = x + 1;
```

### Function Definition
```at
add := @(a: int, b: int) -> int {
    a + b
}
```

### Class Definition  
```at
Point := # (x: int, y: int) {
    // class body
}
```

## Language Grammar

The language is defined using a Bison/Yacc grammar in `src/syntax.ast.y` with support for:
- Expressions with operator precedence and associativity
- Type declarations and inference
- Function and method definitions
- Class and template constructs
- Control flow statements

## Implementation Status

This implementation includes:
- Lexical analysis and parsing
- Semantic analysis with type checking  
- Runtime interpretation
- LLVM-based code generation
- Module system and imports

The language supports both immediate execution and compilation to object files.