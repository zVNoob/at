# Wiki Log

## 2026-10-25 ingest | AT Language Project Analysis

Initial analysis of the AT language implementation project. Created comprehensive wiki documentation covering:

- Project overview and architecture
- Individual component documentation (AST, Parser, Semantic Analyzer, Interpreter, Code Generator)
- Concept pages (Compiler Architecture, Language Features)
- Source specifications (Language Grammar)

This provides a complete knowledge base for understanding this self-contained compiler/interpreter system.

## 2026-11-19 ingest | Std Modules already use at_sigs.h

Verified that std_modules/math/math_sigs.cpp and std_modules/string/string_sigs.cpp already use the struct-based at_sigs.h constexpr builder approach (identical pattern to std_modules/io/io_sigs.cpp). Both modules build successfully. All 12 tests pass via `make test`. Created:

- [[entities/StdModules]] — documentation for math, string, and io std modules
- [[concepts/StdModuleSigs]] — .at.sigs binary format v2 and at_sigs.h builder documentation
- Updated [[index.md]] with new entity and concept pages

## 2026-11-19 ingest | Import module linking fixed

Fixed `io_test.at` compilation where `main` was `ret 0` and no output was produced. Root causes and fixes:

1. **Missing import compilation**: `main.cpp` didn't compile/import `.at` files. Added `scanImports()` to find `import 'file.at'` statements, compile them to `.o` in `/tmp/`, and link them with the main `.o`.

2. **Multiple `main` definition**: Both main module and imported modules defined `main`. Added `--no-main` flag to skip `main` generation in imported modules. Imported modules' top-level code goes into `module_init` function.

3. **Multiple `module_init` definition**: Both modules defined `module_init`. Renamed imported module's `module_init` to `<modname>_module_init` via `objcopy --redefine-sym`. Main module's `main` explicitly calls each `<modname>_module_init()` at startup.

4. **`at_fwrite` compiler bug**: `fwrite(s, 1, strlen(s), f)` compiled to `fwrite(strlen(s), 1, strlen(s), f)` because `strlen` clobbers `rdi`. Fixed by computing `strlen(s)` into a local variable first.

5. **`at_fwrite` used buffered I/O**: Changed to `write(1, s, len)` for direct unbuffered output to fd 1 (stdout).

## 2026-11-19 ingest | String escape sequence processing

String literals with escape sequences (`\n`, `\t`, `\r`, `\\`, `\"`, `\'`) were not being interpreted — they appeared as literal backslash sequences in output.

Root cause: The lexer stores escape sequences literally in string literals (e.g., `\n` stays as the two characters `\` and `n`), but neither the code generator nor the interpreter converted them to actual characters.

Fix: Added escape sequence processing to both:
- `src/codegen_expr.cpp` — STRING handler in `codegenExpr()`
- `src/interpreter.cpp` — STRING handler in `eval()`

Both now convert `\n` → newline, `\t` → tab, `\r` → carriage return, `\\` → backslash, `\"` → double quote, `\'` → single quote.

Files changed:
- `src/main.cpp` — Added import scanning, `--no-main` flag, init function calling
- `src/codegen_main.cpp` — Added `isMainModule` and `initFuncs` params to `compile()`
- `src/codegen_stmt.cpp` — Fixed member assignment handling, global variable creation
- `src/codegen_expr.cpp` — Fixed dot member access call detection, argument extraction, **added escape sequence processing**
- `src/codegen_template.cpp` — Removed debug output
- `src/at_runtime.cpp` — Fixed `at_fwrite` to use `write()` directly
- `src/at_runtime.h` — Updated declarations for `at_fread4`/`at_fwrite4`
- `src/interpreter.cpp` — Updated to use `at_fread4`/`at_fwrite4`, **added escape sequence processing**

All 11 tests pass via `make test`.