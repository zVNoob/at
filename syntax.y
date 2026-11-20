%require "3.2"
%language "c++"
%header

%define api.parser.class {Parser}
%define api.namespace {parser}

%locations


%token INTEGER // Support hex, oct, bin notation
%token DECIMAL // Support exponent notation
%token STRING // use '\"' as begin/end delimiter
%token IMPORT // like string, but use '\'' as begin/end delimiter, work like Lua's require(...)
%token IDENTIFIER // Allow whitespace, tab, newline, UTF8 in middle of ID

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

%start start
%%

expr: INTEGER // Literal
    | DECIMAL
    | STRING
    | IDENTIFIER
    | IMPORT
    | expr '+' expr // Operator
    | expr '-' expr
    | expr '*' expr
    | expr '/' expr
    | expr '%' expr
    | expr '^' expr
    | expr '|' expr
    | expr '&' expr
    | expr '?' expr ':' expr
    | expr '<' expr
    | expr '>' expr
    | expr "<=" expr
    | expr ">=" expr
    | expr "==" expr
    | expr "!=" expr
    | '-' expr %prec UNARY
    | '!' expr %prec UNARY
    | expr "||" expr
    | expr "&&" expr
    | expr "^^" expr
    | '(' expr ')'
    | expr '.' IDENTIFIER // Dot
    | expr '(' expr_list ')' // Call
    | expr '[' expr_list ']' // Index
    | '[' expr_list ']' // Array
    | IDENTIFIER "<{" expr_list "}>" // Construction
    | complex_type "<{" expr_list "}>"
    | '@' '(' func_arg_declare_list ')' '{' scope '}' // Function
    | '@' '(' func_arg_declare_list ')' "->" '(' type ')' '{' scope '}'
    | '&' expr // Borrowing reference, mutable

func_arg_declare: IDENTIFIER
                | IDENTIFIER '=' expr
                | IDENTIFIER ':' type
                | IDENTIFIER ':' type '=' expr


func_arg_declare_list: %empty
                     | func_arg_declare
                     | func_arg_declare_list ',' func_arg_declare

expr_list: %empty 
         | expr
         | expr_list ',' expr

type: base_type
    | type "::" IDENTIFIER

base_type: IDENTIFIER %prec TYPE_REDUCE
         | complex_type %prec TYPE_REDUCE

complex_type: '`' expr '`' // Typeof
            | IDENTIFIER "<|" template_arg_list "|>" // template
            | anonymous_template "<|" template_arg_list "|>"
            | "@?" '(' func_arg_declare_list ')' // Function pointer
            | "@?" '(' func_arg_declare_list ')' "->" '(' type ')'
            | '*' type // Pointer
            | class

type_list: type
         | type_list ',' type

class: '#' '(' func_arg_declare_list ')' '{' scope '}' // Declare a class and the constructor
     | '#' '(' func_arg_declare_list ')' ':' type_list '{' scope '}' // With base classes

template_arg_declare_list: IDENTIFIER
                         | template_arg_declare_list ',' IDENTIFIER

template_arg_list: type
                 | template_arg_list ',' type 


anonymous_template: "<|" template_arg_declare_list "|>" "@?" '(' func_arg_declare_list ')' // Function pointer
                  | "<|" template_arg_declare_list "|>" "@?" '(' func_arg_declare_list ')' "->" type
                  | "<|" template_arg_declare_list "|>" class
                  | "<|" template_arg_declare_list "|>" '@' '(' func_arg_declare_list ')' '{' scope '}'
                  | "<|" template_arg_declare_list "|>" '@' '(' func_arg_declare_list ')' "->" '(' type ')' '{' scope '}' 

declare: ':' // Public
       | "^:" // Protected
       | "~:" // Private
       | "!:" // Const public
       | "!^:" // Const protected
       | "!~:" // Const private

cast_stmt: expr "->" type // Cast expr to type
         | IDENTIFIER ':' expr "->" type // Cast expr to type, with name IDENTIFIER, useful in case of same value casting to 2 different type

cast_stmt_list: cast_stmt
              | cast_stmt_list ',' cast_stmt

unwrap_stmt: IDENTIFIER ':' expr

unwrap_stmt_list: unwrap_stmt
                | unwrap_stmt_list ',' unwrap_stmt

tuple: expr ',' expr
     | tuple ',' expr

id_tuple_list: IDENTIFIER ',' IDENTIFIER
             | id_tuple_list ',' IDENTIFIER

stmt: %empty
    | "@<" expr // Return
    | "@<" // Return none
    | "$^" // Continue
    | "$!" // Break
    | "@$" // "Self" function, useful for recursion
    | "#$" // "Self" class, or "this"
    | expr
    | expr '=' expr // assignment
    | expr "^=" expr // Overloading, only applied to functions
    | tuple '=' tuple
    | IDENTIFIER declare expr // declaration
    | '(' id_tuple_list ')' declare tuple
    | IDENTIFIER declare type '=' expr // declaration with explict type
    | IDENTIFIER declare type '@' INTEGER // Maunal memory pinning, useful for MMIO
    | IDENTIFIER ":=" type // Public alias
    | IDENTIFIER "!:=" type // Private alias
    | IDENTIFIER ":=" anonymous_template
    | IDENTIFIER "!:=" anonymous_template
    | '{' scope '}' // Scope/Block
    | expr '?' '{' scope '}' // If
    | expr '?' '{' scope '}' ':' '{' scope '}' // If-else
    | '$' '(' expr ')' '{' scope '}' // while loop
    | '$' '(' IDENTIFIER ':' expr ')' '{' scope '}' // for loop
    | '*' '(' unwrap_stmt_list ')' '{' scope '}' // Unwrap all variable in expr_list, make it available in scope and execute scope if success
    | '*' '(' unwrap_stmt_list ')' '{' scope '}' ':' '{' scope '}' // If fail, run else block
    | '?' '(' cast_stmt_list ')' '{' scope '}' // Try cast, execute sccope if success
    | '?' '(' cast_stmt_list ')' '{' scope '}' ':' '{' scope '}' // Otherwise, run else block
    | '+' "^=" expr // Operator overloading
    | '-' "^=" expr 
    | '*' "^=" expr
    | '/' "^=" expr
    | '%' "^=" expr
    | '^' "^=" expr
    | '&' "^=" expr
    | '|' "^=" expr
    | "||" "^=" expr
    | "&&" "^=" expr
    | "^^" "^=" expr
    | "==" "^=" expr
    | "<=" "^=" expr
    | "<" "^=" expr
scope: %empty
     | scope stmt ';'

start: scope
%%
