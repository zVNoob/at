---
title: FFI & Imported Object Code Execution
tags: [ffi, dlopen, dlsym, object-import, high, interpreter]
sourceRefs: [src/interpreter.cpp, src/object_importer.cpp, src/semantic.cpp, src/elf_reader.cpp, src/main.cpp]
lastReviewed: 2026-11-21
---

# FFI & Imported Object Code Execution

## Severity: High

## How imported objects gain execution

AT supports importing both `.at` source modules and ELF object files (`.o` / `.so`). Two paths lead to attacker-controlled code running in the compiler/interpreter process:

### 1. Compile-time linking (`src/main.cpp`)

Imported `.at` modules are compiled to `.o` files and linked into the final executable via `cc -no-pie ... <importedObj> ... -o <outFile>`. Object files may contain static initializers (`.init_array` / constructors) that run **before `main`**. A malicious imported module can therefore execute arbitrary code at program startup simply by being imported.

### 2. Interpret-time FFI (`src/interpreter.cpp` `loadModuleFFI`)

For `.o`/`.so` imports, the interpreter calls `dlopen(path, RTLD_NOW | RTLD_LOCAL)` then `dlsym(handle, sym->name)` for every exported function, wrapping each address in a native function (`makeNativeFFIFn`) callable from AT code:
```cpp
using FnType = int64_t (*)(int64_t, int64_t, int64_t, int64_t, int64_t);
int64_t result = fn(a0, a1, a2, a3, a4);
```
A malicious shared library exports a function that, when called (or via its initializer), runs arbitrary code in the interpreter process. `dlopen` itself can execute library constructors.

## Symbol exposure (`src/object_importer.cpp`)

`ObjectImporter::importObjectFile` reads all global symbols from the ELF object (via libelf) and inserts them into the program's symbol table. A malicious object can define symbols that collide with or shadow expected symbols, affecting name resolution and linking.

## `.at.sigs` deserialization (`src/sig_binary.cpp`)

Imported objects carry a `.at.sigs` section parsed by `BinSigReader::read`. The reader bounds-checks table sizes and indices, but performs `reinterpret_cast<const uint32_t*>(p)` / `reinterpret_cast<const ATSigType*>(p)` on potentially **unaligned** data, which can raise `SIGBUS` on strict-alignment architectures (denial of service). It otherwise appears defensive.

## Remediation

1. Treat imported objects/libraries as untrusted: run compilation/linking and `dlopen` in a sandbox (container, unprivileged user, seccomp) or disable importing of arbitrary `.o`/`.so` files.
2. Verify object file provenance (signing/checksums) before linking or dlopen.
3. Run `objcopy`/link steps with `execvp` argv arrays, not `std::system` (see [[concepts/command-injection]]).
4. Align-validate or `memcpy`-read fields in `BinSigReader::read` to avoid unaligned access crashes.
5. Limit which symbols may be imported (deny-list dangerous symbols, allow-list expected module APIs).

## Related

- [[concepts/command-injection]]
- [[concepts/path-traversal-arbitrary-file]]
- [[findings]]
