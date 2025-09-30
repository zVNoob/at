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
namespace type {class Type;}
}

%{
#include "lexer.hpp"
#include "error.hpp"

#include "object.hpp"
#include "variable.hpp"
#include "tuple.hpp"
#include "array.hpp"
#include "type.hpp"

#include "parser_helper.hpp"
#include <iostream>
#include <memory>
%}

%token EQUAL

%token <std::shared_ptr<object::Object>> INTEGER
%token <std::shared_ptr<object::Object>> FRACTION
%token <std::shared_ptr<object::Object>> STRING

%token <std::shared_ptr<object::Object>> IDENTIFIER
%token <std::shared_ptr<object::Object>> VARIABLE
%token <std::shared_ptr<object::Object>> UNRESOLVED_VARIABLE
%token <std::shared_ptr<object::Object>> TYPE

%nterm <std::shared_ptr<object::Object>> vars
%nterm <std::shared_ptr<type::Type>> type
%nterm <std::shared_ptr<object::Object>> expr
%nterm <std::vector<std::shared_ptr<object::Object>>> expr_list
%nterm <std::vector<std::shared_ptr<object::Object>>> var_list
%nterm <std::vector<std::shared_ptr<object::Object>>> arg_list
%nterm <std::vector<std::shared_ptr<type::Type>>> type_list

%right ':' '='

%left TYPE_PREC

%left '?'

%right '[' ']' '(' ')'

%left '|' '&' '^'
%left '<' '>' EQUAL
%left '+' '-'
%left '*' '/' '%'
%right UNARY
%right '`'

%start start
%%

vars: VARIABLE { $$ = $1; }
    | expr '[' arg_list ']' { $$ = exec_call($1,$3,"[]",@2); }

type: TYPE { $$ = std::static_pointer_cast<type::Type>($1); }
    | '`' expr { $$ = $2->type; }
    | '[' type ']' { $$ = array::get_Array_type($2); }
    | '(' type_list ')' { $$ = tuple::get_Tuple_type($2); }

type_list: type ',' type { $$ = std::vector<std::shared_ptr<type::Type>>{$1,$3}; }
         | type_list ',' type { $$ = std::move($1); $$.push_back($3); }

expr: INTEGER { $$ = $1; }
    | FRACTION { $$ = $1; }
    | STRING { $$ = $1; }
    | vars { if (dynamic_cast<variable::Variable*>($1.get())) $$ = static_pointer_cast<variable::Variable>($1)->get_value(); else $$ = $1; }
    | UNRESOLVED_VARIABLE { $$ = $1; }
    | expr '+' expr { $$ = exec_binary_op($1,$3,"+",@2); }
    | expr '-' expr { $$ = exec_binary_op($1,$3,"-",@2); }
    | expr '*' expr { $$ = exec_binary_op($1,$3,"*",@2); }
    | expr '/' expr { $$ = exec_binary_op($1,$3,"/",@2); }
    | expr '%' expr { $$ = exec_binary_op($1,$3,"%",@2); }
    | expr '<' expr { $$ = exec_binary_op($1,$3,"<",@2); }
    | expr '>' expr { $$ = exec_binary_op($1,$3,">",@2); }
    | expr EQUAL expr { $$ = exec_binary_op($1,$3,"==",@2); }
    | expr '|' expr { $$ = exec_binary_op($1,$3,"|",@2); }
    | expr '&' expr { $$ = exec_binary_op($1,$3,"&",@2); }
    | expr '^' expr { $$ = exec_binary_op($1,$3,"^",@2); }
    | '(' expr ')' { $$ = $2; }
    | '+' expr %prec UNARY { $$ = exec_unary_op($2,"+",@1); }
    | '-' expr %prec UNARY { $$ = exec_unary_op($2,"-",@1); }
    | '~' expr %prec UNARY { $$ = exec_unary_op($2,"~",@1); }
    | '!' expr %prec UNARY { $$ = exec_unary_op($2,"!",@1); }
    | '(' expr_list ')' { $$ = make_shared<tuple::Tuple>($2); }
    | '[' arg_list ']' { $$ = exec_build_array($2,@2); }
    | type '(' arg_list ')' { $$ = exec_constructor($1,$3,@2); }
    | expr '(' arg_list ')' { $$ = exec_call($1,$3,"()",@2); }
    | expr '?' expr ':' expr { $$ = exec_conditional($1,$3,$5,@2); }

arg_list: %empty { $$ = std::vector<std::shared_ptr<object::Object>>{}; }
        | expr { $$ = std::vector<std::shared_ptr<object::Object>>{$1}; }
        | arg_list ',' expr { $$ = std::move($1); $$.push_back($3); }

expr_list: expr ',' expr { $$ = std::vector<std::shared_ptr<object::Object>>{$1,$3}; }
         | expr_list ',' expr { $$ = std::move($1); $$.push_back($3); }

var_list: IDENTIFIER { $$ = std::vector<std::shared_ptr<object::Object>>{$1}; }
        | vars { $$ = std::vector<std::shared_ptr<object::Object>>{$1}; }
        | UNRESOLVED_VARIABLE { $$ = std::vector<std::shared_ptr<object::Object>>{$1}; }
        | var_list ',' IDENTIFIER { $$ = std::move($1); $$.push_back($3); }
        | var_list ',' vars { $$ = std::move($1); $$.push_back($3); }
        | var_list ',' UNRESOLVED_VARIABLE { $$ = std::move($1); $$.push_back($3); }


stmt: %empty
    | expr { on_orphan_value($1,lexer,err_rp); }
    | IDENTIFIER { throw error::eval_error("Undefined variable: " + static_pointer_cast<variable::Variable>($1)->name, @1); }
    | var_list '=' expr { exec_assign($1,$3,lexer,@2); }
    | var_list ':' expr { exec_declare($1,$3,lexer,@2); }
    | var_list ':' type { exec_declare($1,$3,lexer,@2); }
    | '{' { push_scope_block(lexer); } scope '}' { 
      on_orphan_value(pop_scope_block(lexer),lexer,err_rp); 
    }
    | '$' '(' <bool>{ 
      $$ = lexer->current_scope->auto_resolve;
      lexer->current_scope->auto_resolve = false;
      } expr ')' '{' {
        lexer->current_scope->auto_resolve = $3;
        push_scope_loop(lexer, @1, $4);
      } scope '}' {
        on_orphan_value(pop_scope_loop(lexer),lexer,err_rp);
      }
    | error
    ;


scope: %empty
     | scope stmt ';'
     ;

start: scope { lexer->current_scope->eval(); }

%%

namespace parser {
  void Parser::error(const Parser::location_type& location, const std::string& message) {
    err_rp->report(lexer, location, message);
  }
}
