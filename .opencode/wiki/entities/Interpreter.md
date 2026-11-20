---
title: Interpreter
tags: [runtime, execution, interpreter]
sourceRefs: [src/interpreter.cpp, src/interpreter.h]
lastReviewed: 2026-10-25
---

# Interpreter

The interpreter provides runtime execution of AT language code directly in memory without compilation to object files.

## Purpose

The interpreter allows for:
- Immediate execution of code 
- Debugging and testing without compilation steps
- Dynamic evaluation of expressions
- Module loading and execution

## Architecture

### Environment Management
The interpreter uses environments (scopes) to track variable bindings:
```cpp
class Environment {
    std::map<std::string, Value> variables;
    EnvironmentPtr parent;
};
```

### Value Representation
Values are represented with a variant type supporting:
- Integers
- Floating-point numbers  
- Strings
- Booleans
- Void values

## Key Functions

### Main Execution Loop
```cpp
Value Interpreter::run(const AstPtr& root) {
    // Execute statements and return final value
}
```

### Expression Evaluation
```cpp
Value Interpreter::eval(const AstPtr& node) {
    // Evaluate individual expressions
}
```

### Statement Execution
```cpp
void Interpreter::exec(const AstPtr& node) {
    // Execute statements like assignments, conditionals
}
```

## Module System

The interpreter supports importing modules:
```at
import "math.at"
```

Modules are loaded and executed in their own environments to maintain scope isolation.

## Integration

The interpreter can be used as an alternative to the code generator:
```cpp
at::Interpreter interp;
at::Value result = interp.run(ast);
// Direct execution without compilation
```