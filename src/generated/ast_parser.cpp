// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.





#include "ast_parser.hpp"




#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K].location)
/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

# ifndef YYLLOC_DEFAULT
#  define YYLLOC_DEFAULT(Current, Rhs, N)                               \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).begin  = YYRHSLOC (Rhs, 1).begin;                   \
          (Current).end    = YYRHSLOC (Rhs, N).end;                     \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).begin = (Current).end = YYRHSLOC (Rhs, 0).end;      \
        }                                                               \
    while (false)
# endif


// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YY_USE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

#line 6 "src/syntax.ast.y"
namespace parser {
#line 137 "src/generated/ast_parser.cpp"

  /// Build a parser object.
  Parser::Parser ()
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr)
#else

#endif
  {}

  Parser::~Parser ()
  {}

  Parser::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------.
  | symbol.  |
  `---------*/

  // basic_symbol.
  template <typename Base>
  Parser::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value ()
    , location (that.location)
  {
    switch (this->kind ())
    {
      case symbol_kind::S_INTEGER: // INTEGER
      case symbol_kind::S_DECIMAL: // DECIMAL
      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_IMPORT: // IMPORT
      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_SCOPE: // "::"
      case symbol_kind::S_TEMPLATE_OPEN: // "<|"
      case symbol_kind::S_TEMPLATE_CLOSE: // "|>"
      case symbol_kind::S_OROR: // "||"
      case symbol_kind::S_ANDAND: // "&&"
      case symbol_kind::S_XORXOR: // "^^"
      case symbol_kind::S_LE: // "<="
      case symbol_kind::S_EQEQ: // "=="
      case symbol_kind::S_GE: // ">="
      case symbol_kind::S_NE: // "!="
      case symbol_kind::S_CONSTRUCT_OPEN: // "<{"
      case symbol_kind::S_CONSTRUCT_CLOSE: // "}>"
      case symbol_kind::S_ARROW: // "->"
      case symbol_kind::S_FUNPTR: // "@?"
      case symbol_kind::S_RETURN: // "@<"
      case symbol_kind::S_CONTINUE: // "$^"
      case symbol_kind::S_BREAK: // "$!"
      case symbol_kind::S_SELF_FUNC: // "@$"
      case symbol_kind::S_SELF_CLASS: // "#$"
      case symbol_kind::S_PROT: // "^:"
      case symbol_kind::S_PRIV: // "~:"
      case symbol_kind::S_CONST_PUB: // "!:"
      case symbol_kind::S_CONST_PROT: // "!^:"
      case symbol_kind::S_CONST_PRIV: // "!~:"
      case symbol_kind::S_ALIAS_PUBLIC: // ":="
      case symbol_kind::S_ALIAS_PRIVATE: // "!:="
      case symbol_kind::S_OVERLOAD: // "^="
      case symbol_kind::S_35_: // '+'
      case symbol_kind::S_36_: // '-'
      case symbol_kind::S_37_: // '*'
      case symbol_kind::S_38_: // '/'
      case symbol_kind::S_39_: // '%'
      case symbol_kind::S_40_: // '^'
      case symbol_kind::S_41_: // '|'
      case symbol_kind::S_42_: // '&'
      case symbol_kind::S_43_: // '<'
      case symbol_kind::S_44_: // '>'
      case symbol_kind::S_45_: // ':'
      case symbol_kind::S_46_: // '='
      case symbol_kind::S_47_: // '?'
      case symbol_kind::S_48_: // '['
      case symbol_kind::S_49_: // ']'
      case symbol_kind::S_50_: // '('
      case symbol_kind::S_51_: // ')'
      case symbol_kind::S_52_: // '{'
      case symbol_kind::S_53_: // '}'
      case symbol_kind::S_54_: // '.'
      case symbol_kind::S_55_: // '`'
      case symbol_kind::S_56_: // '!'
      case symbol_kind::S_57_: // '@'
      case symbol_kind::S_58_: // '#'
      case symbol_kind::S_59_: // '$'
      case symbol_kind::S_60_: // '~'
      case symbol_kind::S_61_: // ','
      case symbol_kind::S_62_: // ';'
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_func_arg_declare: // func_arg_declare
      case symbol_kind::S_func_arg_declare_list: // func_arg_declare_list
      case symbol_kind::S_expr_list: // expr_list
      case symbol_kind::S_type: // type
      case symbol_kind::S_base_type: // base_type
      case symbol_kind::S_complex_type: // complex_type
      case symbol_kind::S_type_list: // type_list
      case symbol_kind::S_class: // class
      case symbol_kind::S_template_arg_declare_list: // template_arg_declare_list
      case symbol_kind::S_template_arg_list: // template_arg_list
      case symbol_kind::S_anonymous_template: // anonymous_template
      case symbol_kind::S_declare: // declare
      case symbol_kind::S_cast_stmt: // cast_stmt
      case symbol_kind::S_cast_stmt_list: // cast_stmt_list
      case symbol_kind::S_tuple: // tuple
      case symbol_kind::S_id_tuple_list: // id_tuple_list
      case symbol_kind::S_stmt: // stmt
      case symbol_kind::S_scope: // scope
      case symbol_kind::S_start: // start
        value.copy< at::AstPtr > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

  }




  template <typename Base>
  Parser::symbol_kind_type
  Parser::basic_symbol<Base>::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


  template <typename Base>
  bool
  Parser::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return this->kind () == symbol_kind::S_YYEMPTY;
  }

  template <typename Base>
  void
  Parser::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    switch (this->kind ())
    {
      case symbol_kind::S_INTEGER: // INTEGER
      case symbol_kind::S_DECIMAL: // DECIMAL
      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_IMPORT: // IMPORT
      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_SCOPE: // "::"
      case symbol_kind::S_TEMPLATE_OPEN: // "<|"
      case symbol_kind::S_TEMPLATE_CLOSE: // "|>"
      case symbol_kind::S_OROR: // "||"
      case symbol_kind::S_ANDAND: // "&&"
      case symbol_kind::S_XORXOR: // "^^"
      case symbol_kind::S_LE: // "<="
      case symbol_kind::S_EQEQ: // "=="
      case symbol_kind::S_GE: // ">="
      case symbol_kind::S_NE: // "!="
      case symbol_kind::S_CONSTRUCT_OPEN: // "<{"
      case symbol_kind::S_CONSTRUCT_CLOSE: // "}>"
      case symbol_kind::S_ARROW: // "->"
      case symbol_kind::S_FUNPTR: // "@?"
      case symbol_kind::S_RETURN: // "@<"
      case symbol_kind::S_CONTINUE: // "$^"
      case symbol_kind::S_BREAK: // "$!"
      case symbol_kind::S_SELF_FUNC: // "@$"
      case symbol_kind::S_SELF_CLASS: // "#$"
      case symbol_kind::S_PROT: // "^:"
      case symbol_kind::S_PRIV: // "~:"
      case symbol_kind::S_CONST_PUB: // "!:"
      case symbol_kind::S_CONST_PROT: // "!^:"
      case symbol_kind::S_CONST_PRIV: // "!~:"
      case symbol_kind::S_ALIAS_PUBLIC: // ":="
      case symbol_kind::S_ALIAS_PRIVATE: // "!:="
      case symbol_kind::S_OVERLOAD: // "^="
      case symbol_kind::S_35_: // '+'
      case symbol_kind::S_36_: // '-'
      case symbol_kind::S_37_: // '*'
      case symbol_kind::S_38_: // '/'
      case symbol_kind::S_39_: // '%'
      case symbol_kind::S_40_: // '^'
      case symbol_kind::S_41_: // '|'
      case symbol_kind::S_42_: // '&'
      case symbol_kind::S_43_: // '<'
      case symbol_kind::S_44_: // '>'
      case symbol_kind::S_45_: // ':'
      case symbol_kind::S_46_: // '='
      case symbol_kind::S_47_: // '?'
      case symbol_kind::S_48_: // '['
      case symbol_kind::S_49_: // ']'
      case symbol_kind::S_50_: // '('
      case symbol_kind::S_51_: // ')'
      case symbol_kind::S_52_: // '{'
      case symbol_kind::S_53_: // '}'
      case symbol_kind::S_54_: // '.'
      case symbol_kind::S_55_: // '`'
      case symbol_kind::S_56_: // '!'
      case symbol_kind::S_57_: // '@'
      case symbol_kind::S_58_: // '#'
      case symbol_kind::S_59_: // '$'
      case symbol_kind::S_60_: // '~'
      case symbol_kind::S_61_: // ','
      case symbol_kind::S_62_: // ';'
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_func_arg_declare: // func_arg_declare
      case symbol_kind::S_func_arg_declare_list: // func_arg_declare_list
      case symbol_kind::S_expr_list: // expr_list
      case symbol_kind::S_type: // type
      case symbol_kind::S_base_type: // base_type
      case symbol_kind::S_complex_type: // complex_type
      case symbol_kind::S_type_list: // type_list
      case symbol_kind::S_class: // class
      case symbol_kind::S_template_arg_declare_list: // template_arg_declare_list
      case symbol_kind::S_template_arg_list: // template_arg_list
      case symbol_kind::S_anonymous_template: // anonymous_template
      case symbol_kind::S_declare: // declare
      case symbol_kind::S_cast_stmt: // cast_stmt
      case symbol_kind::S_cast_stmt_list: // cast_stmt_list
      case symbol_kind::S_tuple: // tuple
      case symbol_kind::S_id_tuple_list: // id_tuple_list
      case symbol_kind::S_stmt: // stmt
      case symbol_kind::S_scope: // scope
      case symbol_kind::S_start: // start
        value.move< at::AstPtr > (YY_MOVE (s.value));
        break;

      default:
        break;
    }

    location = YY_MOVE (s.location);
  }

  // by_kind.
  Parser::by_kind::by_kind () YY_NOEXCEPT
    : kind_ (symbol_kind::S_YYEMPTY)
  {}

#if 201103L <= YY_CPLUSPLUS
  Parser::by_kind::by_kind (by_kind&& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {
    that.clear ();
  }
#endif

  Parser::by_kind::by_kind (const by_kind& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {}

  Parser::by_kind::by_kind (token_kind_type t) YY_NOEXCEPT
    : kind_ (yytranslate_ (t))
  {}



  void
  Parser::by_kind::clear () YY_NOEXCEPT
  {
    kind_ = symbol_kind::S_YYEMPTY;
  }

  void
  Parser::by_kind::move (by_kind& that)
  {
    kind_ = that.kind_;
    that.clear ();
  }

  Parser::symbol_kind_type
  Parser::by_kind::kind () const YY_NOEXCEPT
  {
    return kind_;
  }


  Parser::symbol_kind_type
  Parser::by_kind::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }



  // by_state.
  Parser::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  Parser::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  Parser::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  Parser::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  Parser::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  Parser::symbol_kind_type
  Parser::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  Parser::stack_symbol_type::stack_symbol_type ()
  {}

  Parser::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_INTEGER: // INTEGER
      case symbol_kind::S_DECIMAL: // DECIMAL
      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_IMPORT: // IMPORT
      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_SCOPE: // "::"
      case symbol_kind::S_TEMPLATE_OPEN: // "<|"
      case symbol_kind::S_TEMPLATE_CLOSE: // "|>"
      case symbol_kind::S_OROR: // "||"
      case symbol_kind::S_ANDAND: // "&&"
      case symbol_kind::S_XORXOR: // "^^"
      case symbol_kind::S_LE: // "<="
      case symbol_kind::S_EQEQ: // "=="
      case symbol_kind::S_GE: // ">="
      case symbol_kind::S_NE: // "!="
      case symbol_kind::S_CONSTRUCT_OPEN: // "<{"
      case symbol_kind::S_CONSTRUCT_CLOSE: // "}>"
      case symbol_kind::S_ARROW: // "->"
      case symbol_kind::S_FUNPTR: // "@?"
      case symbol_kind::S_RETURN: // "@<"
      case symbol_kind::S_CONTINUE: // "$^"
      case symbol_kind::S_BREAK: // "$!"
      case symbol_kind::S_SELF_FUNC: // "@$"
      case symbol_kind::S_SELF_CLASS: // "#$"
      case symbol_kind::S_PROT: // "^:"
      case symbol_kind::S_PRIV: // "~:"
      case symbol_kind::S_CONST_PUB: // "!:"
      case symbol_kind::S_CONST_PROT: // "!^:"
      case symbol_kind::S_CONST_PRIV: // "!~:"
      case symbol_kind::S_ALIAS_PUBLIC: // ":="
      case symbol_kind::S_ALIAS_PRIVATE: // "!:="
      case symbol_kind::S_OVERLOAD: // "^="
      case symbol_kind::S_35_: // '+'
      case symbol_kind::S_36_: // '-'
      case symbol_kind::S_37_: // '*'
      case symbol_kind::S_38_: // '/'
      case symbol_kind::S_39_: // '%'
      case symbol_kind::S_40_: // '^'
      case symbol_kind::S_41_: // '|'
      case symbol_kind::S_42_: // '&'
      case symbol_kind::S_43_: // '<'
      case symbol_kind::S_44_: // '>'
      case symbol_kind::S_45_: // ':'
      case symbol_kind::S_46_: // '='
      case symbol_kind::S_47_: // '?'
      case symbol_kind::S_48_: // '['
      case symbol_kind::S_49_: // ']'
      case symbol_kind::S_50_: // '('
      case symbol_kind::S_51_: // ')'
      case symbol_kind::S_52_: // '{'
      case symbol_kind::S_53_: // '}'
      case symbol_kind::S_54_: // '.'
      case symbol_kind::S_55_: // '`'
      case symbol_kind::S_56_: // '!'
      case symbol_kind::S_57_: // '@'
      case symbol_kind::S_58_: // '#'
      case symbol_kind::S_59_: // '$'
      case symbol_kind::S_60_: // '~'
      case symbol_kind::S_61_: // ','
      case symbol_kind::S_62_: // ';'
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_func_arg_declare: // func_arg_declare
      case symbol_kind::S_func_arg_declare_list: // func_arg_declare_list
      case symbol_kind::S_expr_list: // expr_list
      case symbol_kind::S_type: // type
      case symbol_kind::S_base_type: // base_type
      case symbol_kind::S_complex_type: // complex_type
      case symbol_kind::S_type_list: // type_list
      case symbol_kind::S_class: // class
      case symbol_kind::S_template_arg_declare_list: // template_arg_declare_list
      case symbol_kind::S_template_arg_list: // template_arg_list
      case symbol_kind::S_anonymous_template: // anonymous_template
      case symbol_kind::S_declare: // declare
      case symbol_kind::S_cast_stmt: // cast_stmt
      case symbol_kind::S_cast_stmt_list: // cast_stmt_list
      case symbol_kind::S_tuple: // tuple
      case symbol_kind::S_id_tuple_list: // id_tuple_list
      case symbol_kind::S_stmt: // stmt
      case symbol_kind::S_scope: // scope
      case symbol_kind::S_start: // start
        value.YY_MOVE_OR_COPY< at::AstPtr > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  Parser::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.location))
  {
    switch (that.kind ())
    {
      case symbol_kind::S_INTEGER: // INTEGER
      case symbol_kind::S_DECIMAL: // DECIMAL
      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_IMPORT: // IMPORT
      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_SCOPE: // "::"
      case symbol_kind::S_TEMPLATE_OPEN: // "<|"
      case symbol_kind::S_TEMPLATE_CLOSE: // "|>"
      case symbol_kind::S_OROR: // "||"
      case symbol_kind::S_ANDAND: // "&&"
      case symbol_kind::S_XORXOR: // "^^"
      case symbol_kind::S_LE: // "<="
      case symbol_kind::S_EQEQ: // "=="
      case symbol_kind::S_GE: // ">="
      case symbol_kind::S_NE: // "!="
      case symbol_kind::S_CONSTRUCT_OPEN: // "<{"
      case symbol_kind::S_CONSTRUCT_CLOSE: // "}>"
      case symbol_kind::S_ARROW: // "->"
      case symbol_kind::S_FUNPTR: // "@?"
      case symbol_kind::S_RETURN: // "@<"
      case symbol_kind::S_CONTINUE: // "$^"
      case symbol_kind::S_BREAK: // "$!"
      case symbol_kind::S_SELF_FUNC: // "@$"
      case symbol_kind::S_SELF_CLASS: // "#$"
      case symbol_kind::S_PROT: // "^:"
      case symbol_kind::S_PRIV: // "~:"
      case symbol_kind::S_CONST_PUB: // "!:"
      case symbol_kind::S_CONST_PROT: // "!^:"
      case symbol_kind::S_CONST_PRIV: // "!~:"
      case symbol_kind::S_ALIAS_PUBLIC: // ":="
      case symbol_kind::S_ALIAS_PRIVATE: // "!:="
      case symbol_kind::S_OVERLOAD: // "^="
      case symbol_kind::S_35_: // '+'
      case symbol_kind::S_36_: // '-'
      case symbol_kind::S_37_: // '*'
      case symbol_kind::S_38_: // '/'
      case symbol_kind::S_39_: // '%'
      case symbol_kind::S_40_: // '^'
      case symbol_kind::S_41_: // '|'
      case symbol_kind::S_42_: // '&'
      case symbol_kind::S_43_: // '<'
      case symbol_kind::S_44_: // '>'
      case symbol_kind::S_45_: // ':'
      case symbol_kind::S_46_: // '='
      case symbol_kind::S_47_: // '?'
      case symbol_kind::S_48_: // '['
      case symbol_kind::S_49_: // ']'
      case symbol_kind::S_50_: // '('
      case symbol_kind::S_51_: // ')'
      case symbol_kind::S_52_: // '{'
      case symbol_kind::S_53_: // '}'
      case symbol_kind::S_54_: // '.'
      case symbol_kind::S_55_: // '`'
      case symbol_kind::S_56_: // '!'
      case symbol_kind::S_57_: // '@'
      case symbol_kind::S_58_: // '#'
      case symbol_kind::S_59_: // '$'
      case symbol_kind::S_60_: // '~'
      case symbol_kind::S_61_: // ','
      case symbol_kind::S_62_: // ';'
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_func_arg_declare: // func_arg_declare
      case symbol_kind::S_func_arg_declare_list: // func_arg_declare_list
      case symbol_kind::S_expr_list: // expr_list
      case symbol_kind::S_type: // type
      case symbol_kind::S_base_type: // base_type
      case symbol_kind::S_complex_type: // complex_type
      case symbol_kind::S_type_list: // type_list
      case symbol_kind::S_class: // class
      case symbol_kind::S_template_arg_declare_list: // template_arg_declare_list
      case symbol_kind::S_template_arg_list: // template_arg_list
      case symbol_kind::S_anonymous_template: // anonymous_template
      case symbol_kind::S_declare: // declare
      case symbol_kind::S_cast_stmt: // cast_stmt
      case symbol_kind::S_cast_stmt_list: // cast_stmt_list
      case symbol_kind::S_tuple: // tuple
      case symbol_kind::S_id_tuple_list: // id_tuple_list
      case symbol_kind::S_stmt: // stmt
      case symbol_kind::S_scope: // scope
      case symbol_kind::S_start: // start
        value.move< at::AstPtr > (YY_MOVE (that.value));
        break;

      default:
        break;
    }

    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  Parser::stack_symbol_type&
  Parser::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_INTEGER: // INTEGER
      case symbol_kind::S_DECIMAL: // DECIMAL
      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_IMPORT: // IMPORT
      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_SCOPE: // "::"
      case symbol_kind::S_TEMPLATE_OPEN: // "<|"
      case symbol_kind::S_TEMPLATE_CLOSE: // "|>"
      case symbol_kind::S_OROR: // "||"
      case symbol_kind::S_ANDAND: // "&&"
      case symbol_kind::S_XORXOR: // "^^"
      case symbol_kind::S_LE: // "<="
      case symbol_kind::S_EQEQ: // "=="
      case symbol_kind::S_GE: // ">="
      case symbol_kind::S_NE: // "!="
      case symbol_kind::S_CONSTRUCT_OPEN: // "<{"
      case symbol_kind::S_CONSTRUCT_CLOSE: // "}>"
      case symbol_kind::S_ARROW: // "->"
      case symbol_kind::S_FUNPTR: // "@?"
      case symbol_kind::S_RETURN: // "@<"
      case symbol_kind::S_CONTINUE: // "$^"
      case symbol_kind::S_BREAK: // "$!"
      case symbol_kind::S_SELF_FUNC: // "@$"
      case symbol_kind::S_SELF_CLASS: // "#$"
      case symbol_kind::S_PROT: // "^:"
      case symbol_kind::S_PRIV: // "~:"
      case symbol_kind::S_CONST_PUB: // "!:"
      case symbol_kind::S_CONST_PROT: // "!^:"
      case symbol_kind::S_CONST_PRIV: // "!~:"
      case symbol_kind::S_ALIAS_PUBLIC: // ":="
      case symbol_kind::S_ALIAS_PRIVATE: // "!:="
      case symbol_kind::S_OVERLOAD: // "^="
      case symbol_kind::S_35_: // '+'
      case symbol_kind::S_36_: // '-'
      case symbol_kind::S_37_: // '*'
      case symbol_kind::S_38_: // '/'
      case symbol_kind::S_39_: // '%'
      case symbol_kind::S_40_: // '^'
      case symbol_kind::S_41_: // '|'
      case symbol_kind::S_42_: // '&'
      case symbol_kind::S_43_: // '<'
      case symbol_kind::S_44_: // '>'
      case symbol_kind::S_45_: // ':'
      case symbol_kind::S_46_: // '='
      case symbol_kind::S_47_: // '?'
      case symbol_kind::S_48_: // '['
      case symbol_kind::S_49_: // ']'
      case symbol_kind::S_50_: // '('
      case symbol_kind::S_51_: // ')'
      case symbol_kind::S_52_: // '{'
      case symbol_kind::S_53_: // '}'
      case symbol_kind::S_54_: // '.'
      case symbol_kind::S_55_: // '`'
      case symbol_kind::S_56_: // '!'
      case symbol_kind::S_57_: // '@'
      case symbol_kind::S_58_: // '#'
      case symbol_kind::S_59_: // '$'
      case symbol_kind::S_60_: // '~'
      case symbol_kind::S_61_: // ','
      case symbol_kind::S_62_: // ';'
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_func_arg_declare: // func_arg_declare
      case symbol_kind::S_func_arg_declare_list: // func_arg_declare_list
      case symbol_kind::S_expr_list: // expr_list
      case symbol_kind::S_type: // type
      case symbol_kind::S_base_type: // base_type
      case symbol_kind::S_complex_type: // complex_type
      case symbol_kind::S_type_list: // type_list
      case symbol_kind::S_class: // class
      case symbol_kind::S_template_arg_declare_list: // template_arg_declare_list
      case symbol_kind::S_template_arg_list: // template_arg_list
      case symbol_kind::S_anonymous_template: // anonymous_template
      case symbol_kind::S_declare: // declare
      case symbol_kind::S_cast_stmt: // cast_stmt
      case symbol_kind::S_cast_stmt_list: // cast_stmt_list
      case symbol_kind::S_tuple: // tuple
      case symbol_kind::S_id_tuple_list: // id_tuple_list
      case symbol_kind::S_stmt: // stmt
      case symbol_kind::S_scope: // scope
      case symbol_kind::S_start: // start
        value.copy< at::AstPtr > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    return *this;
  }

  Parser::stack_symbol_type&
  Parser::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    switch (that.kind ())
    {
      case symbol_kind::S_INTEGER: // INTEGER
      case symbol_kind::S_DECIMAL: // DECIMAL
      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_IMPORT: // IMPORT
      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_SCOPE: // "::"
      case symbol_kind::S_TEMPLATE_OPEN: // "<|"
      case symbol_kind::S_TEMPLATE_CLOSE: // "|>"
      case symbol_kind::S_OROR: // "||"
      case symbol_kind::S_ANDAND: // "&&"
      case symbol_kind::S_XORXOR: // "^^"
      case symbol_kind::S_LE: // "<="
      case symbol_kind::S_EQEQ: // "=="
      case symbol_kind::S_GE: // ">="
      case symbol_kind::S_NE: // "!="
      case symbol_kind::S_CONSTRUCT_OPEN: // "<{"
      case symbol_kind::S_CONSTRUCT_CLOSE: // "}>"
      case symbol_kind::S_ARROW: // "->"
      case symbol_kind::S_FUNPTR: // "@?"
      case symbol_kind::S_RETURN: // "@<"
      case symbol_kind::S_CONTINUE: // "$^"
      case symbol_kind::S_BREAK: // "$!"
      case symbol_kind::S_SELF_FUNC: // "@$"
      case symbol_kind::S_SELF_CLASS: // "#$"
      case symbol_kind::S_PROT: // "^:"
      case symbol_kind::S_PRIV: // "~:"
      case symbol_kind::S_CONST_PUB: // "!:"
      case symbol_kind::S_CONST_PROT: // "!^:"
      case symbol_kind::S_CONST_PRIV: // "!~:"
      case symbol_kind::S_ALIAS_PUBLIC: // ":="
      case symbol_kind::S_ALIAS_PRIVATE: // "!:="
      case symbol_kind::S_OVERLOAD: // "^="
      case symbol_kind::S_35_: // '+'
      case symbol_kind::S_36_: // '-'
      case symbol_kind::S_37_: // '*'
      case symbol_kind::S_38_: // '/'
      case symbol_kind::S_39_: // '%'
      case symbol_kind::S_40_: // '^'
      case symbol_kind::S_41_: // '|'
      case symbol_kind::S_42_: // '&'
      case symbol_kind::S_43_: // '<'
      case symbol_kind::S_44_: // '>'
      case symbol_kind::S_45_: // ':'
      case symbol_kind::S_46_: // '='
      case symbol_kind::S_47_: // '?'
      case symbol_kind::S_48_: // '['
      case symbol_kind::S_49_: // ']'
      case symbol_kind::S_50_: // '('
      case symbol_kind::S_51_: // ')'
      case symbol_kind::S_52_: // '{'
      case symbol_kind::S_53_: // '}'
      case symbol_kind::S_54_: // '.'
      case symbol_kind::S_55_: // '`'
      case symbol_kind::S_56_: // '!'
      case symbol_kind::S_57_: // '@'
      case symbol_kind::S_58_: // '#'
      case symbol_kind::S_59_: // '$'
      case symbol_kind::S_60_: // '~'
      case symbol_kind::S_61_: // ','
      case symbol_kind::S_62_: // ';'
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_func_arg_declare: // func_arg_declare
      case symbol_kind::S_func_arg_declare_list: // func_arg_declare_list
      case symbol_kind::S_expr_list: // expr_list
      case symbol_kind::S_type: // type
      case symbol_kind::S_base_type: // base_type
      case symbol_kind::S_complex_type: // complex_type
      case symbol_kind::S_type_list: // type_list
      case symbol_kind::S_class: // class
      case symbol_kind::S_template_arg_declare_list: // template_arg_declare_list
      case symbol_kind::S_template_arg_list: // template_arg_list
      case symbol_kind::S_anonymous_template: // anonymous_template
      case symbol_kind::S_declare: // declare
      case symbol_kind::S_cast_stmt: // cast_stmt
      case symbol_kind::S_cast_stmt_list: // cast_stmt_list
      case symbol_kind::S_tuple: // tuple
      case symbol_kind::S_id_tuple_list: // id_tuple_list
      case symbol_kind::S_stmt: // stmt
      case symbol_kind::S_scope: // scope
      case symbol_kind::S_start: // start
        value.move< at::AstPtr > (that.value);
        break;

      default:
        break;
    }

    location = that.location;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  Parser::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);
  }

#if YYDEBUG
  template <typename Base>
  void
  Parser::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YY_USE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " ("
            << yysym.location << ": ";
        YY_USE (yykind);
        yyo << ')';
      }
  }
#endif

  void
  Parser::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  Parser::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  Parser::yypop_ (int n) YY_NOEXCEPT
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  Parser::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  Parser::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  Parser::debug_level_type
  Parser::debug_level () const
  {
    return yydebug_;
  }

  void
  Parser::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  Parser::state_type
  Parser::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  Parser::yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  Parser::yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yytable_ninf_;
  }

  int
  Parser::operator() ()
  {
    return parse ();
  }

  int
  Parser::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The locations where the error started and ended.
    stack_symbol_type yyerror_range[3];

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            yyla.kind_ = yytranslate_ (yylex (&yyla.value, &yyla.location));
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* Variants are always initialized to an empty instance of the
         correct type. The default '$$ = $1' action is NOT applied
         when using variants.  */
      switch (yyr1_[yyn])
    {
      case symbol_kind::S_INTEGER: // INTEGER
      case symbol_kind::S_DECIMAL: // DECIMAL
      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_IMPORT: // IMPORT
      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_SCOPE: // "::"
      case symbol_kind::S_TEMPLATE_OPEN: // "<|"
      case symbol_kind::S_TEMPLATE_CLOSE: // "|>"
      case symbol_kind::S_OROR: // "||"
      case symbol_kind::S_ANDAND: // "&&"
      case symbol_kind::S_XORXOR: // "^^"
      case symbol_kind::S_LE: // "<="
      case symbol_kind::S_EQEQ: // "=="
      case symbol_kind::S_GE: // ">="
      case symbol_kind::S_NE: // "!="
      case symbol_kind::S_CONSTRUCT_OPEN: // "<{"
      case symbol_kind::S_CONSTRUCT_CLOSE: // "}>"
      case symbol_kind::S_ARROW: // "->"
      case symbol_kind::S_FUNPTR: // "@?"
      case symbol_kind::S_RETURN: // "@<"
      case symbol_kind::S_CONTINUE: // "$^"
      case symbol_kind::S_BREAK: // "$!"
      case symbol_kind::S_SELF_FUNC: // "@$"
      case symbol_kind::S_SELF_CLASS: // "#$"
      case symbol_kind::S_PROT: // "^:"
      case symbol_kind::S_PRIV: // "~:"
      case symbol_kind::S_CONST_PUB: // "!:"
      case symbol_kind::S_CONST_PROT: // "!^:"
      case symbol_kind::S_CONST_PRIV: // "!~:"
      case symbol_kind::S_ALIAS_PUBLIC: // ":="
      case symbol_kind::S_ALIAS_PRIVATE: // "!:="
      case symbol_kind::S_OVERLOAD: // "^="
      case symbol_kind::S_35_: // '+'
      case symbol_kind::S_36_: // '-'
      case symbol_kind::S_37_: // '*'
      case symbol_kind::S_38_: // '/'
      case symbol_kind::S_39_: // '%'
      case symbol_kind::S_40_: // '^'
      case symbol_kind::S_41_: // '|'
      case symbol_kind::S_42_: // '&'
      case symbol_kind::S_43_: // '<'
      case symbol_kind::S_44_: // '>'
      case symbol_kind::S_45_: // ':'
      case symbol_kind::S_46_: // '='
      case symbol_kind::S_47_: // '?'
      case symbol_kind::S_48_: // '['
      case symbol_kind::S_49_: // ']'
      case symbol_kind::S_50_: // '('
      case symbol_kind::S_51_: // ')'
      case symbol_kind::S_52_: // '{'
      case symbol_kind::S_53_: // '}'
      case symbol_kind::S_54_: // '.'
      case symbol_kind::S_55_: // '`'
      case symbol_kind::S_56_: // '!'
      case symbol_kind::S_57_: // '@'
      case symbol_kind::S_58_: // '#'
      case symbol_kind::S_59_: // '$'
      case symbol_kind::S_60_: // '~'
      case symbol_kind::S_61_: // ','
      case symbol_kind::S_62_: // ';'
      case symbol_kind::S_expr: // expr
      case symbol_kind::S_func_arg_declare: // func_arg_declare
      case symbol_kind::S_func_arg_declare_list: // func_arg_declare_list
      case symbol_kind::S_expr_list: // expr_list
      case symbol_kind::S_type: // type
      case symbol_kind::S_base_type: // base_type
      case symbol_kind::S_complex_type: // complex_type
      case symbol_kind::S_type_list: // type_list
      case symbol_kind::S_class: // class
      case symbol_kind::S_template_arg_declare_list: // template_arg_declare_list
      case symbol_kind::S_template_arg_list: // template_arg_list
      case symbol_kind::S_anonymous_template: // anonymous_template
      case symbol_kind::S_declare: // declare
      case symbol_kind::S_cast_stmt: // cast_stmt
      case symbol_kind::S_cast_stmt_list: // cast_stmt_list
      case symbol_kind::S_tuple: // tuple
      case symbol_kind::S_id_tuple_list: // id_tuple_list
      case symbol_kind::S_stmt: // stmt
      case symbol_kind::S_scope: // scope
      case symbol_kind::S_start: // start
        yylhs.value.emplace< at::AstPtr > ();
        break;

      default:
        break;
    }


      // Default location.
      {
        stack_type::slice range (yystack_, yylen);
        YYLLOC_DEFAULT (yylhs.location, range, yylen);
        yyerror_range[1].location = yylhs.location;
      }

      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2: // expr: INTEGER
#line 82 "src/syntax.ast.y"
              { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1206 "src/generated/ast_parser.cpp"
    break;

  case 3: // expr: "@$"
#line 83 "src/syntax.ast.y"
                { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1212 "src/generated/ast_parser.cpp"
    break;

  case 4: // expr: "#$"
#line 84 "src/syntax.ast.y"
                 { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1218 "src/generated/ast_parser.cpp"
    break;

  case 5: // expr: DECIMAL
#line 85 "src/syntax.ast.y"
              { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1224 "src/generated/ast_parser.cpp"
    break;

  case 6: // expr: STRING
#line 86 "src/syntax.ast.y"
             { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1230 "src/generated/ast_parser.cpp"
    break;

  case 7: // expr: IDENTIFIER
#line 87 "src/syntax.ast.y"
                 { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1236 "src/generated/ast_parser.cpp"
    break;

  case 8: // expr: IMPORT
#line 88 "src/syntax.ast.y"
             { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1242 "src/generated/ast_parser.cpp"
    break;

  case 9: // expr: expr '+' expr
#line 89 "src/syntax.ast.y"
                    { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1248 "src/generated/ast_parser.cpp"
    break;

  case 10: // expr: expr '-' expr
#line 90 "src/syntax.ast.y"
                    { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1254 "src/generated/ast_parser.cpp"
    break;

  case 11: // expr: expr '*' expr
#line 91 "src/syntax.ast.y"
                    { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1260 "src/generated/ast_parser.cpp"
    break;

  case 12: // expr: expr '/' expr
#line 92 "src/syntax.ast.y"
                    { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1266 "src/generated/ast_parser.cpp"
    break;

  case 13: // expr: expr '%' expr
#line 93 "src/syntax.ast.y"
                    { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1272 "src/generated/ast_parser.cpp"
    break;

  case 14: // expr: expr '^' expr
#line 94 "src/syntax.ast.y"
                    { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1278 "src/generated/ast_parser.cpp"
    break;

  case 15: // expr: expr '|' expr
#line 95 "src/syntax.ast.y"
                    { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1284 "src/generated/ast_parser.cpp"
    break;

  case 16: // expr: expr '&' expr
#line 96 "src/syntax.ast.y"
                    { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1290 "src/generated/ast_parser.cpp"
    break;

  case 17: // expr: expr '?' expr ':' expr
#line 97 "src/syntax.ast.y"
                             { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[4].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[4].value.as < at::AstPtr > ()); if (yystack_[3].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[3].value.as < at::AstPtr > ()); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1296 "src/generated/ast_parser.cpp"
    break;

  case 18: // expr: expr '<' expr
#line 98 "src/syntax.ast.y"
                    { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1302 "src/generated/ast_parser.cpp"
    break;

  case 19: // expr: expr '>' expr
#line 99 "src/syntax.ast.y"
                    { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1308 "src/generated/ast_parser.cpp"
    break;

  case 20: // expr: expr "<=" expr
#line 100 "src/syntax.ast.y"
                     { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1314 "src/generated/ast_parser.cpp"
    break;

  case 21: // expr: expr ">=" expr
#line 101 "src/syntax.ast.y"
                     { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1320 "src/generated/ast_parser.cpp"
    break;

  case 22: // expr: expr "==" expr
#line 102 "src/syntax.ast.y"
                     { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1326 "src/generated/ast_parser.cpp"
    break;

  case 23: // expr: expr "!=" expr
#line 103 "src/syntax.ast.y"
                     { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1332 "src/generated/ast_parser.cpp"
    break;

  case 24: // expr: '-' expr
#line 104 "src/syntax.ast.y"
                           { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1338 "src/generated/ast_parser.cpp"
    break;

  case 25: // expr: '!' expr
#line 105 "src/syntax.ast.y"
                           { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1344 "src/generated/ast_parser.cpp"
    break;

  case 26: // expr: expr "||" expr
#line 106 "src/syntax.ast.y"
                     { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1350 "src/generated/ast_parser.cpp"
    break;

  case 27: // expr: expr "&&" expr
#line 107 "src/syntax.ast.y"
                     { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1356 "src/generated/ast_parser.cpp"
    break;

  case 28: // expr: expr "^^" expr
#line 108 "src/syntax.ast.y"
                     { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1362 "src/generated/ast_parser.cpp"
    break;

  case 29: // expr: '(' expr ')'
#line 109 "src/syntax.ast.y"
                   { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1368 "src/generated/ast_parser.cpp"
    break;

  case 30: // expr: expr '.' IDENTIFIER
#line 110 "src/syntax.ast.y"
                          { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1374 "src/generated/ast_parser.cpp"
    break;

  case 31: // expr: expr '(' expr_list ')'
#line 111 "src/syntax.ast.y"
                             { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[3].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[3].value.as < at::AstPtr > ()); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1380 "src/generated/ast_parser.cpp"
    break;

  case 32: // expr: expr '[' expr_list ']'
#line 112 "src/syntax.ast.y"
                             { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[3].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[3].value.as < at::AstPtr > ()); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1386 "src/generated/ast_parser.cpp"
    break;

  case 33: // expr: '[' expr_list ']'
#line 113 "src/syntax.ast.y"
                        { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1392 "src/generated/ast_parser.cpp"
    break;

  case 34: // expr: IDENTIFIER "<{" expr_list "}>"
#line 114 "src/syntax.ast.y"
                                     { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[3].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[3].value.as < at::AstPtr > ()); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1398 "src/generated/ast_parser.cpp"
    break;

  case 35: // expr: complex_type "<{" expr_list "}>"
#line 115 "src/syntax.ast.y"
                                       { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[3].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[3].value.as < at::AstPtr > ()); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1404 "src/generated/ast_parser.cpp"
    break;

  case 36: // expr: '@' '(' func_arg_declare_list ')' '{' scope '}'
#line 116 "src/syntax.ast.y"
                                                      { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[6].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[6].value.as < at::AstPtr > ()); if (yystack_[5].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[5].value.as < at::AstPtr > ()); if (yystack_[4].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[4].value.as < at::AstPtr > ()); if (yystack_[3].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[3].value.as < at::AstPtr > ()); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1410 "src/generated/ast_parser.cpp"
    break;

  case 37: // expr: '@' '(' func_arg_declare_list ')' "->" '(' type ')' '{' scope '}'
#line 117 "src/syntax.ast.y"
                                                                        { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[10].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[10].value.as < at::AstPtr > ()); if (yystack_[9].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[9].value.as < at::AstPtr > ()); if (yystack_[8].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[8].value.as < at::AstPtr > ()); if (yystack_[7].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[7].value.as < at::AstPtr > ()); if (yystack_[6].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[6].value.as < at::AstPtr > ()); if (yystack_[5].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[5].value.as < at::AstPtr > ()); if (yystack_[4].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[4].value.as < at::AstPtr > ()); if (yystack_[3].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[3].value.as < at::AstPtr > ()); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1416 "src/generated/ast_parser.cpp"
    break;

  case 38: // expr: '&' expr
#line 118 "src/syntax.ast.y"
               { yylhs.value.as < at::AstPtr > () = at::makeNode("expr", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1422 "src/generated/ast_parser.cpp"
    break;

  case 39: // func_arg_declare: IDENTIFIER
#line 120 "src/syntax.ast.y"
                             { yylhs.value.as < at::AstPtr > () = at::makeNode("func_arg_declare", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1428 "src/generated/ast_parser.cpp"
    break;

  case 40: // func_arg_declare: IDENTIFIER '=' expr
#line 121 "src/syntax.ast.y"
                                      { yylhs.value.as < at::AstPtr > () = at::makeNode("func_arg_declare", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1434 "src/generated/ast_parser.cpp"
    break;

  case 41: // func_arg_declare: IDENTIFIER ':' type
#line 122 "src/syntax.ast.y"
                                      { yylhs.value.as < at::AstPtr > () = at::makeNode("func_arg_declare", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1440 "src/generated/ast_parser.cpp"
    break;

  case 42: // func_arg_declare: IDENTIFIER ':' type '=' expr
#line 123 "src/syntax.ast.y"
                                               { yylhs.value.as < at::AstPtr > () = at::makeNode("func_arg_declare", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[4].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[4].value.as < at::AstPtr > ()); if (yystack_[3].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[3].value.as < at::AstPtr > ()); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1446 "src/generated/ast_parser.cpp"
    break;

  case 43: // func_arg_declare_list: %empty
#line 126 "src/syntax.ast.y"
                              { yylhs.value.as < at::AstPtr > () = at::makeNode("func_arg_declare_list_empty", "", {yylhs.location.begin.line, yylhs.location.begin.column}); }
#line 1452 "src/generated/ast_parser.cpp"
    break;

  case 44: // func_arg_declare_list: func_arg_declare
#line 129 "src/syntax.ast.y"
                                        { yylhs.value.as < at::AstPtr > () = at::makeNode("func_arg_declare_list", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1458 "src/generated/ast_parser.cpp"
    break;

  case 45: // func_arg_declare_list: func_arg_declare_list ',' func_arg_declare
#line 130 "src/syntax.ast.y"
                                                                  { yylhs.value.as < at::AstPtr > () = at::makeNode("func_arg_declare_list", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1464 "src/generated/ast_parser.cpp"
    break;

  case 46: // expr_list: %empty
#line 132 "src/syntax.ast.y"
                  { yylhs.value.as < at::AstPtr > () = at::makeNode("expr_list_empty", "", {yylhs.location.begin.line, yylhs.location.begin.column}); }
#line 1470 "src/generated/ast_parser.cpp"
    break;

  case 47: // expr_list: expr
#line 133 "src/syntax.ast.y"
                { yylhs.value.as < at::AstPtr > () = at::makeNode("expr_list", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1476 "src/generated/ast_parser.cpp"
    break;

  case 48: // expr_list: expr_list ',' expr
#line 134 "src/syntax.ast.y"
                              { yylhs.value.as < at::AstPtr > () = at::makeNode("expr_list", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1482 "src/generated/ast_parser.cpp"
    break;

  case 49: // type: base_type
#line 136 "src/syntax.ast.y"
                { yylhs.value.as < at::AstPtr > () = at::makeNode("type", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1488 "src/generated/ast_parser.cpp"
    break;

  case 50: // type: type "::" IDENTIFIER
#line 137 "src/syntax.ast.y"
                           { yylhs.value.as < at::AstPtr > () = at::makeNode("type", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1494 "src/generated/ast_parser.cpp"
    break;

  case 51: // base_type: IDENTIFIER
#line 139 "src/syntax.ast.y"
                                        { yylhs.value.as < at::AstPtr > () = at::makeNode("base_type", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1500 "src/generated/ast_parser.cpp"
    break;

  case 52: // base_type: complex_type
#line 140 "src/syntax.ast.y"
                                          { yylhs.value.as < at::AstPtr > () = at::makeNode("base_type", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1506 "src/generated/ast_parser.cpp"
    break;

  case 53: // complex_type: '`' expr '`'
#line 142 "src/syntax.ast.y"
                           { yylhs.value.as < at::AstPtr > () = at::makeNode("complex_type", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1512 "src/generated/ast_parser.cpp"
    break;

  case 54: // complex_type: IDENTIFIER "<|" template_arg_list "|>"
#line 143 "src/syntax.ast.y"
                                                     { yylhs.value.as < at::AstPtr > () = at::makeNode("complex_type", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[3].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[3].value.as < at::AstPtr > ()); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1518 "src/generated/ast_parser.cpp"
    break;

  case 55: // complex_type: anonymous_template "<|" template_arg_list "|>"
#line 144 "src/syntax.ast.y"
                                                             { yylhs.value.as < at::AstPtr > () = at::makeNode("complex_type", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[3].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[3].value.as < at::AstPtr > ()); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1524 "src/generated/ast_parser.cpp"
    break;

  case 56: // complex_type: "@?" '(' func_arg_declare_list ')'
#line 145 "src/syntax.ast.y"
                                                 { yylhs.value.as < at::AstPtr > () = at::makeNode("complex_type", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[3].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[3].value.as < at::AstPtr > ()); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1530 "src/generated/ast_parser.cpp"
    break;

  case 57: // complex_type: "@?" '(' func_arg_declare_list ')' "->" '(' type ')'
#line 146 "src/syntax.ast.y"
                                                                   { yylhs.value.as < at::AstPtr > () = at::makeNode("complex_type", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[7].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[7].value.as < at::AstPtr > ()); if (yystack_[6].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[6].value.as < at::AstPtr > ()); if (yystack_[5].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[5].value.as < at::AstPtr > ()); if (yystack_[4].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[4].value.as < at::AstPtr > ()); if (yystack_[3].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[3].value.as < at::AstPtr > ()); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1536 "src/generated/ast_parser.cpp"
    break;

  case 58: // complex_type: '*' type
#line 147 "src/syntax.ast.y"
                       { yylhs.value.as < at::AstPtr > () = at::makeNode("complex_type", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1542 "src/generated/ast_parser.cpp"
    break;

  case 59: // complex_type: class
#line 148 "src/syntax.ast.y"
                    { yylhs.value.as < at::AstPtr > () = at::makeNode("complex_type", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1548 "src/generated/ast_parser.cpp"
    break;

  case 60: // type_list: type
#line 150 "src/syntax.ast.y"
                { yylhs.value.as < at::AstPtr > () = at::makeNode("type_list", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1554 "src/generated/ast_parser.cpp"
    break;

  case 61: // type_list: type_list ',' type
#line 151 "src/syntax.ast.y"
                              { yylhs.value.as < at::AstPtr > () = at::makeNode("type_list", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1560 "src/generated/ast_parser.cpp"
    break;

  case 62: // class: '#' '(' func_arg_declare_list ')' '{' scope '}'
#line 153 "src/syntax.ast.y"
                                                       { yylhs.value.as < at::AstPtr > () = at::makeNode("class", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[6].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[6].value.as < at::AstPtr > ()); if (yystack_[5].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[5].value.as < at::AstPtr > ()); if (yystack_[4].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[4].value.as < at::AstPtr > ()); if (yystack_[3].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[3].value.as < at::AstPtr > ()); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1566 "src/generated/ast_parser.cpp"
    break;

  case 63: // class: '#' '(' func_arg_declare_list ')' ':' type_list '{' scope '}'
#line 154 "src/syntax.ast.y"
                                                                     { yylhs.value.as < at::AstPtr > () = at::makeNode("class", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[8].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[8].value.as < at::AstPtr > ()); if (yystack_[7].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[7].value.as < at::AstPtr > ()); if (yystack_[6].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[6].value.as < at::AstPtr > ()); if (yystack_[5].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[5].value.as < at::AstPtr > ()); if (yystack_[4].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[4].value.as < at::AstPtr > ()); if (yystack_[3].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[3].value.as < at::AstPtr > ()); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1572 "src/generated/ast_parser.cpp"
    break;

  case 64: // template_arg_declare_list: IDENTIFIER
#line 156 "src/syntax.ast.y"
                                      { yylhs.value.as < at::AstPtr > () = at::makeNode("template_arg_declare_list", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1578 "src/generated/ast_parser.cpp"
    break;

  case 65: // template_arg_declare_list: template_arg_declare_list ',' IDENTIFIER
#line 157 "src/syntax.ast.y"
                                                                    { yylhs.value.as < at::AstPtr > () = at::makeNode("template_arg_declare_list", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1584 "src/generated/ast_parser.cpp"
    break;

  case 66: // template_arg_list: type
#line 159 "src/syntax.ast.y"
                        { yylhs.value.as < at::AstPtr > () = at::makeNode("template_arg_list", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1590 "src/generated/ast_parser.cpp"
    break;

  case 67: // template_arg_list: template_arg_list ',' type
#line 160 "src/syntax.ast.y"
                                              { yylhs.value.as < at::AstPtr > () = at::makeNode("template_arg_list", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1596 "src/generated/ast_parser.cpp"
    break;

  case 68: // anonymous_template: "<|" template_arg_declare_list "|>" "@?" '(' func_arg_declare_list ')'
#line 163 "src/syntax.ast.y"
                                                                                           { yylhs.value.as < at::AstPtr > () = at::makeNode("anonymous_template", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[6].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[6].value.as < at::AstPtr > ()); if (yystack_[5].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[5].value.as < at::AstPtr > ()); if (yystack_[4].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[4].value.as < at::AstPtr > ()); if (yystack_[3].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[3].value.as < at::AstPtr > ()); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1602 "src/generated/ast_parser.cpp"
    break;

  case 69: // anonymous_template: "<|" template_arg_declare_list "|>" "@?" '(' func_arg_declare_list ')' "->" type
#line 164 "src/syntax.ast.y"
                                                                                                     { yylhs.value.as < at::AstPtr > () = at::makeNode("anonymous_template", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[8].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[8].value.as < at::AstPtr > ()); if (yystack_[7].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[7].value.as < at::AstPtr > ()); if (yystack_[6].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[6].value.as < at::AstPtr > ()); if (yystack_[5].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[5].value.as < at::AstPtr > ()); if (yystack_[4].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[4].value.as < at::AstPtr > ()); if (yystack_[3].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[3].value.as < at::AstPtr > ()); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1608 "src/generated/ast_parser.cpp"
    break;

  case 70: // anonymous_template: "<|" template_arg_declare_list "|>" class
#line 165 "src/syntax.ast.y"
                                                              { yylhs.value.as < at::AstPtr > () = at::makeNode("anonymous_template", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[3].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[3].value.as < at::AstPtr > ()); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1614 "src/generated/ast_parser.cpp"
    break;

  case 71: // anonymous_template: "<|" template_arg_declare_list "|>" '@' '(' func_arg_declare_list ')' '{' scope '}'
#line 166 "src/syntax.ast.y"
                                                                                                        { yylhs.value.as < at::AstPtr > () = at::makeNode("anonymous_template", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[9].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[9].value.as < at::AstPtr > ()); if (yystack_[8].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[8].value.as < at::AstPtr > ()); if (yystack_[7].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[7].value.as < at::AstPtr > ()); if (yystack_[6].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[6].value.as < at::AstPtr > ()); if (yystack_[5].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[5].value.as < at::AstPtr > ()); if (yystack_[4].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[4].value.as < at::AstPtr > ()); if (yystack_[3].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[3].value.as < at::AstPtr > ()); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1620 "src/generated/ast_parser.cpp"
    break;

  case 72: // anonymous_template: "<|" template_arg_declare_list "|>" '@' '(' func_arg_declare_list ')' "->" '(' type ')' '{' scope '}'
#line 167 "src/syntax.ast.y"
                                                                                                                          { yylhs.value.as < at::AstPtr > () = at::makeNode("anonymous_template", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[13].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[13].value.as < at::AstPtr > ()); if (yystack_[12].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[12].value.as < at::AstPtr > ()); if (yystack_[11].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[11].value.as < at::AstPtr > ()); if (yystack_[10].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[10].value.as < at::AstPtr > ()); if (yystack_[9].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[9].value.as < at::AstPtr > ()); if (yystack_[8].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[8].value.as < at::AstPtr > ()); if (yystack_[7].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[7].value.as < at::AstPtr > ()); if (yystack_[6].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[6].value.as < at::AstPtr > ()); if (yystack_[5].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[5].value.as < at::AstPtr > ()); if (yystack_[4].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[4].value.as < at::AstPtr > ()); if (yystack_[3].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[3].value.as < at::AstPtr > ()); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1626 "src/generated/ast_parser.cpp"
    break;

  case 73: // declare: ':'
#line 169 "src/syntax.ast.y"
             { yylhs.value.as < at::AstPtr > () = at::makeNode("declare", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1632 "src/generated/ast_parser.cpp"
    break;

  case 74: // declare: "^:"
#line 170 "src/syntax.ast.y"
              { yylhs.value.as < at::AstPtr > () = at::makeNode("declare", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1638 "src/generated/ast_parser.cpp"
    break;

  case 75: // declare: "~:"
#line 171 "src/syntax.ast.y"
              { yylhs.value.as < at::AstPtr > () = at::makeNode("declare", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1644 "src/generated/ast_parser.cpp"
    break;

  case 76: // declare: "!:"
#line 172 "src/syntax.ast.y"
              { yylhs.value.as < at::AstPtr > () = at::makeNode("declare", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1650 "src/generated/ast_parser.cpp"
    break;

  case 77: // declare: "!^:"
#line 173 "src/syntax.ast.y"
               { yylhs.value.as < at::AstPtr > () = at::makeNode("declare", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1656 "src/generated/ast_parser.cpp"
    break;

  case 78: // declare: "!~:"
#line 174 "src/syntax.ast.y"
               { yylhs.value.as < at::AstPtr > () = at::makeNode("declare", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1662 "src/generated/ast_parser.cpp"
    break;

  case 79: // cast_stmt: expr "->" type
#line 176 "src/syntax.ast.y"
                          { yylhs.value.as < at::AstPtr > () = at::makeNode("cast_stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1668 "src/generated/ast_parser.cpp"
    break;

  case 80: // cast_stmt: IDENTIFIER ':' expr "->" type
#line 177 "src/syntax.ast.y"
                                         { yylhs.value.as < at::AstPtr > () = at::makeNode("cast_stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[4].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[4].value.as < at::AstPtr > ()); if (yystack_[3].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[3].value.as < at::AstPtr > ()); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1674 "src/generated/ast_parser.cpp"
    break;

  case 81: // cast_stmt_list: cast_stmt
#line 179 "src/syntax.ast.y"
                          { yylhs.value.as < at::AstPtr > () = at::makeNode("cast_stmt_list", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1680 "src/generated/ast_parser.cpp"
    break;

  case 82: // cast_stmt_list: cast_stmt_list ',' cast_stmt
#line 180 "src/syntax.ast.y"
                                             { yylhs.value.as < at::AstPtr > () = at::makeNode("cast_stmt_list", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1686 "src/generated/ast_parser.cpp"
    break;

  case 83: // tuple: expr ',' expr
#line 184 "src/syntax.ast.y"
                     { yylhs.value.as < at::AstPtr > () = at::makeNode("tuple", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1692 "src/generated/ast_parser.cpp"
    break;

  case 84: // tuple: tuple ',' expr
#line 185 "src/syntax.ast.y"
                      { yylhs.value.as < at::AstPtr > () = at::makeNode("tuple", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1698 "src/generated/ast_parser.cpp"
    break;

  case 85: // id_tuple_list: IDENTIFIER ',' IDENTIFIER
#line 187 "src/syntax.ast.y"
                                         { yylhs.value.as < at::AstPtr > () = at::makeNode("id_tuple_list", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1704 "src/generated/ast_parser.cpp"
    break;

  case 86: // id_tuple_list: id_tuple_list ',' IDENTIFIER
#line 188 "src/syntax.ast.y"
                                            { yylhs.value.as < at::AstPtr > () = at::makeNode("id_tuple_list", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1710 "src/generated/ast_parser.cpp"
    break;

  case 87: // stmt: %empty
#line 190 "src/syntax.ast.y"
             { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt_empty", "", {yylhs.location.begin.line, yylhs.location.begin.column}); }
#line 1716 "src/generated/ast_parser.cpp"
    break;

  case 88: // stmt: "@<" expr
#line 191 "src/syntax.ast.y"
                { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1722 "src/generated/ast_parser.cpp"
    break;

  case 89: // stmt: "@<"
#line 192 "src/syntax.ast.y"
           { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1728 "src/generated/ast_parser.cpp"
    break;

  case 90: // stmt: "$^"
#line 193 "src/syntax.ast.y"
           { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1734 "src/generated/ast_parser.cpp"
    break;

  case 91: // stmt: "$!"
#line 194 "src/syntax.ast.y"
           { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1740 "src/generated/ast_parser.cpp"
    break;

  case 92: // stmt: expr
#line 195 "src/syntax.ast.y"
           { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1746 "src/generated/ast_parser.cpp"
    break;

  case 93: // stmt: expr '=' expr
#line 196 "src/syntax.ast.y"
                    { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1752 "src/generated/ast_parser.cpp"
    break;

  case 94: // stmt: expr "^=" expr
#line 197 "src/syntax.ast.y"
                     { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1758 "src/generated/ast_parser.cpp"
    break;

  case 95: // stmt: tuple '=' tuple
#line 198 "src/syntax.ast.y"
                      { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1764 "src/generated/ast_parser.cpp"
    break;

  case 96: // stmt: IDENTIFIER declare expr
#line 199 "src/syntax.ast.y"
                              { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1770 "src/generated/ast_parser.cpp"
    break;

  case 97: // stmt: '(' id_tuple_list ')' declare tuple
#line 200 "src/syntax.ast.y"
                                          { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[4].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[4].value.as < at::AstPtr > ()); if (yystack_[3].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[3].value.as < at::AstPtr > ()); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1776 "src/generated/ast_parser.cpp"
    break;

  case 98: // stmt: IDENTIFIER declare type '=' expr
#line 201 "src/syntax.ast.y"
                                       { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[4].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[4].value.as < at::AstPtr > ()); if (yystack_[3].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[3].value.as < at::AstPtr > ()); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1782 "src/generated/ast_parser.cpp"
    break;

  case 99: // stmt: IDENTIFIER declare type '@' INTEGER
#line 202 "src/syntax.ast.y"
                                          { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[4].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[4].value.as < at::AstPtr > ()); if (yystack_[3].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[3].value.as < at::AstPtr > ()); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1788 "src/generated/ast_parser.cpp"
    break;

  case 100: // stmt: IDENTIFIER ":=" type
#line 203 "src/syntax.ast.y"
                           { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1794 "src/generated/ast_parser.cpp"
    break;

  case 101: // stmt: IDENTIFIER "!:=" type
#line 204 "src/syntax.ast.y"
                            { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1800 "src/generated/ast_parser.cpp"
    break;

  case 102: // stmt: IDENTIFIER ":=" anonymous_template
#line 205 "src/syntax.ast.y"
                                         { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1806 "src/generated/ast_parser.cpp"
    break;

  case 103: // stmt: IDENTIFIER "!:=" anonymous_template
#line 206 "src/syntax.ast.y"
                                          { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1812 "src/generated/ast_parser.cpp"
    break;

  case 104: // stmt: '{' scope '}'
#line 207 "src/syntax.ast.y"
                    { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1818 "src/generated/ast_parser.cpp"
    break;

  case 105: // stmt: expr '?' '{' scope '}'
#line 208 "src/syntax.ast.y"
                             { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[4].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[4].value.as < at::AstPtr > ()); if (yystack_[3].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[3].value.as < at::AstPtr > ()); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1824 "src/generated/ast_parser.cpp"
    break;

  case 106: // stmt: expr '?' '{' scope '}' ':' '{' scope '}'
#line 209 "src/syntax.ast.y"
                                               { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[8].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[8].value.as < at::AstPtr > ()); if (yystack_[7].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[7].value.as < at::AstPtr > ()); if (yystack_[6].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[6].value.as < at::AstPtr > ()); if (yystack_[5].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[5].value.as < at::AstPtr > ()); if (yystack_[4].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[4].value.as < at::AstPtr > ()); if (yystack_[3].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[3].value.as < at::AstPtr > ()); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1830 "src/generated/ast_parser.cpp"
    break;

  case 107: // stmt: '$' '(' expr ')' '{' scope '}'
#line 210 "src/syntax.ast.y"
                                     { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[6].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[6].value.as < at::AstPtr > ()); if (yystack_[5].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[5].value.as < at::AstPtr > ()); if (yystack_[4].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[4].value.as < at::AstPtr > ()); if (yystack_[3].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[3].value.as < at::AstPtr > ()); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1836 "src/generated/ast_parser.cpp"
    break;

  case 108: // stmt: '$' '(' IDENTIFIER ':' expr ')' '{' scope '}'
#line 211 "src/syntax.ast.y"
                                                    { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[8].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[8].value.as < at::AstPtr > ()); if (yystack_[7].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[7].value.as < at::AstPtr > ()); if (yystack_[6].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[6].value.as < at::AstPtr > ()); if (yystack_[5].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[5].value.as < at::AstPtr > ()); if (yystack_[4].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[4].value.as < at::AstPtr > ()); if (yystack_[3].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[3].value.as < at::AstPtr > ()); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1842 "src/generated/ast_parser.cpp"
    break;

  case 109: // stmt: '*' '(' expr_list ')' '{' scope '}'
#line 212 "src/syntax.ast.y"
                                          { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[6].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[6].value.as < at::AstPtr > ()); if (yystack_[5].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[5].value.as < at::AstPtr > ()); if (yystack_[4].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[4].value.as < at::AstPtr > ()); if (yystack_[3].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[3].value.as < at::AstPtr > ()); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1848 "src/generated/ast_parser.cpp"
    break;

  case 110: // stmt: '*' '(' expr_list ')' '{' scope '}' ':' '{' scope '}'
#line 213 "src/syntax.ast.y"
                                                            { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[10].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[10].value.as < at::AstPtr > ()); if (yystack_[9].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[9].value.as < at::AstPtr > ()); if (yystack_[8].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[8].value.as < at::AstPtr > ()); if (yystack_[7].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[7].value.as < at::AstPtr > ()); if (yystack_[6].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[6].value.as < at::AstPtr > ()); if (yystack_[5].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[5].value.as < at::AstPtr > ()); if (yystack_[4].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[4].value.as < at::AstPtr > ()); if (yystack_[3].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[3].value.as < at::AstPtr > ()); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1854 "src/generated/ast_parser.cpp"
    break;

  case 111: // stmt: '?' '(' cast_stmt_list ')' '{' scope '}'
#line 214 "src/syntax.ast.y"
                                               { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[6].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[6].value.as < at::AstPtr > ()); if (yystack_[5].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[5].value.as < at::AstPtr > ()); if (yystack_[4].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[4].value.as < at::AstPtr > ()); if (yystack_[3].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[3].value.as < at::AstPtr > ()); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1860 "src/generated/ast_parser.cpp"
    break;

  case 112: // stmt: '?' '(' cast_stmt_list ')' '{' scope '}' ':' '{' scope '}'
#line 215 "src/syntax.ast.y"
                                                                 { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[10].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[10].value.as < at::AstPtr > ()); if (yystack_[9].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[9].value.as < at::AstPtr > ()); if (yystack_[8].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[8].value.as < at::AstPtr > ()); if (yystack_[7].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[7].value.as < at::AstPtr > ()); if (yystack_[6].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[6].value.as < at::AstPtr > ()); if (yystack_[5].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[5].value.as < at::AstPtr > ()); if (yystack_[4].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[4].value.as < at::AstPtr > ()); if (yystack_[3].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[3].value.as < at::AstPtr > ()); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1866 "src/generated/ast_parser.cpp"
    break;

  case 113: // stmt: '+' "^=" expr
#line 216 "src/syntax.ast.y"
                    { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1872 "src/generated/ast_parser.cpp"
    break;

  case 114: // stmt: '-' "^=" expr
#line 217 "src/syntax.ast.y"
                    { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1878 "src/generated/ast_parser.cpp"
    break;

  case 115: // stmt: '*' "^=" expr
#line 218 "src/syntax.ast.y"
                    { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1884 "src/generated/ast_parser.cpp"
    break;

  case 116: // stmt: '/' "^=" expr
#line 219 "src/syntax.ast.y"
                    { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1890 "src/generated/ast_parser.cpp"
    break;

  case 117: // stmt: '%' "^=" expr
#line 220 "src/syntax.ast.y"
                    { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1896 "src/generated/ast_parser.cpp"
    break;

  case 118: // stmt: '^' "^=" expr
#line 221 "src/syntax.ast.y"
                    { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1902 "src/generated/ast_parser.cpp"
    break;

  case 119: // stmt: '&' "^=" expr
#line 222 "src/syntax.ast.y"
                    { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1908 "src/generated/ast_parser.cpp"
    break;

  case 120: // stmt: '|' "^=" expr
#line 223 "src/syntax.ast.y"
                    { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1914 "src/generated/ast_parser.cpp"
    break;

  case 121: // stmt: "||" "^=" expr
#line 224 "src/syntax.ast.y"
                     { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1920 "src/generated/ast_parser.cpp"
    break;

  case 122: // stmt: "&&" "^=" expr
#line 225 "src/syntax.ast.y"
                     { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1926 "src/generated/ast_parser.cpp"
    break;

  case 123: // stmt: "^^" "^=" expr
#line 226 "src/syntax.ast.y"
                     { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1932 "src/generated/ast_parser.cpp"
    break;

  case 124: // stmt: "==" "^=" expr
#line 227 "src/syntax.ast.y"
                     { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1938 "src/generated/ast_parser.cpp"
    break;

  case 125: // stmt: "<=" "^=" expr
#line 228 "src/syntax.ast.y"
                     { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1944 "src/generated/ast_parser.cpp"
    break;

  case 126: // stmt: '<' "^=" expr
#line 229 "src/syntax.ast.y"
                    { yylhs.value.as < at::AstPtr > () = at::makeNode("stmt", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1950 "src/generated/ast_parser.cpp"
    break;

  case 127: // scope: %empty
#line 230 "src/syntax.ast.y"
              { yylhs.value.as < at::AstPtr > () = at::makeNode("scope_empty", "", {yylhs.location.begin.line, yylhs.location.begin.column}); }
#line 1956 "src/generated/ast_parser.cpp"
    break;

  case 128: // scope: scope stmt ';'
#line 231 "src/syntax.ast.y"
                      { yylhs.value.as < at::AstPtr > () = at::makeNode("scope", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[2].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[2].value.as < at::AstPtr > ()); if (yystack_[1].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[1].value.as < at::AstPtr > ()); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); }
#line 1962 "src/generated/ast_parser.cpp"
    break;

  case 129: // start: scope
#line 233 "src/syntax.ast.y"
             { yylhs.value.as < at::AstPtr > () = at::makeNode("start", "", {yylhs.location.begin.line, yylhs.location.begin.column}); if (yystack_[0].value.as < at::AstPtr > ()) yylhs.value.as < at::AstPtr > ()->children.push_back(yystack_[0].value.as < at::AstPtr > ()); at::g_parse_result = yylhs.value.as < at::AstPtr > (); }
#line 1968 "src/generated/ast_parser.cpp"
    break;


#line 1972 "src/generated/ast_parser.cpp"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        std::string msg = YY_("syntax error");
        error (yyla.location, YY_MOVE (msg));
      }


    yyerror_range[1].location = yyla.location;
    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yyerror_range[1].location = yystack_[0].location;
        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;

      yyerror_range[2].location = yyla.location;
      YYLLOC_DEFAULT (error_token.location, yyerror_range, 2);

      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  Parser::error (const syntax_error& yyexc)
  {
    error (yyexc.location, yyexc.what ());
  }

#if YYDEBUG || 0
  const char *
  Parser::symbol_name (symbol_kind_type yysymbol)
  {
    return yytname_[yysymbol];
  }
#endif // #if YYDEBUG || 0









  const signed char Parser::yypact_ninf_ = -121;

  const short Parser::yytable_ninf_ = -130;

  const short
  Parser::yypact_[] =
  {
    -121,   117,    16,  -121,  -121,  -121,  -121,   424,    12,    -7,
      13,    50,    53,    58,    -9,  1353,  -121,  -121,  -121,  -121,
      79,    33,    43,    80,    81,    82,   113,  1287,   132,    83,
    1353,  1379,  -121,  1353,  1353,   112,   118,   120,  1533,   153,
    -121,   169,   -38,   123,  -121,    39,  1353,  -121,  -121,  -121,
    -121,  -121,    39,    39,  -121,  1419,  -121,     0,  1353,  1353,
    1353,  1353,  1353,   183,    77,  1353,    39,  1353,  1353,  1944,
    1353,  1353,   137,   184,  1353,  1353,  -121,  -121,  -121,  1353,
    1353,  1353,  1353,  1353,  1985,  1353,  1445,  1944,   -28,    -4,
    1680,   -31,   388,  1635,   137,   183,   183,  1485,  1353,  1353,
    1353,  1353,  1353,  1353,  1353,  1353,  1353,  1353,  1353,  1353,
    1353,  1353,  1353,  1353,  1353,  1353,  1353,  1313,  1353,  1353,
     185,  1353,  1353,    39,  1353,  1353,  -121,   204,     1,   -12,
     204,   169,   204,   169,    91,  1944,     9,   153,    47,   206,
    1944,  1944,  1944,  1944,  1944,    -2,  -121,    57,  1353,  1944,
    1944,  1944,    84,   210,  1944,  1944,  1944,  1944,  1944,  1944,
     101,  1724,  -121,   126,  -121,  1353,   211,  -121,   222,   213,
    -121,  -121,   128,   163,   116,  1768,   167,   167,   167,   161,
     161,   161,   161,  1944,   193,   193,   137,   137,   137,  1985,
    1985,  1985,   161,   161,  1944,  -121,  1812,    24,   175,  -121,
    1944,   -10,     2,  1584,   173,  1944,  -121,    39,  -121,  1353,
     219,   178,   187,  -121,  -121,    39,  1353,   221,   183,   209,
    -121,  1353,    39,   212,  1445,  1944,  -121,  1353,  -121,    51,
      99,  1353,   216,   489,  1353,  -121,  -121,  -121,  -121,   204,
    1944,  -121,   183,   183,     7,  1944,   189,  -121,  -121,  1856,
     204,  -121,  -121,   173,   208,  -121,    39,  -121,  1900,  -121,
     218,  1944,   182,   194,  1353,    39,   546,    39,   603,    39,
     660,   204,    75,   717,   217,   774,   227,   236,    54,  1944,
       5,   225,   204,   228,    14,  -121,  -121,    39,  -121,  -121,
    -121,  -121,    39,   230,  -121,  -121,   229,   233,   234,   831,
     204,   888,   945,   204,    39,  1002,  -121,  -121,  -121,  -121,
    -121,  -121,    21,  -121,  1059,  1116,  1173,   240,  -121,  -121,
    -121,  -121,  1230,  -121
  };

  const unsigned char
  Parser::yydefact_[] =
  {
     127,    87,     0,     2,     5,     6,     8,     7,     0,     0,
       0,     0,     0,     0,     0,    89,    90,    91,     3,     4,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      46,     0,   127,     0,     0,     0,     0,     0,    92,     0,
      59,     0,     0,     0,     1,     0,    46,    74,    75,    76,
      77,    78,     0,     0,    73,     0,    64,     0,     0,     0,
       0,     0,     0,    43,     7,     0,     0,     0,     0,    88,
       0,     0,    24,    51,     0,    46,    58,    49,    52,     0,
       0,     0,     0,     0,    38,     0,     0,    47,     0,     7,
       0,     0,    87,     0,    25,    43,    43,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    46,    46,
       0,     0,    46,     0,     0,     0,   128,    66,     0,     0,
     100,   102,   101,   103,     7,    96,     0,    52,     0,     0,
     121,   122,   123,   125,   124,    39,    44,     0,     0,   113,
     114,   115,     0,     0,   116,   117,   118,   120,   119,   126,
       7,     0,    81,     0,    33,     0,     0,    29,     0,     0,
     104,    53,     0,     0,     7,     0,    26,    27,    28,    20,
      22,    21,    23,    94,     9,    10,    11,    12,    13,    14,
      15,    16,    18,    19,    93,   127,     0,     0,     0,    30,
      83,     0,     0,     0,    95,    84,    54,     0,    34,     0,
       0,     0,     0,    70,    65,     0,     0,    56,     0,     0,
      50,     0,     0,     0,     0,    48,    85,     0,    86,     0,
       0,     0,     0,    87,     0,    32,    31,    35,    55,    67,
      98,    99,    43,    43,    41,    40,     0,    45,   127,     0,
      79,   127,    82,    97,     0,   127,     0,   127,     0,   127,
     105,    17,     0,     0,     0,     0,    87,     0,    87,     0,
      87,    60,     0,    87,     0,    87,     0,    68,     0,    42,
       0,   109,    80,   111,     0,    36,   127,     0,    62,   127,
     107,   127,     0,     0,   127,    57,     0,     0,     0,    87,
      61,    87,    87,    69,     0,    87,   127,   127,   127,    63,
     108,   106,     0,    71,    87,    87,    87,     0,   110,   112,
      37,   127,    87,    72
  };

  const short
  Parser::yypgoto_[] =
  {
    -121,   257,    64,   -93,   -40,   -27,  -121,   -21,  -121,   155,
    -121,   171,    59,   127,    72,  -121,  -120,  -121,  -121,   -32,
    -121
  };

  const short
  Parser::yydefgoto_[] =
  {
       0,    38,   146,   147,    88,    76,    77,    39,   272,    40,
      57,   128,    41,    55,   162,   163,    42,    91,    43,     1,
       2
  };

  const short
  Parser::yytable_[] =
  {
      92,    78,   172,   173,   204,    45,   129,   208,   124,   237,
     138,   206,   238,   153,    46,   153,    44,   153,   127,    56,
     168,   164,   153,   125,    78,   130,   132,    58,   136,   153,
     169,    78,    78,   165,   137,   152,     3,     4,     5,     6,
      64,    63,     8,   215,   216,    78,    73,    59,     8,   165,
      73,   165,     8,   264,    14,   209,   295,   166,    18,    19,
      14,   139,   207,   207,    14,   298,   210,    71,   211,    65,
      66,   254,   317,   235,   293,    67,    66,    74,   197,   198,
      66,    30,   201,    68,    60,   165,    45,    61,    33,    34,
      35,    36,    62,    75,    33,    46,   127,    36,    33,   -51,
      45,    36,    78,   255,   212,    36,   294,   253,   217,    46,
      45,   131,   133,    70,    79,    80,    81,  -129,   218,    46,
       3,     4,     5,     6,     7,    45,     8,   286,     9,    10,
      11,    12,    13,    86,    46,   219,   287,   -51,    14,    15,
      16,    17,    18,    19,   256,   165,   221,    82,   -51,   262,
     263,   257,    20,    21,    22,    23,    24,    25,    26,    27,
      28,   231,    95,   233,    29,    30,    85,    31,    96,    32,
      97,   122,    33,    34,    35,    36,    37,   223,   123,   229,
     239,   101,   102,   103,   104,   126,    78,   224,   244,   218,
     145,   120,   199,    45,    78,   250,   106,   107,   108,   109,
     110,    78,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   153,   214,   230,   120,   266,   220,   226,   268,
     228,   120,   241,   270,   218,   273,   236,   275,   242,   271,
     108,   109,   110,   277,   125,    78,   165,   243,   280,   265,
     282,   246,   284,   218,    78,   278,    78,   120,    78,    47,
      48,    49,    50,    51,   299,   218,   292,   301,   269,   302,
     300,   248,   305,   276,   251,   303,    78,    54,   259,   289,
     296,    78,    69,   297,   314,   315,   316,   312,    72,   291,
     304,   306,   247,    78,    84,   307,   308,    87,    90,   322,
      93,    94,   321,   213,   202,   227,   252,     0,     0,     0,
       0,     0,     0,    87,     0,     0,     0,     0,     0,     0,
       0,     0,   135,     0,     0,   140,   141,   142,   143,   144,
       0,     0,    72,     0,    84,    90,     0,   149,   150,     0,
       0,   151,    87,     0,     0,     0,   154,   155,   156,   157,
     158,     0,   159,   161,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   189,   190,
     191,   192,   193,   194,   196,    87,    87,     0,   200,    87,
       0,   203,   205,     0,     0,     0,     0,     0,     0,     0,
       0,     3,     4,     5,     6,     7,     0,     8,     0,     9,
      10,    11,    12,    13,     0,   196,     0,     0,     0,    14,
      15,    16,    17,    18,    19,     0,     0,     0,     0,     0,
       0,     0,   225,    20,    21,    22,    23,    24,    25,    26,
      27,    28,     0,    45,     0,    29,    30,     0,    31,     0,
      32,   170,    46,    33,    34,    35,    36,    37,     0,     0,
       0,    47,    48,    49,    50,    51,    52,    53,     0,     0,
       0,     0,     0,     0,     0,     0,   240,     0,     0,    54,
       0,     0,     0,   245,     0,     0,     0,     0,   249,     0,
       0,   161,     0,     0,   203,     0,     0,     0,   258,     0,
       0,   261,     3,     4,     5,     6,     7,     0,     8,     0,
       9,    10,    11,    12,    13,     0,     0,     0,     0,     0,
      14,    15,    16,    17,    18,    19,     0,     0,     0,     0,
       0,   279,     0,     0,    20,    21,    22,    23,    24,    25,
      26,    27,    28,     0,     0,     0,    29,    30,     0,    31,
       0,    32,   260,     0,    33,    34,    35,    36,    37,     3,
       4,     5,     6,     7,     0,     8,     0,     9,    10,    11,
      12,    13,     0,     0,     0,     0,     0,    14,    15,    16,
      17,    18,    19,     0,     0,     0,     0,     0,     0,     0,
       0,    20,    21,    22,    23,    24,    25,    26,    27,    28,
       0,     0,     0,    29,    30,     0,    31,     0,    32,   281,
       0,    33,    34,    35,    36,    37,     3,     4,     5,     6,
       7,     0,     8,     0,     9,    10,    11,    12,    13,     0,
       0,     0,     0,     0,    14,    15,    16,    17,    18,    19,
       0,     0,     0,     0,     0,     0,     0,     0,    20,    21,
      22,    23,    24,    25,    26,    27,    28,     0,     0,     0,
      29,    30,     0,    31,     0,    32,   283,     0,    33,    34,
      35,    36,    37,     3,     4,     5,     6,     7,     0,     8,
       0,     9,    10,    11,    12,    13,     0,     0,     0,     0,
       0,    14,    15,    16,    17,    18,    19,     0,     0,     0,
       0,     0,     0,     0,     0,    20,    21,    22,    23,    24,
      25,    26,    27,    28,     0,     0,     0,    29,    30,     0,
      31,     0,    32,   285,     0,    33,    34,    35,    36,    37,
       3,     4,     5,     6,     7,     0,     8,     0,     9,    10,
      11,    12,    13,     0,     0,     0,     0,     0,    14,    15,
      16,    17,    18,    19,     0,     0,     0,     0,     0,     0,
       0,     0,    20,    21,    22,    23,    24,    25,    26,    27,
      28,     0,     0,     0,    29,    30,     0,    31,     0,    32,
     288,     0,    33,    34,    35,    36,    37,     3,     4,     5,
       6,     7,     0,     8,     0,     9,    10,    11,    12,    13,
       0,     0,     0,     0,     0,    14,    15,    16,    17,    18,
      19,     0,     0,     0,     0,     0,     0,     0,     0,    20,
      21,    22,    23,    24,    25,    26,    27,    28,     0,     0,
       0,    29,    30,     0,    31,     0,    32,   290,     0,    33,
      34,    35,    36,    37,     3,     4,     5,     6,     7,     0,
       8,     0,     9,    10,    11,    12,    13,     0,     0,     0,
       0,     0,    14,    15,    16,    17,    18,    19,     0,     0,
       0,     0,     0,     0,     0,     0,    20,    21,    22,    23,
      24,    25,    26,    27,    28,     0,     0,     0,    29,    30,
       0,    31,     0,    32,   309,     0,    33,    34,    35,    36,
      37,     3,     4,     5,     6,     7,     0,     8,     0,     9,
      10,    11,    12,    13,     0,     0,     0,     0,     0,    14,
      15,    16,    17,    18,    19,     0,     0,     0,     0,     0,
       0,     0,     0,    20,    21,    22,    23,    24,    25,    26,
      27,    28,     0,     0,     0,    29,    30,     0,    31,     0,
      32,   310,     0,    33,    34,    35,    36,    37,     3,     4,
       5,     6,     7,     0,     8,     0,     9,    10,    11,    12,
      13,     0,     0,     0,     0,     0,    14,    15,    16,    17,
      18,    19,     0,     0,     0,     0,     0,     0,     0,     0,
      20,    21,    22,    23,    24,    25,    26,    27,    28,     0,
       0,     0,    29,    30,     0,    31,     0,    32,   311,     0,
      33,    34,    35,    36,    37,     3,     4,     5,     6,     7,
       0,     8,     0,     9,    10,    11,    12,    13,     0,     0,
       0,     0,     0,    14,    15,    16,    17,    18,    19,     0,
       0,     0,     0,     0,     0,     0,     0,    20,    21,    22,
      23,    24,    25,    26,    27,    28,     0,     0,     0,    29,
      30,     0,    31,     0,    32,   313,     0,    33,    34,    35,
      36,    37,     3,     4,     5,     6,     7,     0,     8,     0,
       9,    10,    11,    12,    13,     0,     0,     0,     0,     0,
      14,    15,    16,    17,    18,    19,     0,     0,     0,     0,
       0,     0,     0,     0,    20,    21,    22,    23,    24,    25,
      26,    27,    28,     0,     0,     0,    29,    30,     0,    31,
       0,    32,   318,     0,    33,    34,    35,    36,    37,     3,
       4,     5,     6,     7,     0,     8,     0,     9,    10,    11,
      12,    13,     0,     0,     0,     0,     0,    14,    15,    16,
      17,    18,    19,     0,     0,     0,     0,     0,     0,     0,
       0,    20,    21,    22,    23,    24,    25,    26,    27,    28,
       0,     0,     0,    29,    30,     0,    31,     0,    32,   319,
       0,    33,    34,    35,    36,    37,     3,     4,     5,     6,
       7,     0,     8,     0,     9,    10,    11,    12,    13,     0,
       0,     0,     0,     0,    14,    15,    16,    17,    18,    19,
       0,     0,     0,     0,     0,     0,     0,     0,    20,    21,
      22,    23,    24,    25,    26,    27,    28,     0,     0,     0,
      29,    30,     0,    31,     0,    32,   320,     0,    33,    34,
      35,    36,    37,     3,     4,     5,     6,     7,     0,     8,
       0,     9,    10,    11,    12,    13,     0,     0,     0,     0,
       0,    14,    15,    16,    17,    18,    19,     0,     0,     0,
       0,     0,     0,     0,     0,    20,    21,    22,    23,    24,
      25,    26,    27,    28,     0,     0,     0,    29,    30,     0,
      31,     0,    32,   323,     0,    33,    34,    35,    36,    37,
       3,     4,     5,     6,    64,     0,     8,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    14,     0,
       0,     0,    18,    19,     0,     0,     3,     4,     5,     6,
      64,    83,     8,    65,    66,     0,     0,     0,     0,    67,
       0,     0,     0,     0,    14,    30,     0,    68,    18,    19,
       0,     0,    33,    34,    35,    36,     0,     0,     0,    65,
      66,     0,     0,     0,     0,    67,     3,     4,     5,     6,
      64,    30,     8,    68,     0,   195,     0,     0,    33,    34,
      35,    36,     0,     0,    14,     0,     0,     0,    18,    19,
       0,     0,     3,     4,     5,     6,    89,     0,     8,    65,
      66,     0,     0,     0,     0,    67,     0,     0,     0,     0,
      14,    30,     0,    68,    18,    19,     0,     0,    33,    34,
      35,    36,     0,     0,     0,    65,    66,     0,     0,     0,
       0,    67,     3,     4,     5,     6,   134,    30,     8,    68,
       0,     0,     0,     0,    33,    34,    35,    36,     0,     0,
      14,     0,     0,     0,    18,    19,     0,     0,     3,     4,
       5,     6,   160,     0,     8,    65,    66,     0,     0,     0,
       0,    67,     0,     0,     0,     0,    14,    30,     0,    68,
      18,    19,     0,     0,    33,    34,    35,    36,     0,     0,
       0,    65,    66,     0,     0,     0,     0,    67,     3,     4,
       5,     6,   174,    30,     8,    68,     0,     0,     0,     0,
      33,    34,    35,    36,     0,     0,    14,     0,     0,     0,
      18,    19,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    65,    66,     0,     0,     0,     0,    67,     0,     0,
       0,     0,     0,    30,     0,    68,     0,     0,     0,     0,
      33,    34,    35,    36,    98,    99,   100,   101,   102,   103,
     104,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,     0,   116,
     117,   118,     0,   119,     0,     0,     0,   120,     0,     0,
       0,     0,     0,     0,   121,    98,    99,   100,   101,   102,
     103,   104,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,     0,
       0,   148,   118,     0,   119,     0,     0,     0,   120,     0,
       0,     0,     0,     0,     0,   121,    98,    99,   100,   101,
     102,   103,   104,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
       0,     0,   148,   118,     0,   119,     0,     0,     0,   120,
     171,    98,    99,   100,   101,   102,   103,   104,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,     0,     0,   148,   118,     0,
     119,   167,     0,     0,   120,    98,    99,   100,   101,   102,
     103,   104,     0,     0,   222,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,     0,
       0,   148,   118,     0,   119,     0,     0,     0,   120,    98,
      99,   100,   101,   102,   103,   104,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,     0,     0,   148,   118,     0,   119,   232,
       0,     0,   120,    98,    99,   100,   101,   102,   103,   104,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   234,     0,   148,
     118,     0,   119,     0,     0,     0,   120,    98,    99,   100,
     101,   102,   103,   104,     0,     0,   267,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,     0,     0,   148,   118,     0,   119,     0,     0,     0,
     120,    98,    99,   100,   101,   102,   103,   104,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,     0,     0,   148,   118,     0,
     119,   274,     0,     0,   120,    98,    99,   100,   101,   102,
     103,   104,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,     0,
       0,   148,   118,     0,   119,     0,     0,     0,   120,   101,
     102,   103,   104,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     106,   107,   108,   109,   110,     0,     0,     0,   114,   115,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   120
  };

  const short
  Parser::yycheck_[] =
  {
      32,    22,    95,    96,   124,     9,    46,    19,    46,    19,
      10,    10,    10,     8,    18,     8,     0,     8,    45,     7,
      51,    49,     8,    61,    45,    52,    53,    34,    55,     8,
      61,    52,    53,    61,    55,    75,     3,     4,     5,     6,
       7,    50,     9,    45,    46,    66,     7,    34,     9,    61,
       7,    61,     9,    46,    21,    46,    51,    61,    25,    26,
      21,    61,    61,    61,    21,    51,    57,    34,    21,    36,
      37,    20,    51,    49,    20,    42,    37,    34,   118,   119,
      37,    48,   122,    50,    34,    61,     9,    34,    55,    56,
      57,    58,    34,    50,    55,    18,   123,    58,    55,     8,
       9,    58,   123,    52,    57,    58,    52,   227,    51,    18,
       9,    52,    53,    34,    34,    34,    34,     0,    61,    18,
       3,     4,     5,     6,     7,     9,     9,    52,    11,    12,
      13,    14,    15,    50,    18,    51,    61,    46,    21,    22,
      23,    24,    25,    26,    45,    61,    45,    34,    57,   242,
     243,    52,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    45,    50,   195,    47,    48,    34,    50,    50,    52,
      50,    18,    55,    56,    57,    58,    59,    51,     9,    51,
     207,    14,    15,    16,    17,    62,   207,    61,   215,    61,
       7,    54,     7,     9,   215,   222,    35,    36,    37,    38,
      39,   222,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,     8,     7,    51,    54,   248,     7,     7,   251,
       7,    54,     3,   255,    61,   257,    51,   259,    50,   256,
      37,    38,    39,    51,    61,   256,    61,    50,   265,    50,
     267,    20,   269,    61,   265,    51,   267,    54,   269,    27,
      28,    29,    30,    31,   286,    61,    20,   289,    50,   291,
     287,    52,   294,    45,    52,   292,   287,    45,    52,    52,
      45,   292,    15,    45,   306,   307,   308,   304,    21,    52,
      50,    52,   218,   304,    27,    52,    52,    30,    31,   321,
      33,    34,    52,   138,   123,   168,   224,    -1,    -1,    -1,
      -1,    -1,    -1,    46,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    55,    -1,    -1,    58,    59,    60,    61,    62,
      -1,    -1,    65,    -1,    67,    68,    -1,    70,    71,    -1,
      -1,    74,    75,    -1,    -1,    -1,    79,    80,    81,    82,
      83,    -1,    85,    86,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,    -1,   121,   122,
      -1,   124,   125,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,     5,     6,     7,    -1,     9,    -1,    11,
      12,    13,    14,    15,    -1,   148,    -1,    -1,    -1,    21,
      22,    23,    24,    25,    26,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   165,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    -1,     9,    -1,    47,    48,    -1,    50,    -1,
      52,    53,    18,    55,    56,    57,    58,    59,    -1,    -1,
      -1,    27,    28,    29,    30,    31,    32,    33,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   209,    -1,    -1,    45,
      -1,    -1,    -1,   216,    -1,    -1,    -1,    -1,   221,    -1,
      -1,   224,    -1,    -1,   227,    -1,    -1,    -1,   231,    -1,
      -1,   234,     3,     4,     5,     6,     7,    -1,     9,    -1,
      11,    12,    13,    14,    15,    -1,    -1,    -1,    -1,    -1,
      21,    22,    23,    24,    25,    26,    -1,    -1,    -1,    -1,
      -1,   264,    -1,    -1,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    -1,    -1,    -1,    47,    48,    -1,    50,
      -1,    52,    53,    -1,    55,    56,    57,    58,    59,     3,
       4,     5,     6,     7,    -1,     9,    -1,    11,    12,    13,
      14,    15,    -1,    -1,    -1,    -1,    -1,    21,    22,    23,
      24,    25,    26,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      -1,    -1,    -1,    47,    48,    -1,    50,    -1,    52,    53,
      -1,    55,    56,    57,    58,    59,     3,     4,     5,     6,
       7,    -1,     9,    -1,    11,    12,    13,    14,    15,    -1,
      -1,    -1,    -1,    -1,    21,    22,    23,    24,    25,    26,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    -1,    -1,    -1,
      47,    48,    -1,    50,    -1,    52,    53,    -1,    55,    56,
      57,    58,    59,     3,     4,     5,     6,     7,    -1,     9,
      -1,    11,    12,    13,    14,    15,    -1,    -1,    -1,    -1,
      -1,    21,    22,    23,    24,    25,    26,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    -1,    -1,    -1,    47,    48,    -1,
      50,    -1,    52,    53,    -1,    55,    56,    57,    58,    59,
       3,     4,     5,     6,     7,    -1,     9,    -1,    11,    12,
      13,    14,    15,    -1,    -1,    -1,    -1,    -1,    21,    22,
      23,    24,    25,    26,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    -1,    -1,    -1,    47,    48,    -1,    50,    -1,    52,
      53,    -1,    55,    56,    57,    58,    59,     3,     4,     5,
       6,     7,    -1,     9,    -1,    11,    12,    13,    14,    15,
      -1,    -1,    -1,    -1,    -1,    21,    22,    23,    24,    25,
      26,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    -1,    -1,
      -1,    47,    48,    -1,    50,    -1,    52,    53,    -1,    55,
      56,    57,    58,    59,     3,     4,     5,     6,     7,    -1,
       9,    -1,    11,    12,    13,    14,    15,    -1,    -1,    -1,
      -1,    -1,    21,    22,    23,    24,    25,    26,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    -1,    -1,    -1,    47,    48,
      -1,    50,    -1,    52,    53,    -1,    55,    56,    57,    58,
      59,     3,     4,     5,     6,     7,    -1,     9,    -1,    11,
      12,    13,    14,    15,    -1,    -1,    -1,    -1,    -1,    21,
      22,    23,    24,    25,    26,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    -1,    -1,    -1,    47,    48,    -1,    50,    -1,
      52,    53,    -1,    55,    56,    57,    58,    59,     3,     4,
       5,     6,     7,    -1,     9,    -1,    11,    12,    13,    14,
      15,    -1,    -1,    -1,    -1,    -1,    21,    22,    23,    24,
      25,    26,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    -1,
      -1,    -1,    47,    48,    -1,    50,    -1,    52,    53,    -1,
      55,    56,    57,    58,    59,     3,     4,     5,     6,     7,
      -1,     9,    -1,    11,    12,    13,    14,    15,    -1,    -1,
      -1,    -1,    -1,    21,    22,    23,    24,    25,    26,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    -1,    -1,    -1,    47,
      48,    -1,    50,    -1,    52,    53,    -1,    55,    56,    57,
      58,    59,     3,     4,     5,     6,     7,    -1,     9,    -1,
      11,    12,    13,    14,    15,    -1,    -1,    -1,    -1,    -1,
      21,    22,    23,    24,    25,    26,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    -1,    -1,    -1,    47,    48,    -1,    50,
      -1,    52,    53,    -1,    55,    56,    57,    58,    59,     3,
       4,     5,     6,     7,    -1,     9,    -1,    11,    12,    13,
      14,    15,    -1,    -1,    -1,    -1,    -1,    21,    22,    23,
      24,    25,    26,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      -1,    -1,    -1,    47,    48,    -1,    50,    -1,    52,    53,
      -1,    55,    56,    57,    58,    59,     3,     4,     5,     6,
       7,    -1,     9,    -1,    11,    12,    13,    14,    15,    -1,
      -1,    -1,    -1,    -1,    21,    22,    23,    24,    25,    26,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    -1,    -1,    -1,
      47,    48,    -1,    50,    -1,    52,    53,    -1,    55,    56,
      57,    58,    59,     3,     4,     5,     6,     7,    -1,     9,
      -1,    11,    12,    13,    14,    15,    -1,    -1,    -1,    -1,
      -1,    21,    22,    23,    24,    25,    26,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    -1,    -1,    -1,    47,    48,    -1,
      50,    -1,    52,    53,    -1,    55,    56,    57,    58,    59,
       3,     4,     5,     6,     7,    -1,     9,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    21,    -1,
      -1,    -1,    25,    26,    -1,    -1,     3,     4,     5,     6,
       7,    34,     9,    36,    37,    -1,    -1,    -1,    -1,    42,
      -1,    -1,    -1,    -1,    21,    48,    -1,    50,    25,    26,
      -1,    -1,    55,    56,    57,    58,    -1,    -1,    -1,    36,
      37,    -1,    -1,    -1,    -1,    42,     3,     4,     5,     6,
       7,    48,     9,    50,    -1,    52,    -1,    -1,    55,    56,
      57,    58,    -1,    -1,    21,    -1,    -1,    -1,    25,    26,
      -1,    -1,     3,     4,     5,     6,     7,    -1,     9,    36,
      37,    -1,    -1,    -1,    -1,    42,    -1,    -1,    -1,    -1,
      21,    48,    -1,    50,    25,    26,    -1,    -1,    55,    56,
      57,    58,    -1,    -1,    -1,    36,    37,    -1,    -1,    -1,
      -1,    42,     3,     4,     5,     6,     7,    48,     9,    50,
      -1,    -1,    -1,    -1,    55,    56,    57,    58,    -1,    -1,
      21,    -1,    -1,    -1,    25,    26,    -1,    -1,     3,     4,
       5,     6,     7,    -1,     9,    36,    37,    -1,    -1,    -1,
      -1,    42,    -1,    -1,    -1,    -1,    21,    48,    -1,    50,
      25,    26,    -1,    -1,    55,    56,    57,    58,    -1,    -1,
      -1,    36,    37,    -1,    -1,    -1,    -1,    42,     3,     4,
       5,     6,     7,    48,     9,    50,    -1,    -1,    -1,    -1,
      55,    56,    57,    58,    -1,    -1,    21,    -1,    -1,    -1,
      25,    26,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    36,    37,    -1,    -1,    -1,    -1,    42,    -1,    -1,
      -1,    -1,    -1,    48,    -1,    50,    -1,    -1,    -1,    -1,
      55,    56,    57,    58,    11,    12,    13,    14,    15,    16,
      17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    -1,    46,
      47,    48,    -1,    50,    -1,    -1,    -1,    54,    -1,    -1,
      -1,    -1,    -1,    -1,    61,    11,    12,    13,    14,    15,
      16,    17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    -1,
      -1,    47,    48,    -1,    50,    -1,    -1,    -1,    54,    -1,
      -1,    -1,    -1,    -1,    -1,    61,    11,    12,    13,    14,
      15,    16,    17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      -1,    -1,    47,    48,    -1,    50,    -1,    -1,    -1,    54,
      55,    11,    12,    13,    14,    15,    16,    17,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    -1,    -1,    47,    48,    -1,
      50,    51,    -1,    -1,    54,    11,    12,    13,    14,    15,
      16,    17,    -1,    -1,    20,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    -1,
      -1,    47,    48,    -1,    50,    -1,    -1,    -1,    54,    11,
      12,    13,    14,    15,    16,    17,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    -1,    -1,    47,    48,    -1,    50,    51,
      -1,    -1,    54,    11,    12,    13,    14,    15,    16,    17,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    -1,    47,
      48,    -1,    50,    -1,    -1,    -1,    54,    11,    12,    13,
      14,    15,    16,    17,    -1,    -1,    20,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    -1,    -1,    47,    48,    -1,    50,    -1,    -1,    -1,
      54,    11,    12,    13,    14,    15,    16,    17,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    -1,    -1,    47,    48,    -1,
      50,    51,    -1,    -1,    54,    11,    12,    13,    14,    15,
      16,    17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    -1,
      -1,    47,    48,    -1,    50,    -1,    -1,    -1,    54,    14,
      15,    16,    17,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      35,    36,    37,    38,    39,    -1,    -1,    -1,    43,    44,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    54
  };

  const signed char
  Parser::yystos_[] =
  {
       0,    84,    85,     3,     4,     5,     6,     7,     9,    11,
      12,    13,    14,    15,    21,    22,    23,    24,    25,    26,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    47,
      48,    50,    52,    55,    56,    57,    58,    59,    66,    72,
      74,    77,    81,    83,     0,     9,    18,    27,    28,    29,
      30,    31,    32,    33,    45,    78,     7,    75,    34,    34,
      34,    34,    34,    50,     7,    36,    37,    42,    50,    66,
      34,    34,    66,     7,    34,    50,    70,    71,    72,    34,
      34,    34,    34,    34,    66,    34,    50,    66,    69,     7,
      66,    82,    84,    66,    66,    50,    50,    50,    11,    12,
      13,    14,    15,    16,    17,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    46,    47,    48,    50,
      54,    61,    18,     9,    46,    61,    62,    70,    76,    69,
      70,    77,    70,    77,     7,    66,    70,    72,    10,    61,
      66,    66,    66,    66,    66,     7,    67,    68,    47,    66,
      66,    66,    69,     8,    66,    66,    66,    66,    66,    66,
       7,    66,    79,    80,    49,    61,    61,    51,    51,    61,
      53,    55,    68,    68,     7,    66,    66,    66,    66,    66,
      66,    66,    66,    66,    66,    66,    66,    66,    66,    66,
      66,    66,    66,    66,    66,    52,    66,    69,    69,     7,
      66,    69,    76,    66,    81,    66,    10,    61,    19,    46,
      57,    21,    57,    74,     7,    45,    46,    51,    61,    51,
       7,    45,    20,    51,    61,    66,     7,    78,     7,    51,
      51,    45,    51,    84,    45,    49,    51,    19,    10,    70,
      66,     3,    50,    50,    70,    66,    20,    67,    52,    66,
      70,    52,    79,    81,    20,    52,    45,    52,    66,    52,
      53,    66,    68,    68,    46,    50,    84,    20,    84,    50,
      84,    70,    73,    84,    51,    84,    45,    51,    51,    66,
      70,    53,    70,    53,    70,    53,    52,    61,    53,    52,
      53,    52,    20,    20,    52,    51,    45,    45,    51,    84,
      70,    84,    84,    70,    50,    84,    52,    52,    52,    53,
      53,    53,    70,    53,    84,    84,    84,    51,    53,    53,
      53,    52,    84,    53
  };

  const signed char
  Parser::yyr1_[] =
  {
       0,    65,    66,    66,    66,    66,    66,    66,    66,    66,
      66,    66,    66,    66,    66,    66,    66,    66,    66,    66,
      66,    66,    66,    66,    66,    66,    66,    66,    66,    66,
      66,    66,    66,    66,    66,    66,    66,    66,    66,    67,
      67,    67,    67,    68,    68,    68,    69,    69,    69,    70,
      70,    71,    71,    72,    72,    72,    72,    72,    72,    72,
      73,    73,    74,    74,    75,    75,    76,    76,    77,    77,
      77,    77,    77,    78,    78,    78,    78,    78,    78,    79,
      79,    80,    80,    81,    81,    82,    82,    83,    83,    83,
      83,    83,    83,    83,    83,    83,    83,    83,    83,    83,
      83,    83,    83,    83,    83,    83,    83,    83,    83,    83,
      83,    83,    83,    83,    83,    83,    83,    83,    83,    83,
      83,    83,    83,    83,    83,    83,    83,    84,    84,    85
  };

  const signed char
  Parser::yyr2_[] =
  {
       0,     2,     1,     1,     1,     1,     1,     1,     1,     3,
       3,     3,     3,     3,     3,     3,     3,     5,     3,     3,
       3,     3,     3,     3,     2,     2,     3,     3,     3,     3,
       3,     4,     4,     3,     4,     4,     7,    11,     2,     1,
       3,     3,     5,     0,     1,     3,     0,     1,     3,     1,
       3,     1,     1,     3,     4,     4,     4,     8,     2,     1,
       1,     3,     7,     9,     1,     3,     1,     3,     7,     9,
       4,    10,    14,     1,     1,     1,     1,     1,     1,     3,
       5,     1,     3,     3,     3,     3,     3,     0,     2,     1,
       1,     1,     1,     3,     3,     3,     3,     5,     5,     5,
       3,     3,     3,     3,     3,     5,     9,     7,     9,     7,
      11,     7,    11,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     0,     3,     1
  };


#if YYDEBUG
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const Parser::yytname_[] =
  {
  "\"end of file\"", "error", "\"invalid token\"", "INTEGER", "DECIMAL",
  "STRING", "IMPORT", "IDENTIFIER", "\"::\"", "\"<|\"", "\"|>\"", "\"||\"",
  "\"&&\"", "\"^^\"", "\"<=\"", "\"==\"", "\">=\"", "\"!=\"", "\"<{\"",
  "\"}>\"", "\"->\"", "\"@?\"", "\"@<\"", "\"$^\"", "\"$!\"", "\"@$\"",
  "\"#$\"", "\"^:\"", "\"~:\"", "\"!:\"", "\"!^:\"", "\"!~:\"", "\":=\"",
  "\"!:=\"", "\"^=\"", "'+'", "'-'", "'*'", "'/'", "'%'", "'^'", "'|'",
  "'&'", "'<'", "'>'", "':'", "'='", "'?'", "'['", "']'", "'('", "')'",
  "'{'", "'}'", "'.'", "'`'", "'!'", "'@'", "'#'", "'$'", "'~'", "','",
  "';'", "TYPE_REDUCE", "UNARY", "$accept", "expr", "func_arg_declare",
  "func_arg_declare_list", "expr_list", "type", "base_type",
  "complex_type", "type_list", "class", "template_arg_declare_list",
  "template_arg_list", "anonymous_template", "declare", "cast_stmt",
  "cast_stmt_list", "tuple", "id_tuple_list", "stmt", "scope", "start", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const unsigned char
  Parser::yyrline_[] =
  {
       0,    82,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   120,
     121,   122,   123,   126,   129,   130,   132,   133,   134,   136,
     137,   139,   140,   142,   143,   144,   145,   146,   147,   148,
     150,   151,   153,   154,   156,   157,   159,   160,   163,   164,
     165,   166,   167,   169,   170,   171,   172,   173,   174,   176,
     177,   179,   180,   184,   185,   187,   188,   190,   191,   192,
     193,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,   224,   225,   226,   227,   228,   229,   230,   231,   233
  };

  void
  Parser::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
  Parser::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG

  Parser::symbol_kind_type
  Parser::yytranslate_ (int t) YY_NOEXCEPT
  {
    // YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to
    // TOKEN-NUM as returned by yylex.
    static
    const signed char
    translate_table[] =
    {
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    56,     2,    58,    59,    39,    42,     2,
      50,    51,    37,    35,    61,    36,    54,    38,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    45,    62,
      43,    46,    44,    47,    57,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    48,     2,    49,    40,     2,    55,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    52,    41,    53,    60,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      63,    64
    };
    // Last valid token kind.
    const int code_max = 291;

    if (t <= 0)
      return symbol_kind::S_YYEOF;
    else if (t <= code_max)
      return static_cast <symbol_kind_type> (translate_table[t]);
    else
      return symbol_kind::S_YYUNDEF;
  }

#line 6 "src/syntax.ast.y"
} // parser
#line 2876 "src/generated/ast_parser.cpp"

#line 234 "src/syntax.ast.y"

