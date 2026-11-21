---
title: Wiki Index
tags: [catalog]
sourceRefs: []
lastReviewed: 2026-11-21
---

# Wiki Index — Project `at`

## Project overview

- [[overview]] — Project overview: a C++ compiler ("at") using LLVM 17, with lexer/parser, semantic analysis, LLVM codegen, ELF object importing, and a runtime. *(to be created)*
- [[concepts/security-audit-scope]] — Scope and methodology of this security audit.
- [[findings]] — Consolidated list of destructive security findings (user input with silent secondary effects only).

## Concepts

- [[concepts/command-injection]] — Command injection via `std::system()` in `src/main.cpp` (Critical).
- [[concepts/path-traversal-arbitrary-file]] — Path traversal and arbitrary file read/write via import paths and `-o`/`-B` (High).
- [[concepts/ffi-object-import]] — Arbitrary code execution via linked/dlopened imported ELF objects and FFI (High).
- [[concepts/memory-safety]] — Memory safety and input parsing (at_fread overflow, sig_binary unaligned reads). *Out of scope for destructive audit — normal compiler/runtime behavior.*
- [[concepts/runtime-sandbox]] — Missing sandboxing, `-no-pie`, PATH reliance, unrestricted interpreter file I/O. *Out of scope for destructive audit — hardening, not a user-input secondary effect.*
- [[concepts/CompilerArchitecture]] — Multi-pass compiler architecture (lexer, parser, semantic, codegen, interpreter).
- [[concepts/LanguageFeatures]] — AT language syntax and features.
- [[concepts/StdModuleSigs]] — `.at.sigs` binary signature format and `at_sigs.h` builders.

## Entities (components)

- [[entities/AT-language]] — AT programming language overview.
- [[entities/AST]] — Abstract Syntax Tree structure.
- [[entities/Parser]] — Parser (Bison/Yacc grammar `src/syntax.ast.y`).
- [[entities/SemanticAnalyzer]] — Semantic analysis / type checking.
- [[entities/CodeGen]] — LLVM-based code generation.
- [[entities/Interpreter]] — In-memory interpreter and module/FFI loading.
- [[entities/StdModules]] — math, string, io standard modules.

## Sources

- [[sources/AT-language-spec]] — AT language specification (derived from `src/syntax.ast.y`).

## Security audit (new)

- [[concepts/security-audit-scope]]
- [[concepts/command-injection]]
- [[concepts/path-traversal-arbitrary-file]]
- [[concepts/ffi-object-import]]
- [[concepts/memory-safety]]
- [[concepts/runtime-sandbox]]
- [[findings]]
