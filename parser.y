%require "3.2"
%language "c++"

%locations
%define parse.error verbose
%define api.value.type variant
%define parse.trace
%parse-param {Lexer& lexer}


%code requires {
#include <variant>
using namespace std;
#define yylex lexer.lex 
class Lexer;
}

%{
#include "misc/bigint.hpp"
#include "misc/bigfraction.hpp"
#include "lexer.hpp"
%}


%token <std::variant<BigInt, BigFraction, std::string>> CONSTANT 

%token <std::string> IDENTIFIER


%left '+' '-'
%left '*' '/'
%right POWER

%start input



%%


input: %empty
	 | expr ';' input
	 ;

expr: exp_eval
	;

exp_eval: CONSTANT

		 ;
%%
#include "lexer.hpp"
namespace yy {
	void parser::error(const location_type& location, const std::string& message) {
		std::cerr << location << ": ";
		std::string col = "200;200;200";
		// part 2: coloring the error message
		{
		int colon_loc = 0;
		for (;colon_loc < message.size(); ++colon_loc) {
			if (message[colon_loc] == ':') {
				break;
			}
			if (message[colon_loc] == ',') {
				break;
			}
		}
		if (colon_loc == message.size()) {
			std::cerr << message << std::endl;
		}
		else {
			switch (message[0]) {
				case 's': // syntax error
				case 't': // type mismatch
					col = "250;50;50";
					break;
				case 'w': // warning
					col = "250;250;50";
					break;
			}
			std::cerr << "\033[38;2;" << col << "m" << message.substr(0, colon_loc) 
					  << "\033[0m" << message.substr(colon_loc + 1) << std::endl;
		}
		}
		// part 3: print out location and source code
		{
		int line = location.begin.line;
		std::cerr << "\033[38;2;200;200;200m" << lexer.buffer[line - 1].substr(0, location.begin.column-1);
		if (location.end.line == location.begin.line) {
			std::cerr << "\033[38;2;" << col << "m" << lexer.buffer[line - 1].substr(location.begin.column-1, location.end.column - location.begin.column);
			std::cerr << "\033[0m" << lexer.buffer[line - 1].substr(location.end.column-1);
		}
		else {
			std::cerr << "\033[38;2;" << col << "m" << lexer.buffer[line - 1].substr(location.begin.column-1) << std::endl;
			for (int i = line+1; i < location.end.line; ++i) {
				std::cerr << lexer.buffer[i - 1] << std::endl;
			}
			std::cerr << lexer.buffer[location.end.line - 1].substr(0, location.end.column-1);
			std::cerr << "\033[38;2;200;200;200m" << lexer.buffer[location.end.line - 1].substr(location.end.column-1);
		}
		std::cerr << "\033[0m" << std::endl;
		}

	}
}
int main() {
	Lexer lexer(std::cin);
	yy::parser parse(lexer);
	parse.set_debug_level(1);
	parse();


}
