---
title: Memory Safety & Input Parsing
tags: [memory-safety, integer-overflow, parsing, elf, medium]
sourceRefs: [src/at_runtime.cpp, src/sig_binary.cpp, src/lexer.cpp, src/elf_reader.cpp]
lastReviewed: 2026-11-21
---

# Memory Safety & Input Parsing

## Severity: Medium (with Low sub-items)

## Reviewed areas

### `src/at_runtime.cpp` — runtime primitives exposed to AT programs

- `at_malloc(size)` / `at_free(ptr)` — thin wrappers around `malloc`/`free`. No overflow guards beyond the standard allocator.
- `at_fread(int handle, int size)` — `char* buf = (char*)malloc(size + 1); fread(buf, 1, size, f);`. If `size == INT_MAX`, `size + 1` overflows (wraps), producing a tiny/zero allocation while `fread` attempts to write `INT_MAX` bytes → **heap buffer overflow**. `size` is attacker-controllable from AT source.
- `at_strdup(s)` — `malloc(len)` where `len = strlen(s) + 1`; safe for normal inputs, no explicit cap on extremely long strings (memory exhaustion).
- File I/O functions (`at_fopen`, `at_fread`, `at_fwrite`, …) let AT programs read/write arbitrary files — intended functionality, but no sandbox (see [[concepts/runtime-sandbox]]).

### `src/sig_binary.cpp` — `.at.sigs` binary deserialization

`BinSigReader::read` parses the `.at.sigs` section from imported ELF objects (attacker-controlled). It bounds-checks `num_types`/`num_decls` against `data.size()` and validates index lookups (`< btypes.size()`, `< ATSG_MAX_PARAMS`, etc.), so out-of-bounds reads are largely avoided. However, it uses `reinterpret_cast<const uint32_t*>(p)` and struct-pointer casts on data that may be **unaligned**, which can trigger `SIGBUS` on strict-alignment architectures → denial of service via a malformed `.at.sigs` section.

### `src/lexer.cpp` — source lexing

Number/string/identifier parsing operates on `std::string` with bounds-checked access; no fixed-size buffers. No obvious memory-safety defects found. String escape handling (`\\`) is simple but does not decode escapes into a separate buffer, reducing risk.

### `src/elf_reader.cpp` — ELF reading via libelf

Uses `libelf` (`elf_begin`, `gelf_getsym`, `elf_strptr`, …), which performs its own bounds checking on ELF structures. Generally defensive against malformed ELF; the main risk is resource exhaustion from a very large `.at.sigs` or symbol table (bounded by file size).

## Remediation

1. Add an upper bound to `at_fread`'s `size` parameter and use `size_t` arithmetic that checks for overflow before `malloc(size + 1)`.
2. Cap allocation sizes in `at_strdup` and other runtime allocation entry points to resist memory-exhaustion via AT source.
3. Replace struct-pointer `reinterpret_cast` reads in `BinSigReader::read` with `memcpy` into aligned locals (or require 4-byte alignment) to avoid `SIGBUS` on strict-alignment platforms.
4. Consider a max file-size / section-size limit when reading imported ELF objects.
5. Compile the compiler itself with hardening flags: `-D_FORTIFY_SOURCE=2`, `-fstack-protector-strong`, `-pie`, `-Wl,-z,relro,-z,now` (note: driver currently passes `-no-pie`, weakening the produced executables — see [[concepts/runtime-sandbox]]).

## Related

- [[concepts/runtime-sandbox]]
- [[findings]]
