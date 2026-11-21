---
title: Path Traversal & Arbitrary File Read/Write
tags: [path-traversal, arbitrary-file-read, arbitrary-file-write, high, import]
sourceRefs: [src/main.cpp, src/interpreter.cpp]
lastReviewed: 2026-11-21
---

# Path Traversal & Arbitrary File Read/Write

## Severity: High

## Arbitrary file read via `import`

The import path from an `import '...'` statement is used directly to open/read files:

- `src/main.cpp` `scanImports`: `path` is passed to `./main ... <path>` (re-compiled) and `builder.addFile(path, ...)`.
- `src/interpreter.cpp` `loadModule`: `std::ifstream file(path)` reads the source of any imported `.at` file.
- `src/interpreter.cpp` `loadModuleFFI`: `dlopen(path)` loads any `.o`/`.so` file supplied via import.

An attacker can specify an absolute path, e.g. `import '/etc/hostname.at'` (any file ending in `.at` is readable), or traverse directories such as `import '../../etc/something.at'`. There is no confinement to a project directory or allow-list of import locations.

## Arbitrary file write via `-o` and `-B`

- The `-o` CLI argument (`src/main.cpp`) becomes the output object/executable/shared-library path and is interpolated into link commands. An attacker controlling `-o` can write to any writable filesystem location (path traversal via `../`) and, combined with command injection, achieve code execution.
- The `-B buildDir` argument is passed to `std::filesystem::create_directories(buildDir)` and used to construct output paths, allowing directory creation outside the intended tree.

## Remediation

1. Resolve and canonicalize all file/import paths; reject paths that escape an allowed base directory (block `..`, absolute paths outside the project, symlinks).
2. For imports, restrict resolution to a configured module search path (current dir + stdlib dir) and reject absolute paths.
3. Validate `-o`/`-B` values: allow only safe filename characters, reject `..` and absolute paths unless explicitly required.
4. Do not use user-supplied paths to construct shell commands — see [[concepts/command-injection]].

## Related

- [[concepts/command-injection]]
- [[findings]]
