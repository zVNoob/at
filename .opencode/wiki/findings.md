---
title: Consolidated Security Findings (Destructive Only)
tags: [findings, summary, destructive, user-input]
sourceRefs: [src/main.cpp, src/interpreter.cpp, src/object_importer.cpp, src/elf_reader.cpp]
lastReviewed: 2026-11-21
---

# Consolidated Security Findings (Destructive Only)

Only findings where **user-controlled input** causes a **silent secondary effect**. Normal compiler behavior (ELF parsing, runtime I/O, hardening flags, PATH tool resolution) is excluded.

## Summary table

| ID | Severity | Title | Location | Silent secondary effect |
|----|----------|-------|----------|-------------------------|
| F-01 | Critical | Command injection via `std::system()` — import path | `src/main.cpp:124,127,135,140,143` | Arbitrary command execution as the compiler user |
| F-02 | Critical | Command injection via `std::system()` — `-o` output | `src/main.cpp:204,221` | Arbitrary command execution as the compiler user |
| F-03 | High | Arbitrary code execution via imported ELF object | `src/main.cpp`, `src/interpreter.cpp` | Imported `.o`/`.so` constructors run before `main`; `dlopen`+FFI executes code in-process |
| F-04 | High | Path traversal via import path / `-o` / `-B` | `src/main.cpp`, `src/interpreter.cpp` | Reads or writes files outside intended directory (`..`, absolute paths) |

## Detail & remediation

### F-01 — Command injection: import path

`scanImports()` (runs unconditionally, including interpreter mode) builds `./main --emit-obj --no-main -o <obj> <path>` and `objcopy ... <obj>` from the path in `import '...'`, then calls `std::system()`. Shell metacharacters (`;`, `|`, `&`, backticks, `$`, quotes) in the import path execute arbitrary commands.

**Remediation:** Replace `std::system()` with `execvp`-style `argv` arrays (no shell). Validate import paths: allow only safe filename characters, reject shell metacharacters, reject `..` and absolute paths; confine to an allowed module search directory.

### F-02 — Command injection: `-o` / `-B`

The `-o` CLI argument (and `-B` build dir) is interpolated into `cc` link commands and `create_directories`. A value like `/tmp/evil; rm -rf / #.o` executes arbitrary commands.

**Remediation:** Same as F-01 — `execvp` argv arrays, validate/sanitize `-o`/`-B` values, reject `..` and shell metacharacters.

### F-03 — Arbitrary code execution via imported ELF object

Imported `.at` modules are compiled to `.o` and linked into the final executable (`cc -no-pie ... <importedObj> ... -o <outFile>`). Object files may contain static initializers (`.init_array`) that run **before `main`**. In interpreter mode, `loadModuleFFI` calls `dlopen(path)` then `dlsym`/FFI-wraps every exported function — a malicious library executes code at load time or on first call.

**Remediation:** Treat imported objects as untrusted. Run linking/`dlopen` in a sandbox (container, unprivileged user, seccomp). Verify object provenance (signing/checksums) before importing. Use `execvp` (not `std::system`) for tool invocations. Limit which symbols may be imported.

### F-04 — Path traversal via import path / `-o` / `-B`

Import paths and `-o`/`-B` values are used to open/read/write files with no confinement. `import '../../etc/secret.at'` reads arbitrary files; `-o ../../evil.o` writes outside the intended tree.

**Remediation:** Canonicalize all file/import paths; reject paths that escape an allowed base directory (block `..`, absolute paths outside project, symlinks). Restrict imports to a configured module search path. Validate `-o`/`-B` filenames.

## Immediate priorities

1. Eliminate all `std::system()` calls with shell-interpolated user input (F-01, F-02) — **critical**, exploitable for remote code execution from a single malicious source file.
2. Sandbox compilation/linking and importing of objects (F-03).
3. Confine import and `-o`/`-B` paths to an allowed directory (F-04).

## Excluded as normal compiler behavior (not in scope)

- ELF/object parsing via libelf — expected object-importer function.
- Runtime file I/O primitives (`at_fopen`, `at_fread`, …) — expected language runtime.
- Build hardening (`-no-pie`, PATH tool resolution) — not a destructive user-input secondary effect.
- `.at.sigs` reader reliability — DoS concern, not destructive state change.

## Related

- [[concepts/security-audit-scope]] — methodology and attack surface.
