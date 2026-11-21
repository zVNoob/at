---
title: Project Overview
tags: [overview, project, compiler]
sourceRefs: [src/main.cpp, src/parser.h, src/semantic.h, src/codegen.h, src/interpreter.h, Makefile]
lastReviewed: 2026-11-21
---

# Project Overview — `at`

`at` is a statically-typed programming language and compiler implemented in C++. It uses LLVM 17 as its code generation backend and supports both ahead-of-time compilation (to object files/executables/shared libraries) and direct interpretation of source.

## Pipeline

1. **Lexing & parsing** — `src/lexer.cpp` and the Bison/Yacc grammar `src/syntax.ast.y` produce an AST.
2. **Semantic analysis** — `src/semantic.cpp` performs type checking and scope resolution.
3. **Code generation** — `src/codegen.*` emits LLVM IR/object files.
4. **Execution** — either the in-process `Interpreter` (`src/interpreter.cpp`) or linking via external `cc`/`objcopy` (`src/main.cpp`).

## Key entry points

- `src/main.cpp` — driver: argument parsing, import scanning, compilation, linking.
- `src/at_runtime.cpp` — C runtime (file I/O, memory primitives) linked into compiled programs.
- `src/object_importer.cpp` / `src/elf_reader.cpp` — ELF object import and `.at.sigs` signature reading.

## Standard modules

`math`, `string`, and `io` (under `std_modules/`), each with a `.at.sigs` binary signature section.

## See also

- [[concepts/CompilerArchitecture]] — detailed phase breakdown.
- [[concepts/LanguageFeatures]] — language syntax.
- [[concepts/security-audit-scope]] — security audit scope.
- [[findings]] — security findings.
