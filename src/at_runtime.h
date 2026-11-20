#pragma once

#include <cstdio>
#include <cstddef>

extern "C" {

// File open modes: 0=read, 1=write, 2=append, 3=read+write
FILE* at_fopen(const char* path, int mode);
int at_fclose(FILE* f);
size_t at_fread4(void* buf, size_t size, size_t count, FILE* f);
size_t at_fwrite4(const void* buf, size_t size, size_t count, FILE* f);

// 2-arg versions for AT codegen
char* at_fread(int handle, int size);
int at_fwrite(int handle, const char* s);
int at_feof(FILE* f);
int at_fflush(FILE* f);
long at_ftell(FILE* f);
int at_fseek(FILE* f, long offset, int whence);
char* at_fgets(char* buf, int size, FILE* f);
int at_fputs(const char* s, FILE* f);
int at_fputc(int c, FILE* f);
int at_fgetc(FILE* f);

// Standard streams
FILE* at_stdin();
FILE* at_stdout();
FILE* at_stderr();

// String helpers
size_t at_strlen(const char* s);
char* at_strdup(const char* s);

// Memory
void* at_malloc(size_t size);
void at_free(void* ptr);

} // extern "C"
