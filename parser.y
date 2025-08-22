%require "3.2"
%language "c++"
%header

%define api.parser.class {Parser}
%define api.namespace {parser}

%locations
%define parse.error verbose
%define api.value.type variant
%define parse.trace
%define parse.assert

%parse-param {lexer::Lexer* lexer}
%parse-param {error::ErrorReporter* err_rp}

%code requires {
#include "type/bigint.hpp"
#include "type/bigfraction.hpp"
#define yylex lexer->lex
namespace lexer {class Lexer;}
namespace error {class ErrorReporter;}
namespace object {class Object;}
}

%{
#include "type/bigint.hpp"
#include "type/bigfraction.hpp"

#include "lexer.hpp"
#include "error.hpp"
#include "parser_helper.hpp"

#include "objects/object.hpp"

#include "objects/integer.hpp"
#include "objects/string.hpp"

#include <iostream>
%}

%token <BigInt> INTEGER
%token <BigFraction> FRACTION
%token <std::string> STRING

%token <std::string> IDENTIFIER

%nterm <object::Object*> expr



%left '+' '-'
%left '*' '/' '%'
%right UNARY


%start scope
%%


expr: INTEGER { $$ = new integer::Integer($1,@1); }
    | STRING { $$ = new string::String($1,@1); }
    | expr '+' expr {$$ = exec_binary_op($1, $3, "+"); }
    | expr '-' expr {$$ = exec_binary_op($1, $3, "-"); }
    | expr '*' expr {$$ = exec_binary_op($1, $3, "*"); }
    | '+' expr {$$ = exec_unary_op($2, "+"); }
    | '-' expr {$$ = exec_unary_op($2, "-"); }
    ;

stmt: %empty
    | expr { std::cout << $1->to_string() << std::endl; delete $1;}
    | error
    ;


scope: %empty
     | scope stmt ';'
     ;

%%

namespace parser {
  void Parser::error(const Parser::location_type& location, const std::string& message) {
    err_rp->report(lexer, location, message);
  }
}
