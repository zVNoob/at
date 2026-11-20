%require "3.2"
%language "c++"
%header

%define api.parser.class {Parser}
%define api.namespace {parser}

%locations

%code provides { int yylex(parser::Parser::semantic_type*, parser::Parser::location_type*); }

%code requires {
#include "ast.h"
namespace at { extern AstPtr g_parse_result; }
}

%define api.value.type variant


%token <at::AstPtr> INTEGER // Support hex, oct, bin notation
%token <at::AstPtr> DECIMAL // Support exponent notation
%token <at::AstPtr> STRING // use '\"' as begin/end delimiter
%token <at::AstPtr> IMPORT // like string, but use '\'' as begin/end delimiter, work like Lua's require(...)
%token <at::AstPtr> IDENTIFIER // Allow whitespace, tab, newline, UTF8 in middle of ID

%token <at::AstPtr> SCOPE "::"
%token <at::AstPtr> TEMPLATE_OPEN "<|"
%token <at::AstPtr> TEMPLATE_CLOSE "|>"
%token <at::AstPtr> OROR "||"
%token <at::AstPtr> ANDAND "&&"
%token <at::AstPtr> XORXOR "^^"
%token <at::AstPtr> LE "<="
%token <at::AstPtr> EQEQ "=="
%token <at::AstPtr> GE ">="
%token <at::AstPtr> NE "!="
%token <at::AstPtr> CONSTRUCT_OPEN "<{"
%token <at::AstPtr> CONSTRUCT_CLOSE "}>"
%token <at::AstPtr> ARROW "->"
%token <at::AstPtr> FUNPTR "@?"
%token <at::AstPtr> RETURN "@<"
%token <at::AstPtr> CONTINUE "$^"
%token <at::AstPtr> BREAK "$!"
%token <at::AstPtr> SELF_FUNC "@$"
%token <at::AstPtr> SELF_CLASS "#$"
%token <at::AstPtr> PROT "^:"
%token <at::AstPtr> PRIV "~:"
%token <at::AstPtr> CONST_PUB "!:"
%token <at::AstPtr> CONST_PROT "!^:"
%token <at::AstPtr> CONST_PRIV "!~:"
%token <at::AstPtr> ALIAS_PUBLIC ":="
%token <at::AstPtr> ALIAS_PRIVATE "!:="
%token <at::AstPtr> OVERLOAD "^="
%token <at::AstPtr> '+' '-' '*' '/' '%' '^' '|' '&' '<' '>' ':' '=' '?' '[' ']' '(' ')' '{' '}' '.' '`' '!' '@' '#' '$' '~' ',' ';'
%nonassoc TYPE_REDUCE
%nonassoc "::"
%nonassoc "<|"

%right ':' '='

%left '?'

%right '[' ']' '(' ')' '{' '}'

%left "||" "&&" "^^"
%left '|' '&' '^'
%left '<' '>' "<=" "==" ">=" "!="
%left '+' '-'
%left '*' '/' '%'
%right UNARY
%left '.'

%type <at::AstPtr> start scope stmt expr
%type <at::AstPtr> func_arg_declare func_arg_declare_list expr_list
%type <at::AstPtr> type base_type complex_type type_list
%type <at::AstPtr> class template_arg_declare_list template_arg_list
%type <at::AstPtr> anonymous_template declare cast_stmt cast_stmt_list
%type <at::AstPtr> tuple id_tuple_list

%start start
%%

expr: INTEGER { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); } // Literal
    | SELF_FUNC { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); } // Self function
    | SELF_CLASS { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); } // Self class
    | DECIMAL { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); }
    | STRING { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); }
    | IDENTIFIER { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); }
    | IMPORT { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); }
    | expr '+' expr { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); } // Operator
    | expr '-' expr { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
    | expr '*' expr { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
    | expr '/' expr { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
    | expr '%' expr { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
    | expr '^' expr { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
    | expr '|' expr { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
    | expr '&' expr { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
    | expr '?' expr ':' expr { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); if ($4) $$->children.push_back($4); if ($5) $$->children.push_back($5); }
    | expr '<' expr { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
    | expr '>' expr { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
    | expr "<=" expr { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
    | expr ">=" expr { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
    | expr "==" expr { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
    | expr "!=" expr { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
    | '-' expr %prec UNARY { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); }
    | '!' expr %prec UNARY { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); }
    | expr "||" expr { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
    | expr "&&" expr { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
    | expr "^^" expr { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
    | '(' expr ')' { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
    | expr '.' IDENTIFIER { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); } // Dot
    | expr '(' expr_list ')' { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); if ($4) $$->children.push_back($4); } // Call
    | expr '[' expr_list ']' { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); if ($4) $$->children.push_back($4); } // Index
    | '[' expr_list ']' { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); } // Array
    | IDENTIFIER "<{" expr_list "}>" { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); if ($4) $$->children.push_back($4); } // Construction
    | complex_type "<{" expr_list "}>" { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); if ($4) $$->children.push_back($4); }
    | '@' '(' func_arg_declare_list ')' '{' scope '}' { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); if ($4) $$->children.push_back($4); if ($5) $$->children.push_back($5); if ($6) $$->children.push_back($6); if ($7) $$->children.push_back($7); } // Function
    | '@' '(' func_arg_declare_list ')' "->" '(' type ')' '{' scope '}' { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); if ($4) $$->children.push_back($4); if ($5) $$->children.push_back($5); if ($6) $$->children.push_back($6); if ($7) $$->children.push_back($7); if ($8) $$->children.push_back($8); if ($9) $$->children.push_back($9); if ($10) $$->children.push_back($10); if ($11) $$->children.push_back($11); }
    | '&' expr { $$ = at::makeNode("expr", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); } // Borrowing reference, mutable

func_arg_declare: IDENTIFIER { $$ = at::makeNode("func_arg_declare", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); }
                | IDENTIFIER '=' expr { $$ = at::makeNode("func_arg_declare", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
                | IDENTIFIER ':' type { $$ = at::makeNode("func_arg_declare", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
                | IDENTIFIER ':' type '=' expr { $$ = at::makeNode("func_arg_declare", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); if ($4) $$->children.push_back($4); if ($5) $$->children.push_back($5); }


func_arg_declare_list: %empty { $$ = at::makeNode("func_arg_declare_list_empty", "", {@$.begin.line, @$.begin.column}); }


                     | func_arg_declare { $$ = at::makeNode("func_arg_declare_list", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); }
                     | func_arg_declare_list ',' func_arg_declare { $$ = at::makeNode("func_arg_declare_list", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }

expr_list: %empty { $$ = at::makeNode("expr_list_empty", "", {@$.begin.line, @$.begin.column}); }
         | expr { $$ = at::makeNode("expr_list", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); }
         | expr_list ',' expr { $$ = at::makeNode("expr_list", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }

type: base_type { $$ = at::makeNode("type", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); }
    | type "::" IDENTIFIER { $$ = at::makeNode("type", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }

base_type: IDENTIFIER %prec TYPE_REDUCE { $$ = at::makeNode("base_type", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); }
         | complex_type %prec TYPE_REDUCE { $$ = at::makeNode("base_type", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); }

complex_type: '`' expr '`' { $$ = at::makeNode("complex_type", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); } // Typeof
            | IDENTIFIER "<|" template_arg_list "|>" { $$ = at::makeNode("complex_type", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); if ($4) $$->children.push_back($4); } // template
            | anonymous_template "<|" template_arg_list "|>" { $$ = at::makeNode("complex_type", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); if ($4) $$->children.push_back($4); }
            | "@?" '(' func_arg_declare_list ')' { $$ = at::makeNode("complex_type", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); if ($4) $$->children.push_back($4); } // Function pointer
            | "@?" '(' func_arg_declare_list ')' "->" '(' type ')' { $$ = at::makeNode("complex_type", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); if ($4) $$->children.push_back($4); if ($5) $$->children.push_back($5); if ($6) $$->children.push_back($6); if ($7) $$->children.push_back($7); if ($8) $$->children.push_back($8); }
            | '*' type { $$ = at::makeNode("complex_type", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); } // Pointer
            | class { $$ = at::makeNode("complex_type", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); }

type_list: type { $$ = at::makeNode("type_list", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); }
         | type_list ',' type { $$ = at::makeNode("type_list", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }

class: '#' '(' func_arg_declare_list ')' '{' scope '}' { $$ = at::makeNode("class", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); if ($4) $$->children.push_back($4); if ($5) $$->children.push_back($5); if ($6) $$->children.push_back($6); if ($7) $$->children.push_back($7); } // Declare a class and the constructor
     | '#' '(' func_arg_declare_list ')' ':' type_list '{' scope '}' { $$ = at::makeNode("class", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); if ($4) $$->children.push_back($4); if ($5) $$->children.push_back($5); if ($6) $$->children.push_back($6); if ($7) $$->children.push_back($7); if ($8) $$->children.push_back($8); if ($9) $$->children.push_back($9); } // With base classes

template_arg_declare_list: IDENTIFIER { $$ = at::makeNode("template_arg_declare_list", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); }
                         | template_arg_declare_list ',' IDENTIFIER { $$ = at::makeNode("template_arg_declare_list", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }

template_arg_list: type { $$ = at::makeNode("template_arg_list", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); }
                 | template_arg_list ',' type { $$ = at::makeNode("template_arg_list", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }


anonymous_template: "<|" template_arg_declare_list "|>" "@?" '(' func_arg_declare_list ')' { $$ = at::makeNode("anonymous_template", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); if ($4) $$->children.push_back($4); if ($5) $$->children.push_back($5); if ($6) $$->children.push_back($6); if ($7) $$->children.push_back($7); } // Function pointer
                  | "<|" template_arg_declare_list "|>" "@?" '(' func_arg_declare_list ')' "->" type { $$ = at::makeNode("anonymous_template", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); if ($4) $$->children.push_back($4); if ($5) $$->children.push_back($5); if ($6) $$->children.push_back($6); if ($7) $$->children.push_back($7); if ($8) $$->children.push_back($8); if ($9) $$->children.push_back($9); }
                  | "<|" template_arg_declare_list "|>" class { $$ = at::makeNode("anonymous_template", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); if ($4) $$->children.push_back($4); }
                  | "<|" template_arg_declare_list "|>" '@' '(' func_arg_declare_list ')' '{' scope '}' { $$ = at::makeNode("anonymous_template", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); if ($4) $$->children.push_back($4); if ($5) $$->children.push_back($5); if ($6) $$->children.push_back($6); if ($7) $$->children.push_back($7); if ($8) $$->children.push_back($8); if ($9) $$->children.push_back($9); if ($10) $$->children.push_back($10); }
                  | "<|" template_arg_declare_list "|>" '@' '(' func_arg_declare_list ')' "->" '(' type ')' '{' scope '}' { $$ = at::makeNode("anonymous_template", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); if ($4) $$->children.push_back($4); if ($5) $$->children.push_back($5); if ($6) $$->children.push_back($6); if ($7) $$->children.push_back($7); if ($8) $$->children.push_back($8); if ($9) $$->children.push_back($9); if ($10) $$->children.push_back($10); if ($11) $$->children.push_back($11); if ($12) $$->children.push_back($12); if ($13) $$->children.push_back($13); if ($14) $$->children.push_back($14); }

declare: ':' { $$ = at::makeNode("declare", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); } // Public
       | "^:" { $$ = at::makeNode("declare", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); } // Protected
       | "~:" { $$ = at::makeNode("declare", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); } // Private
       | "!:" { $$ = at::makeNode("declare", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); } // Const public
       | "!^:" { $$ = at::makeNode("declare", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); } // Const protected
       | "!~:" { $$ = at::makeNode("declare", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); } // Const private

cast_stmt: expr "->" type { $$ = at::makeNode("cast_stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); } // Cast expr to type
         | IDENTIFIER ':' expr "->" type { $$ = at::makeNode("cast_stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); if ($4) $$->children.push_back($4); if ($5) $$->children.push_back($5); } // Cast expr to type, with name IDENTIFIER, useful in case of same value casting to 2 different type

cast_stmt_list: cast_stmt { $$ = at::makeNode("cast_stmt_list", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); }
              | cast_stmt_list ',' cast_stmt { $$ = at::makeNode("cast_stmt_list", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }



tuple: expr ',' expr { $$ = at::makeNode("tuple", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
     | tuple ',' expr { $$ = at::makeNode("tuple", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }

id_tuple_list: IDENTIFIER ',' IDENTIFIER { $$ = at::makeNode("id_tuple_list", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
             | id_tuple_list ',' IDENTIFIER { $$ = at::makeNode("id_tuple_list", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }

stmt: %empty { $$ = at::makeNode("stmt_empty", "", {@$.begin.line, @$.begin.column}); }
    | "@<" expr { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); } // Return
    | "@<" { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); } // Return (void)
    | "$^" { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); } // Continue
    | "$!" { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); } // Break
    | expr { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); }
    | expr '=' expr { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); } // assignment
    | expr "^=" expr { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); } // Overloading, only applied to functions
    | tuple '=' tuple { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
    | IDENTIFIER declare expr { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); } // declaration
    | '(' id_tuple_list ')' declare tuple { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); if ($4) $$->children.push_back($4); if ($5) $$->children.push_back($5); }
    | IDENTIFIER declare type '=' expr { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); if ($4) $$->children.push_back($4); if ($5) $$->children.push_back($5); } // declaration with explict type
    | IDENTIFIER declare type '@' INTEGER { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); if ($4) $$->children.push_back($4); if ($5) $$->children.push_back($5); } // Maunal memory pinning, useful for MMIO
    | IDENTIFIER ":=" type { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); } // Public alias
    | IDENTIFIER "!:=" type { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); } // Private alias
    | IDENTIFIER ":=" anonymous_template { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
    | IDENTIFIER "!:=" anonymous_template { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
    | '{' scope '}' { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); } // Scope/Block
    | expr '?' '{' scope '}' { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); if ($4) $$->children.push_back($4); if ($5) $$->children.push_back($5); } // If
    | expr '?' '{' scope '}' ':' '{' scope '}' { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); if ($4) $$->children.push_back($4); if ($5) $$->children.push_back($5); if ($6) $$->children.push_back($6); if ($7) $$->children.push_back($7); if ($8) $$->children.push_back($8); if ($9) $$->children.push_back($9); } // If-else
    | '$' '(' expr ')' '{' scope '}' { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); if ($4) $$->children.push_back($4); if ($5) $$->children.push_back($5); if ($6) $$->children.push_back($6); if ($7) $$->children.push_back($7); } // while loop
    | '$' '(' IDENTIFIER ':' expr ')' '{' scope '}' { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); if ($4) $$->children.push_back($4); if ($5) $$->children.push_back($5); if ($6) $$->children.push_back($6); if ($7) $$->children.push_back($7); if ($8) $$->children.push_back($8); if ($9) $$->children.push_back($9); } // for loop
    | '*' '(' expr_list ')' '{' scope '}' { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); if ($4) $$->children.push_back($4); if ($5) $$->children.push_back($5); if ($6) $$->children.push_back($6); if ($7) $$->children.push_back($7); } // Unwrap: bind each expr to alias name, execute scope if all non-null
    | '*' '(' expr_list ')' '{' scope '}' ':' '{' scope '}' { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); if ($4) $$->children.push_back($4); if ($5) $$->children.push_back($5); if ($6) $$->children.push_back($6); if ($7) $$->children.push_back($7); if ($8) $$->children.push_back($8); if ($9) $$->children.push_back($9); if ($10) $$->children.push_back($10); if ($11) $$->children.push_back($11); } // If any null, run else block
    | '?' '(' cast_stmt_list ')' '{' scope '}' { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); if ($4) $$->children.push_back($4); if ($5) $$->children.push_back($5); if ($6) $$->children.push_back($6); if ($7) $$->children.push_back($7); } // Try cast, execute sccope if success
    | '?' '(' cast_stmt_list ')' '{' scope '}' ':' '{' scope '}' { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); if ($4) $$->children.push_back($4); if ($5) $$->children.push_back($5); if ($6) $$->children.push_back($6); if ($7) $$->children.push_back($7); if ($8) $$->children.push_back($8); if ($9) $$->children.push_back($9); if ($10) $$->children.push_back($10); if ($11) $$->children.push_back($11); } // Otherwise, run else block
    | '+' "^=" expr { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); } // Operator overloading
    | '-' "^=" expr { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
    | '*' "^=" expr { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
    | '/' "^=" expr { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
    | '%' "^=" expr { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
    | '^' "^=" expr { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
    | '&' "^=" expr { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
    | '|' "^=" expr { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
    | "||" "^=" expr { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
    | "&&" "^=" expr { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
    | "^^" "^=" expr { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
    | "==" "^=" expr { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
    | "<=" "^=" expr { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
    | '<' "^=" expr { $$ = at::makeNode("stmt", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }
scope: %empty { $$ = at::makeNode("scope_empty", "", {@$.begin.line, @$.begin.column}); }
     | scope stmt ';' { $$ = at::makeNode("scope", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); if ($2) $$->children.push_back($2); if ($3) $$->children.push_back($3); }

start: scope { $$ = at::makeNode("start", "", {@$.begin.line, @$.begin.column}); if ($1) $$->children.push_back($1); at::g_parse_result = $$; }
%%
