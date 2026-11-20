---
title: Language Features
tags: [language, features, syntax]
sourceRefs: [src/syntax.ast.y, tests/test_simple.at, src/main.cpp]
lastReviewed: 2026-10-25
---

# Language Features

The AT language implements a rich set of features designed for modern programming with static typing and functional elements.

## Core Syntax Elements

### Variable Declaration and Assignment
```at
x : int = 42;
y : int = x + 1;
```

Variables must be explicitly typed and can be assigned at declaration time.

### Expressions
Supports standard arithmetic, logical, and bitwise operators:
- Arithmetic: `+`, `-`, `*`, `/`, `%`, `^`
- Logical: `||`, `&&`, `^^` (xor)
- Comparison: `<`, `>`, `<=`, `>=`, `==`, `!=`
- Assignment: `=`

### Function Definitions
Functions are defined using the `@` syntax:
```at
add := @(a: int, b: int) -> int {
    a + b
}
```

### Class Definitions
Classes use the `#` syntax:
```at
Point := # (x: int, y: int) {
    // class body
}
```

## Advanced Features

### Templates/Generics
Support for generic programming constructs:
```at
List := <|T|> # (items: T[]) {
    // template class
}
```

### Type Casting
Explicit casting between types:
```at
value := 42;
casted := value -> float;
```

### Modules and Imports
Module system allows code organization and reuse:
```at
import "math.at"
```

## Operator Overloading

Custom operators can be defined using special syntax, including:
- `||` (logical OR)
- `&&` (logical AND) 
- `^^` (exclusive OR)
- Various assignment operators

## Type System

### Primitive Types
- int (integer)
- float (floating point)  
- string (text)
- bool (boolean)

### Composite Types
- Arrays: `int[]`
- Pointers: `*int`
- Function pointers: `@?(int, int) -> int`

### Type Inference
The language supports type inference in many contexts to reduce verbosity.

## Control Flow

### Conditional Expressions
```at
result := if (condition) { true_value } else { false_value };
```

### Loops
Support for various loop constructs including:
- While loops
- For loops with iterators