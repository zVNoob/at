# Build the string std module as a static .o or shared .so.
CXX ?= g++
CXXFLAGS ?= -O2 -std=c++20
CFLAGS ?= -O2 -fPIC -shared
LD ?= ld

all: string.o

string.o: string.cpp string.h string_sigs.cpp
	$(CXX) $(CXXFLAGS) -c -o string_tmp.o string.cpp
	$(CXX) $(CXXFLAGS) -c -o string_sigs_tmp.o string_sigs.cpp
	$(LD) -r string_tmp.o string_sigs_tmp.o -o string.o
	rm -f string_tmp.o string_sigs_tmp.o

shared: libstring.so

libstring.so: string.cpp string.h string_sigs.cpp
	$(CXX) $(CFLAGS) -std=c++20 -o libstring.so string.cpp string_sigs.cpp

clean:
	rm -f string.o libstring.so string_tmp.o string_sigs_tmp.o

.PHONY: all shared clean
