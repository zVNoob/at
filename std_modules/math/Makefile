# Build the math std module as a static .o or shared .so.
CXX ?= g++
CXXFLAGS ?= -O2 -std=c++20
CFLAGS ?= -O2 -fPIC -shared
LD ?= ld

all: math.o

math.o: math.cpp math.h math_sigs.cpp
	$(CXX) $(CXXFLAGS) -c -o math_tmp.o math.cpp
	$(CXX) $(CXXFLAGS) -c -o math_sigs_tmp.o math_sigs.cpp
	$(LD) -r math_tmp.o math_sigs_tmp.o -o math.o
	rm -f math_tmp.o math_sigs_tmp.o

shared: libmath.so

libmath.so: math.cpp math.h math_sigs.cpp
	$(CXX) $(CFLAGS) -std=c++20 -o libmath.so math.cpp math_sigs.cpp

clean:
	rm -f math.o libmath.so math_tmp.o math_sigs_tmp.o math_sigs.bin

.PHONY: all shared clean
