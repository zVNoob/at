#include "io.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

static void io_destroy(at_io_stream* s) {
    if (!s) return;
    if (s->file && s->file != stdin && s->file != stdout && s->file != stderr) {
        fclose(s->file);
    }
    free(s);
}

static const char* io_name(at_io_stream* s) { return s ? s->name : ""; }

static size_t io_read(at_io_stream* s, char* buf, size_t n) {
    if (!s || !s->file) return 0;
    return fread(buf, 1, n, s->file);
}

static size_t io_write(at_io_stream* s, const void* buf, size_t n) {
    if (!s || !s->file) return 0;
    return fwrite(buf, 1, n, s->file);
}

static int io_close(at_io_stream* s) {
    if (!s) return 0;
    if (s->file && s->file != stdin && s->file != stdout && s->file != stderr) {
        fclose(s->file);
        s->file = nullptr;
    }
    return 0;
}

static int io_flush(at_io_stream* s) {
    if (!s || !s->file) return 0;
    return fflush(s->file);
}

static const at_io_stream_vtable IO_VTABLE = {
    io_destroy, io_name, io_read, io_write, io_close, io_flush
};

const at_io_stream_vtable* at_io_stream_vtable_ptr() { return &IO_VTABLE; }

at_io_stream* at_io_open(const char* path, int mode) {
    const char* m = "r";
    switch (mode) {
        case 0: m = "r"; break;
        case 1: m = "w"; break;
        case 2: m = "a"; break;
        case 3: m = "r+"; break;
    }
    FILE* f = fopen(path, m);
    if (!f) return nullptr;
    at_io_stream* s = (at_io_stream*)malloc(sizeof(at_io_stream));
    if (!s) { fclose(f); return nullptr; }
    s->vtable = &IO_VTABLE;
    s->file = f;
    snprintf(s->name, sizeof(s->name), "%s", path ? path : "");
    return s;
}

static at_io_stream* wrap_std(FILE* f, const char* name) {
    at_io_stream* s = (at_io_stream*)malloc(sizeof(at_io_stream));
    if (!s) return nullptr;
    s->vtable = &IO_VTABLE;
    s->file = f;
    snprintf(s->name, sizeof(s->name), "%s", name);
    return s;
}

at_io_stream* at_io_stdin_stream() { return wrap_std(stdin, "stdin"); }
at_io_stream* at_io_stdout_stream() { return wrap_std(stdout, "stdout"); }
at_io_stream* at_io_stderr_stream() { return wrap_std(stderr, "stderr"); }


