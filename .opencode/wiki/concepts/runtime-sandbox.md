---
title: Runtime Sandboxing & Hardening Gaps
tags: [sandbox, hardening, pie, no-pie, runtime, low]
sourceRefs: [src/main.cpp, src/interpreter.cpp, src/at_runtime.cpp, Makefile]
lastReviewed: 2026-11-21
---

# Runtime Sandboxing & Hardening Gaps

## Severity: Low / Hardening

## Findings

### No sandboxing

The compiler and interpreter run with the full privileges of the invoking user. There is no seccomp filter, container, namespace isolation, or unprivileged-user execution. Combined with command injection [[concepts/command-injection]] and FFI/dlopen [[concepts/ffi-object-import]], a successful exploit runs arbitrary code as the user.

### `-no-pie` disables position-independent executables

`src/main.cpp` links executables with `cc -no-pie ...`. PIE (Position-Independent Executable) is disabled, making produced binaries easier to exploit (fixed code addresses). The Makefile's release build uses `-O2 -flto=auto` but no `-pie`/`-Wl,-z,relro,-z,now` hardening flags.

### Reliance on external tools via PATH

The driver invokes `cc`, `objcopy`, and `./main` by name. A modified `PATH` can cause execution of attacker-controlled `cc`/`objcopy` binaries (DLL/search-order hijacking equivalent). `./main` is invoked by relative path, reducing but not eliminating this risk.

### Interpreter exposes file I/O to AT programs

`at_runtime.cpp` exposes `at_fopen`, `at_fread`, `at_fwrite`, `at_fgets`, `at_fputs`, etc. as native functions bound in `src/interpreter.cpp` (`Interpreter::Interpreter`). Interpreted AT programs can read/write arbitrary files accessible to the process. There is no capability/sandbox model restricting which paths an interpreted program may touch.

### `src/at_runtime.cpp` links runtime source directly

The link command appends `src/at_runtime.cpp` as a source file (`cc ... src/at_runtime.cpp -o ...`), compiling it on each link. Functionally fine, but unusual and means runtime source must be present at link time.

## Remediation

1. Add sandboxing: run compiler/interpreter as an unprivileged user, ideally inside a container or with seccomp-bpf limiting syscalls (no `execve`/`system` from compiled output unless required).
2. Restore PIE and full RELRO: link executables without `-no-pie`; add `-Wl,-z,relro,-z,now` and `-fstack-protector-strong`.
3. Avoid `PATH`-dependent tool resolution: use absolute/known paths for `cc`/`objcopy`, or validate the resolved executable.
4. Add a file-access policy/sandbox for interpreted programs (allowed directory roots for `at_fopen`).
5. Pin the runtime source location or build `at_runtime.o` once and link the object rather than recompiling source each time.

## Related

- [[concepts/command-injection]]
- [[concepts/ffi-object-import]]
- [[concepts/memory-safety]]
- [[findings]]
