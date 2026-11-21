---
title: Security Audit Scope & Methodology
tags: [audit, scope, methodology, destructive, user-input]
sourceRefs: [src/main.cpp, src/interpreter.cpp, src/object_importer.cpp, src/elf_reader.cpp]
lastReviewed: 2026-11-21
---

# Security Audit Scope & Methodology

## Target

Project `at` — a statically-typed language compiler implemented in C++ (LLVM 17 backend).

## Scope: truly destructive findings only

This audit intentionally excludes what is **normal for a compiler** and keeps only findings where **user-controlled input** (source code or CLI argument) causes a **silent secondary effect** — i.e., an action the user did not request that modifies state or executes code outside the normal compile/interpret pipeline.

### Kept (truly destructive)

| Surface | Destructive secondary effect |
|---------|------------------------------|
| Import path (`import '...'`) in `src/main.cpp` | Interpolated into `std::system()` shell commands → arbitrary command execution |
| `-o` / `-B` CLI argument in `src/main.cpp` | Interpolated into link commands → arbitrary command execution |
| Imported ELF object (`.o`/`.so`) | Linked into executable (constructors run before `main`) or `dlopen`d + called via FFI → arbitrary code execution in the compiler/interpreter process |
| Import path / `-o` path traversal | Reads or writes files outside the intended directory (`..`, absolute paths) |

### Excluded (normal compiler behavior — not destructive secondary effects)

- ELF/object parsing via libelf (`src/elf_reader.cpp`) — expected for an object importer.
- Memory allocator / file I/O runtime (`src/at_runtime.cpp`) — expected runtime primitives.
- PIE / hardening flags (`-no-pie`) — build hardening, not a user-input secondary effect.
- PATH resolution of `cc`/`objcopy` — toolchain invocation, not destructive per se.
- Interpreter exposing `at_fopen`/`at_fread`/etc. — expected language runtime I/O.
- Unaligned reads in `.at.sigs` reader (`src/sig_binary.cpp`) — reliability/DoS concern, not destructive state change.

## Severity scale used

- **Critical** — Arbitrary code execution from untrusted input.
- **High** — Silent code execution or arbitrary file read/write outside intended scope.

## See also

- [[findings]] — consolidated destructive findings.
