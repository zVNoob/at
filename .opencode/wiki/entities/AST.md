---
title: Abstract Syntax Tree (AST)
tags: [compiler, ast, parsing]
sourceRefs: [src/ast.h, src/parser.cpp, tools/dump_ast.cpp]
lastReviewed: 2026-10-25
---

# Abstract Syntax Tree (AST)

The AST is the core data structure representing the parsed source code in the AT language implementation.

## Structure

The AST is defined in `src/ast.h` with two main components:

### SourceSpan
```cpp
struct SourceSpan {
    std::size_t line = 0;
    std::size_t column = 0;
};
```

### AstNode
```cpp
struct AstNode {
    std::string kind;          // production / node kind
    std::string text;          // lexeme or operator text
    SourceSpan span;
    std::vector<AstPtr> children;

    AstNode() = default;
    AstNode(std::string k, std::string t = "", SourceSpan s = {})
        : kind(std::move(k)), text(std::move(t)), span(s) {}
};
```

## Usage

The AST is constructed during parsing and represents the hierarchical structure of the source code. Each node corresponds to a grammar rule from the language specification.

## AST Node Kinds

Common node kinds include:
- `expr` - Expression nodes
- `func_arg_declare` - Function argument declarations  
- `type` - Type specifications
- `declare` - Visibility declarations (`public`, `private`, etc.)
- `cast_stmt` - Type casting operations
- `class` - Class definitions

## AST Dumping

The `dump_ast` tool (in `tools/dump_ast.cpp`) provides a visualization of the AST structure:
```bash
./dump_ast "x : int = 42;"
```

This outputs the tree structure showing how the parser has interpreted the source code.