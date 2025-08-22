all: bison compile

bison:
	bison parser.y -o parser.cpp -Wcounterexample

compile:
	g++ *.cpp objects/*.cpp -o main -lgmp -g
