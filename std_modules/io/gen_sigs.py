#!/usr/bin/env python3
"""Generate .at.sigs binary section for the io std module.

Output: io_sigs.bin - a raw binary blob to be added to io.o via
  objcopy --add-section .at.sigs=io_sigs.bin io.o
"""
import struct
import sys

ATSG_MAGIC = 0x47535441
ATSG_VERSION = 2

ATSG_T_PRIM = 0
ATSG_P_INT = 0
ATSG_P_STRING = 2
ATSG_P_VOID = 3
ATSG_KIND_FN = 0


def prim_type(kind):
    """ATSigType: 32 bytes packed"""
    return struct.pack('<BBHBH', ATSG_T_PRIM, kind, 0, 0, 0) + \
           b'\x00' * 32 + b'\x00\x00' + b'\x00' + b'\x00\x00\x00'


def fn_decl(name_off, ret_idx, param_type_indices):
    """ATSigDecl: 128 bytes packed"""
    nparams = len(param_type_indices)
    buf = bytearray(128)
    buf[0] = ATSG_KIND_FN
    struct.pack_into('<H', buf, 1, name_off)
    buf[3] = 0  # tmpl_param_count
    buf[12] = 0  # is_meta
    buf[13] = nparams  # param_count
    for i, tidx in enumerate(param_type_indices):
        struct.pack_into('<H', buf, 46 + i * 2, tidx)
    struct.pack_into('<H', buf, 78, ret_idx)  # ret_type_idx
    return bytes(buf)


# String table
strings = [
    b"at_fopen", b"at_fclose", b"at_fread", b"at_fwrite",
    b"at_feof", b"at_fflush", b"at_ftell", b"at_fseek",
    b"at_fgets", b"at_fputs", b"at_fputc", b"at_fgetc",
    b"at_stdin", b"at_stdout", b"at_stderr",
    b"at_io_stream_vtable_ptr",
    b"int", b"string", b"void",
]
string_data = b''.join(s + b'\x00' for s in strings)
string_offsets = {}
off = 0
for s in strings:
    string_offsets[s] = off
    off += len(s) + 1

# Type table: idx 0 = int, idx 1 = string, idx 2 = void
types = [prim_type(ATSG_P_INT), prim_type(ATSG_P_STRING), prim_type(ATSG_P_VOID)]
TYP_INT = 0
TYP_STRING = 1
TYP_VOID = 2

# Declarations
decls = [
    # at_fopen(string, string) -> int
    fn_decl(string_offsets[b"at_fopen"], TYP_INT, [TYP_STRING, TYP_STRING]),
    # at_fclose(int) -> int
    fn_decl(string_offsets[b"at_fclose"], TYP_INT, [TYP_INT]),
    # at_fread(int, int) -> string
    fn_decl(string_offsets[b"at_fread"], TYP_STRING, [TYP_INT, TYP_INT]),
    # at_fwrite(int, string) -> int
    fn_decl(string_offsets[b"at_fwrite"], TYP_INT, [TYP_INT, TYP_STRING]),
    # at_feof(int) -> int
    fn_decl(string_offsets[b"at_feof"], TYP_INT, [TYP_INT]),
    # at_fflush(int) -> int
    fn_decl(string_offsets[b"at_fflush"], TYP_INT, [TYP_INT]),
    # at_ftell(int) -> int
    fn_decl(string_offsets[b"at_ftell"], TYP_INT, [TYP_INT]),
    # at_fseek(int, int, int) -> int
    fn_decl(string_offsets[b"at_fseek"], TYP_INT, [TYP_INT, TYP_INT, TYP_INT]),
    # at_fgets(int, int) -> string
    fn_decl(string_offsets[b"at_fgets"], TYP_STRING, [TYP_INT, TYP_INT]),
    # at_fputs(string, int) -> int
    fn_decl(string_offsets[b"at_fputs"], TYP_INT, [TYP_STRING, TYP_INT]),
    # at_fputc(int, int) -> int
    fn_decl(string_offsets[b"at_fputc"], TYP_INT, [TYP_INT, TYP_INT]),
    # at_fgetc(int) -> int
    fn_decl(string_offsets[b"at_fgetc"], TYP_INT, [TYP_INT]),
    # at_stdin() -> int
    fn_decl(string_offsets[b"at_stdin"], TYP_INT, []),
    # at_stdout() -> int
    fn_decl(string_offsets[b"at_stdout"], TYP_INT, []),
    # at_stderr() -> int
    fn_decl(string_offsets[b"at_stderr"], TYP_INT, []),
    # at_io_stream_vtable_ptr() -> int
    fn_decl(string_offsets[b"at_io_stream_vtable_ptr"], TYP_INT, []),
]

# Build section
buf = bytearray()
buf += struct.pack('<IIII', ATSG_MAGIC, ATSG_VERSION, len(types), len(decls))
for t in types:
    buf += t
for d in decls:
    buf += d
buf += string_data

output = sys.argv[1] if len(sys.argv) > 1 else 'io_sigs.bin'
with open(output, 'wb') as f:
    f.write(buf)
print(f"Wrote {len(buf)} bytes to {output}")
print(f"  Header: 16 bytes")
print(f"  Types: {len(types)} * 32 = {len(types)*32} bytes")
print(f"  Decls: {len(decls)} * 128 = {len(decls)*128} bytes")
print(f"  Strings: {len(string_data)} bytes")
