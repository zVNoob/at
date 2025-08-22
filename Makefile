all: bison compile

bison:
	bison parser.y -o parser.cpp -Wcounterexample

compile:
	g++ *.cpp objects/*.cpp -o main -std=c++20 -lgmp -g
