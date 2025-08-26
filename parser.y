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
#include <memory>
#define yylex lexer->lex
namespace lexer {class Lexer;}
namespace error {class ErrorReporter;}
namespace object {class Object;}
namespace variable {class Variable;}
}

%{
#include "bigint.hpp"
#include "bigfraction.hpp"

#include "lexer.hpp"
#include "error.hpp"
#include "parser_helper.hpp"

#include "object.hpp"

#include "integer.hpp"
#include "string.hpp"
#include "fraction.hpp"
#include "variable.hpp"

#include <iostream>
#include <memory>
%}

%token <BigInt> INTEGER
%token <BigFraction> FRACTION
%token <std::string> STRING

%token <std::string> IDENTIFIER
%token <std::shared_ptr<variable::Variable>> VARIABLE

%nterm <std::shared_ptr<object::Object>> expr
%nterm <std::shared_ptr<object::Object>> unknown_expr

%nterm <std::vector<std::shared_ptr<object::Object>>> expr_list


%left UNEVALUABLE

%left '+' '-'
%left '*' '/' '%'
%right POWER
%right UNARY


%start scope
%%


expr: INTEGER { $$ = std::make_shared<integer::Integer>($1,@1); }
    | STRING { $$ = std::make_shared<string::String>($1,@1); }
    | FRACTION { $$ = std::make_shared<fraction::Fraction>($1,@1); }
    | VARIABLE { $$ = ($1->get_value()); }
    | expr '+' expr {$$ = exec_binary_op($1, $3, "+"); }
    | expr '-' expr {$$ = exec_binary_op($1, $3, "-"); }
    | expr '*' expr {$$ = exec_binary_op($1, $3, "*"); }
    | expr '/' expr {$$ = exec_binary_op($1, $3, "/"); }
    | expr '%' expr {$$ = exec_binary_op($1, $3, "%"); }
    | '+' expr {$$ = exec_unary_op($2, "+"); }
    | '-' expr {$$ = exec_unary_op($2, "-"); }
    ;

unknown_expr: expr %prec UNEVALUABLE { $$ = $1; }
            | unknown_expr '+' unknown_expr { $$ = exec_binary_op($1, $3, "+"); }

expr_list: unknown_expr { std::vector<std::shared_ptr<object::Object>> v; v.emplace_back($1); $$ = v; }
         | expr_list ',' unknown_expr { $1.emplace_back($3); $$ = $1; }
         ;

stmt: %empty
    | expr_list { for( auto& s:$1) std::cout << s->to_string() << std::endl;}
    | IDENTIFIER ':' expr { exec_declare($1, $3, lexer);}
    | VARIABLE ':' expr { exec_declare($1->name, $3, lexer);}
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
