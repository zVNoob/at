all:
	bison -d parser.y
	g++ -o main parser.tab.cc -g -lgmp
