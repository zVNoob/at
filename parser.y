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
#include <vector>
#define yylex lexer->lex
namespace lexer {class Lexer;}
namespace error {class ErrorReporter;}
namespace object {class Object;}
}

%{
#include "lexer.hpp"
#include "error.hpp"

#include "object.hpp"
#include "variable.hpp"
#include "tuple.hpp"
#include "array.hpp"

#include "parser_helper.hpp"
#include <iostream>
#include <memory>
%}


%token <std::shared_ptr<object::Object>> INTEGER
%token <std::shared_ptr<object::Object>> FRACTION
%token <std::shared_ptr<object::Object>> STRING

%token <std::shared_ptr<object::Object>> IDENTIFIER
%token <std::shared_ptr<object::Object>> VARIABLE

%nterm <std::shared_ptr<object::Object>> vars
%nterm <std::shared_ptr<object::Object>> expr
%nterm <std::vector<std::shared_ptr<object::Object>>> expr_list
%nterm <std::vector<std::shared_ptr<object::Object>>> var_list
%nterm <std::vector<std::shared_ptr<object::Object>>> arg_list


%right ':' '='

%right '[' ']' '(' ')'


%left '+' '-'
%left '*' '/' '%'
%right UNARY
%right '`'

%start scope
%%

vars: VARIABLE { $$ = $1; }
    | expr '[' arg_list ']' { $$ = exec_call($1,$3,"[]",@2); }
    | expr '(' arg_list ')' { $$ = exec_call($1,$3,"()",@2); }

expr: INTEGER { $$ = $1; }
    | FRACTION { $$ = $1; }
    | STRING { $$ = $1; }
    | vars { $$ = static_pointer_cast<variable::Variable>($1)->get_value(); }
    | expr '+' expr { $$ = exec_binary_op($1,$3,"+",@2); }
    | expr '-' expr { $$ = exec_binary_op($1,$3,"-",@2); }
    | expr '*' expr { $$ = exec_binary_op($1,$3,"*",@2); }
    | expr '/' expr { $$ = exec_binary_op($1,$3,"/",@2); }
    | expr '%' expr { $$ = exec_binary_op($1,$3,"%",@2); }
    | '(' expr ')' { $$ = $2; }
    | '+' expr %prec UNARY { $$ = exec_unary_op($2,"+",@1); }
    | '-' expr %prec UNARY { $$ = exec_unary_op($2,"-",@1); }
    | '~' expr %prec UNARY { $$ = exec_unary_op($2,"~",@1); }
    | '(' expr_list ')' { $$ = make_shared<tuple::Tuple>($2); }
    | '[' arg_list ']' { $$ = exec_build_array($2,@2); }

arg_list: expr { $$ = std::vector<std::shared_ptr<object::Object>>{$1}; }
        | arg_list ',' expr { $$ = std::move($1); $$.push_back($3); }

expr_list: expr ',' expr { $$ = std::vector<std::shared_ptr<object::Object>>{$1,$3}; }
         | expr_list ',' expr { $$ = std::move($1); $$.push_back($3); }

var_list: IDENTIFIER { $$ = std::vector<std::shared_ptr<object::Object>>{$1}; }
        | vars { $$ = std::vector<std::shared_ptr<object::Object>>{$1}; }
        | var_list ',' IDENTIFIER { $$ = std::move($1); $$.push_back($3); }
        | var_list ',' vars { $$ = std::move($1); $$.push_back($3); }


stmt: %empty
    | expr { err_rp->orphan_value($1); }
    | IDENTIFIER { throw error::eval_error("Undefined variable: " + static_pointer_cast<variable::Variable>($1)->name, @1); }
    | var_list '=' expr { exec_assign($1,$3,@2); }
    | var_list ':' expr { exec_declare($1,$3,lexer,@2); }
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
