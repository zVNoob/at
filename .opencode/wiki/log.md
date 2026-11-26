---
title: Wiki Log
tags: [log]
sourceRefs: []
lastReviewed: 2026-11-21
---

# Wiki Log

## [2026-11-21] seed | Initial wiki seed for security audit of project `at`
## [2026-11-21] ingest | Security audit: added command-injection, path-traversal, ffi-object-import, memory-safety, runtime-sandbox concept pages + findings.md; updated index.md and log.md
## [2026-11-21] refine | Narrowed audit to truly destructive findings only (user input with silent secondary effects); excluded normal compiler behavior; recorded gc-sections/.at.sigs quirk
## [2026-11-22] ingest | Ported .at.sigs to version 3 variable-length TLV format; fixed import compilation to use in-process CodeGen instead of subprocess
## [2026-11-25] ingest | Fixed three remaining issues: (1) added C/C++ comment support (// line, /* */ block) in lexer.cpp, (2) fixed interpreter if/else branching in interpreter.cpp, (3) fixed .at.sigs reader/writer type deduction and class method resolution in sig_binary.cpp/h; all 12 tests pass via make test
## [2026-11-26] ingest | Unified compiler and interpreter via LLVM ORC JIT: created CodeGenLower shared lowering class (codegen_lower.h/cpp), rewrote CodeGen to delegate to CodeGenLower, rewrote Interpreter to use ORC JIT (LLJIT) for execution with tree-walking fallback for imports, added native function dispatch via at_native_dispatch trampoline, all 12 tests pass via make test
