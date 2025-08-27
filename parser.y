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

%printer { yyo << $$->to_string(); } expr

%code requires {
#include "bigint.hpp"
#include "bigfraction.hpp"
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

%token <std::shared_ptr<variable::Variable>> LHS_VARIABLE

%nterm <std::shared_ptr<object::Object>> expr
%nterm <std::shared_ptr<object::Object>> unknown_expr

%nterm <std::vector<std::shared_ptr<object::Object>>> expr_list
%nterm <std::vector<std::string>> identifier_list
%nterm <std::vector<std::shared_ptr<variable::Variable>>> variable_list

%left UNEVALUABLE

%left '+' '-'
%left '*' '/' '%'
%right UNARY


%start scope
%%

expr: INTEGER { $$ = std::make_shared<integer::Integer>($1,@1); }
    | STRING { $$ = std::make_shared<string::String>($1,@1); }
    | FRACTION { $$ = std::make_shared<fraction::Fraction>($1,@1); }
    | VARIABLE { $$ = $1->get_value(); }
    | expr '+' expr { $$ = exec_binary_op($1, $3, "+"); }
    | expr '-' expr { $$ = exec_binary_op($1, $3, "-"); }
    | expr '*' expr { $$ = exec_binary_op($1, $3, "*"); }
    | expr '/' expr { $$ = exec_binary_op($1, $3, "/"); }
    | expr '%' expr { $$ = exec_binary_op($1, $3, "%"); }
    | '(' expr ')' { $$ = $2; }
    | '+' expr { $$ = exec_unary_op($2, "+"); }
    | '-' expr { $$ = exec_unary_op($2, "-"); }
    ;

unknown_expr: expr %prec UNEVALUABLE { $$ = $1; }

expr_list: unknown_expr { std::vector<std::shared_ptr<object::Object>> v; v.emplace_back($1); $$ = v; }
         | expr_list ',' unknown_expr { $1.emplace_back($3); $$ = $1; }
         ;

identifier_list: IDENTIFIER { std::vector<std::string> v; v.emplace_back($1); $$ = v; }
               | identifier_list ',' IDENTIFIER { $1.emplace_back($3); $$ = $1; }
               ;

variable_list: LHS_VARIABLE { std::vector<std::shared_ptr<variable::Variable>> v; v.emplace_back($1); $$ = v; }
             | variable_list ',' LHS_VARIABLE { $1.emplace_back($3); $$ = $1; }
             ;

stmt: %empty
    | expr_list { if (!lexer->scope->parent) err_rp->orphan_return($1);}
    | identifier_list ':' expr_list { exec_declare($1, $3, lexer, @1);}
    | variable_list '=' expr_list { exec_assign($1, $3, lexer, @1); }
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
