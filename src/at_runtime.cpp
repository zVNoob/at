#include "at_runtime.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <unistd.h>

// Custom section for function signatures (used by compiler for type info)
// Format: one line per function: "name param_types... -> return_type"
// Types: int, decimal, string, bool, ptr, void
__attribute__((section(".at.sigs"), used))
static const char at_sigs[] =
    "at_fopen str int -> ptr\n"
    "at_fclose ptr -> int\n"
    "at_fread ptr int -> str\n"
    "at_fwrite ptr str -> int\n"
    "at_feof ptr -> int\n"
    "at_fflush ptr -> int\n"
    "at_ftell ptr -> int\n"
    "at_fseek ptr int int -> int\n"
    "at_fgets ptr int -> str\n"
    "at_fputs str ptr -> int\n"
    "at_fputc int ptr -> int\n"
    "at_fgetc ptr -> int\n"
    "at_stdin -> ptr\n"
    "at_stdout -> ptr\n"
    "at_stderr -> ptr\n"
    "at_strlen str -> int\n"
    "at_strdup str -> str\n"
    "at_malloc int -> ptr\n"
    "at_free ptr -> void\n";

extern "C" {

FILE* at_fopen(const char* path, int mode) {
    const char* modes[] = {"r", "w", "a", "r+"};
    if (mode < 0 || mode > 3) mode = 0;
    return fopen(path, modes[mode]);
}

int at_fclose(FILE* f) {
    if (!f) return -1;
    return fclose(f);
}

size_t at_fread4(void* buf, size_t size, size_t count, FILE* f) {
    if (!f || !buf) return 0;
    return fread(buf, size, count, f);
}

size_t at_fwrite4(const void* buf, size_t size, size_t count, FILE* f) {
    if (!f || !buf) return 0;
    return fwrite(buf, size, count, f);
}

// 2-arg versions for AT codegen: at_fread(handle, size) -> str, at_fwrite(handle, str) -> int
extern "C" char* at_fread(int handle, int size) {
    FILE* f = (FILE*)(intptr_t)handle;
    if (!f || size <= 0) return nullptr;
    char* buf = (char*)malloc(size + 1);
    if (!buf) return nullptr;
    size_t n = fread(buf, 1, size, f);
    buf[n] = '\0';
    return buf;
}

extern "C" int at_fwrite(int handle, const char* s) {
    if (!s) return 0;
    size_t len = strlen(s);
    return write(1, s, len);
}

int at_feof(FILE* f) {
    if (!f) return 1;
    return feof(f);
}

int at_fflush(FILE* f) {
    if (!f) return -1;
    return fflush(f);
}

long at_ftell(FILE* f) {
    if (!f) return -1;
    return ftell(f);
}

int at_fseek(FILE* f, long offset, int whence) {
    if (!f) return -1;
    return fseek(f, offset, whence);
}

char* at_fgets(char* buf, int size, FILE* f) {
    if (!f || !buf || size <= 0) return nullptr;
    return fgets(buf, size, f);
}

int at_fputs(const char* s, FILE* f) {
    if (!f || !s) return -1;
    return fputs(s, f);
}

int at_fputc(int c, FILE* f) {
    if (!f) return -1;
    return fputc(c, f);
}

int at_fgetc(FILE* f) {
    if (!f) return -1;
    return fgetc(f);
}

FILE* at_stdin() { return stdin; }
FILE* at_stdout() { return stdout; }
FILE* at_stderr() { return stderr; }

size_t at_strlen(const char* s) {
    if (!s) return 0;
    return strlen(s);
}

char* at_strdup(const char* s) {
    if (!s) return nullptr;
    size_t len = strlen(s) + 1;
    char* dup = (char*)malloc(len);
    if (dup) memcpy(dup, s, len);
    return dup;
}

void* at_malloc(size_t size) {
    return malloc(size);
}

void at_free(void* ptr) {
    free(ptr);
}

} // extern "C"
