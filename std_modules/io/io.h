#pragma once

#include <cstdio>
#include <cstdint>
#include <cstddef>

#include "../abi.h"

// I/O module for the `at` language.
// Exports an opaque-handle + method-table ABI so `at` can treat streams as classes.

extern "C" {

typedef struct at_io_stream at_io_stream;

// Method table (vtable) for a stream. Function pointers take the handle as first arg.
typedef struct {
    void (*destroy)(at_io_stream* self);
    const char* (*name)(at_io_stream* self);
    size_t (*read)(at_io_stream* self, char* buf, size_t n);
    size_t (*write)(at_io_stream* self, const void* buf, size_t n);
    int (*close)(at_io_stream* self);
    int (*flush)(at_io_stream* self);
} at_io_stream_vtable;

// Opaque handle.
struct at_io_stream {
    const at_io_stream_vtable* vtable;
    FILE* file;
    char name[64];
};

// Factory.
at_io_stream* at_io_open(const char* path, int mode);

// Vtable accessor (single shared instance).
const at_io_stream_vtable* at_io_stream_vtable_ptr();

// Standard streams.
at_io_stream* at_io_stdin_stream();
at_io_stream* at_io_stdout_stream();
at_io_stream* at_io_stderr_stream();

} // extern "C"
