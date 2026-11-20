# Build the io std module as a static .o or shared .so.
CXX ?= g++
CXXFLAGS ?= -O2 -std=c++20
CFLAGS ?= -O2 -fPIC -shared
LD ?= ld

all: io.o

io.o: io.cpp io.h io_sigs.cpp
	$(CXX) $(CXXFLAGS) -c -o io_tmp.o io.cpp
	$(CXX) $(CXXFLAGS) -c -o io_sigs_tmp.o io_sigs.cpp
	$(LD) -r io_tmp.o io_sigs_tmp.o -o io.o
	rm -f io_tmp.o io_sigs_tmp.o

shared: libio.so

libio.so: io.cpp io.h io_sigs.cpp
	$(CXX) $(CFLAGS) -std=c++20 -o libio.so io.cpp io_sigs.cpp

clean:
	rm -f io.o libio.so io_tmp.o io_sigs_tmp.o io_sigs.bin

.PHONY: all shared clean
