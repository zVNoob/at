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
#include <memory>
#define yylex lexer->lex
namespace lexer {class Lexer;}
namespace error {class ErrorReporter;}
namespace object {class Object;}
}

%{
#include "lexer.hpp"
#include "error.hpp"

#include "object.hpp"

#include "parser_helper.hpp"
#include <iostream>
#include <memory>
%}


%token <std::shared_ptr<object::Object>> INTEGER
%token <std::shared_ptr<object::Object>> FRACTION
%token <std::shared_ptr<object::Object>> STRING

%token <std::string> IDENTIFIER

%nterm <std::shared_ptr<object::Object>> expr
%nterm <std::vector<std::shared_ptr<object::Object>>> expr_list

%left UNEVALUATABLE

%right '[' ']'

%left '+' '-'
%left '*' '/' '%'
%right UNARY
%right '`'

%start scope
%%


expr: INTEGER { $$ = $1; }
    | FRACTION { $$ = $1; }
    | STRING { $$ = $1; }
    | expr '+' expr { $$ = exec_binary_op($1,$3,"+",@2); }
    | expr '-' expr { $$ = exec_binary_op($1,$3,"-",@2); }
    | expr '*' expr { $$ = exec_binary_op($1,$3,"*",@2); }
    | expr '/' expr { $$ = exec_binary_op($1,$3,"/",@2); }
    | expr '%' expr { $$ = exec_binary_op($1,$3,"%",@2); }
    | '(' expr ')' { $$ = $2; }
    | '+' expr %prec UNARY { $$ = exec_unary_op($2,"+",@1); }
    | '-' expr %prec UNARY { $$ = exec_unary_op($2,"-",@1); }
    | '~' expr %prec UNARY { $$ = exec_unary_op($2,"~",@1); }
    | expr '[' expr_list ']' { $$ = exec_index_op($1,$3,@2); }

expr_list: expr { $$ = std::vector<std::shared_ptr<object::Object>>{$1}; }
         | expr_list ',' expr { $$ = $1; $1.push_back($3); }


stmt: %empty
    | expr { err_rp->orphan_value($1); }
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
