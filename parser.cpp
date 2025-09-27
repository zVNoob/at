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



// First part of user prologue.
#line 29 "parser.y"

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

#line 56 "parser.cpp"


#include "parser.hpp"




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

#line 6 "parser.y"
namespace parser {
#line 154 "parser.cpp"

  /// Build a parser object.
  Parser::Parser (lexer::Lexer* lexer_yyarg, error::ErrorReporter* err_rp_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      lexer (lexer_yyarg),
      err_rp (err_rp_yyarg)
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
      case symbol_kind::S_FRACTION: // FRACTION
      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_VARIABLE: // VARIABLE
      case symbol_kind::S_TYPE: // TYPE
      case symbol_kind::S_vars: // vars
      case symbol_kind::S_expr: // expr
        value.copy< std::shared_ptr<object::Object> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type: // type
        value.copy< std::shared_ptr<type::Type> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_arg_list: // arg_list
      case symbol_kind::S_expr_list: // expr_list
      case symbol_kind::S_var_list: // var_list
        value.copy< std::vector<std::shared_ptr<object::Object>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type_list: // type_list
        value.copy< std::vector<std::shared_ptr<type::Type>> > (YY_MOVE (that.value));
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
      case symbol_kind::S_FRACTION: // FRACTION
      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_VARIABLE: // VARIABLE
      case symbol_kind::S_TYPE: // TYPE
      case symbol_kind::S_vars: // vars
      case symbol_kind::S_expr: // expr
        value.move< std::shared_ptr<object::Object> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_type: // type
        value.move< std::shared_ptr<type::Type> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_arg_list: // arg_list
      case symbol_kind::S_expr_list: // expr_list
      case symbol_kind::S_var_list: // var_list
        value.move< std::vector<std::shared_ptr<object::Object>> > (YY_MOVE (s.value));
        break;

      case symbol_kind::S_type_list: // type_list
        value.move< std::vector<std::shared_ptr<type::Type>> > (YY_MOVE (s.value));
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
      case symbol_kind::S_FRACTION: // FRACTION
      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_VARIABLE: // VARIABLE
      case symbol_kind::S_TYPE: // TYPE
      case symbol_kind::S_vars: // vars
      case symbol_kind::S_expr: // expr
        value.YY_MOVE_OR_COPY< std::shared_ptr<object::Object> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type: // type
        value.YY_MOVE_OR_COPY< std::shared_ptr<type::Type> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_arg_list: // arg_list
      case symbol_kind::S_expr_list: // expr_list
      case symbol_kind::S_var_list: // var_list
        value.YY_MOVE_OR_COPY< std::vector<std::shared_ptr<object::Object>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type_list: // type_list
        value.YY_MOVE_OR_COPY< std::vector<std::shared_ptr<type::Type>> > (YY_MOVE (that.value));
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
      case symbol_kind::S_FRACTION: // FRACTION
      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_VARIABLE: // VARIABLE
      case symbol_kind::S_TYPE: // TYPE
      case symbol_kind::S_vars: // vars
      case symbol_kind::S_expr: // expr
        value.move< std::shared_ptr<object::Object> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type: // type
        value.move< std::shared_ptr<type::Type> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_arg_list: // arg_list
      case symbol_kind::S_expr_list: // expr_list
      case symbol_kind::S_var_list: // var_list
        value.move< std::vector<std::shared_ptr<object::Object>> > (YY_MOVE (that.value));
        break;

      case symbol_kind::S_type_list: // type_list
        value.move< std::vector<std::shared_ptr<type::Type>> > (YY_MOVE (that.value));
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
      case symbol_kind::S_FRACTION: // FRACTION
      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_VARIABLE: // VARIABLE
      case symbol_kind::S_TYPE: // TYPE
      case symbol_kind::S_vars: // vars
      case symbol_kind::S_expr: // expr
        value.copy< std::shared_ptr<object::Object> > (that.value);
        break;

      case symbol_kind::S_type: // type
        value.copy< std::shared_ptr<type::Type> > (that.value);
        break;

      case symbol_kind::S_arg_list: // arg_list
      case symbol_kind::S_expr_list: // expr_list
      case symbol_kind::S_var_list: // var_list
        value.copy< std::vector<std::shared_ptr<object::Object>> > (that.value);
        break;

      case symbol_kind::S_type_list: // type_list
        value.copy< std::vector<std::shared_ptr<type::Type>> > (that.value);
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
      case symbol_kind::S_FRACTION: // FRACTION
      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_VARIABLE: // VARIABLE
      case symbol_kind::S_TYPE: // TYPE
      case symbol_kind::S_vars: // vars
      case symbol_kind::S_expr: // expr
        value.move< std::shared_ptr<object::Object> > (that.value);
        break;

      case symbol_kind::S_type: // type
        value.move< std::shared_ptr<type::Type> > (that.value);
        break;

      case symbol_kind::S_arg_list: // arg_list
      case symbol_kind::S_expr_list: // expr_list
      case symbol_kind::S_var_list: // var_list
        value.move< std::vector<std::shared_ptr<object::Object>> > (that.value);
        break;

      case symbol_kind::S_type_list: // type_list
        value.move< std::vector<std::shared_ptr<type::Type>> > (that.value);
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
      case symbol_kind::S_FRACTION: // FRACTION
      case symbol_kind::S_STRING: // STRING
      case symbol_kind::S_IDENTIFIER: // IDENTIFIER
      case symbol_kind::S_VARIABLE: // VARIABLE
      case symbol_kind::S_TYPE: // TYPE
      case symbol_kind::S_vars: // vars
      case symbol_kind::S_expr: // expr
        yylhs.value.emplace< std::shared_ptr<object::Object> > ();
        break;

      case symbol_kind::S_type: // type
        yylhs.value.emplace< std::shared_ptr<type::Type> > ();
        break;

      case symbol_kind::S_arg_list: // arg_list
      case symbol_kind::S_expr_list: // expr_list
      case symbol_kind::S_var_list: // var_list
        yylhs.value.emplace< std::vector<std::shared_ptr<object::Object>> > ();
        break;

      case symbol_kind::S_type_list: // type_list
        yylhs.value.emplace< std::vector<std::shared_ptr<type::Type>> > ();
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
  case 2: // vars: VARIABLE
#line 79 "parser.y"
               { yylhs.value.as < std::shared_ptr<object::Object> > () = yystack_[0].value.as < std::shared_ptr<object::Object> > (); }
#line 819 "parser.cpp"
    break;

  case 3: // vars: expr '[' arg_list ']'
#line 80 "parser.y"
                            { yylhs.value.as < std::shared_ptr<object::Object> > () = exec_call(yystack_[3].value.as < std::shared_ptr<object::Object> > (),yystack_[1].value.as < std::vector<std::shared_ptr<object::Object>> > (),"[]",yystack_[2].location); }
#line 825 "parser.cpp"
    break;

  case 4: // vars: expr '(' arg_list ')'
#line 81 "parser.y"
                            { yylhs.value.as < std::shared_ptr<object::Object> > () = exec_call(yystack_[3].value.as < std::shared_ptr<object::Object> > (),yystack_[1].value.as < std::vector<std::shared_ptr<object::Object>> > (),"()",yystack_[2].location); }
#line 831 "parser.cpp"
    break;

  case 5: // type: TYPE
#line 83 "parser.y"
           { yylhs.value.as < std::shared_ptr<type::Type> > () = std::static_pointer_cast<type::Type>(yystack_[0].value.as < std::shared_ptr<object::Object> > ()); }
#line 837 "parser.cpp"
    break;

  case 6: // type: '`' expr
#line 84 "parser.y"
               { yylhs.value.as < std::shared_ptr<type::Type> > () = yystack_[0].value.as < std::shared_ptr<object::Object> > ()->type; }
#line 843 "parser.cpp"
    break;

  case 7: // type: '[' type ']'
#line 85 "parser.y"
                   { yylhs.value.as < std::shared_ptr<type::Type> > () = array::get_Array_type(yystack_[1].value.as < std::shared_ptr<type::Type> > ()); }
#line 849 "parser.cpp"
    break;

  case 8: // type: '(' type_list ')'
#line 86 "parser.y"
                        { yylhs.value.as < std::shared_ptr<type::Type> > () = tuple::get_Tuple_type(yystack_[1].value.as < std::vector<std::shared_ptr<type::Type>> > ()); }
#line 855 "parser.cpp"
    break;

  case 9: // type_list: type ',' type
#line 88 "parser.y"
                         { yylhs.value.as < std::vector<std::shared_ptr<type::Type>> > () = std::vector<std::shared_ptr<type::Type>>{yystack_[2].value.as < std::shared_ptr<type::Type> > (),yystack_[0].value.as < std::shared_ptr<type::Type> > ()}; }
#line 861 "parser.cpp"
    break;

  case 10: // type_list: type_list ',' type
#line 89 "parser.y"
                              { yylhs.value.as < std::vector<std::shared_ptr<type::Type>> > () = std::move(yystack_[2].value.as < std::vector<std::shared_ptr<type::Type>> > ()); yylhs.value.as < std::vector<std::shared_ptr<type::Type>> > ().push_back(yystack_[0].value.as < std::shared_ptr<type::Type> > ()); }
#line 867 "parser.cpp"
    break;

  case 11: // expr: INTEGER
#line 91 "parser.y"
              { yylhs.value.as < std::shared_ptr<object::Object> > () = yystack_[0].value.as < std::shared_ptr<object::Object> > (); }
#line 873 "parser.cpp"
    break;

  case 12: // expr: FRACTION
#line 92 "parser.y"
               { yylhs.value.as < std::shared_ptr<object::Object> > () = yystack_[0].value.as < std::shared_ptr<object::Object> > (); }
#line 879 "parser.cpp"
    break;

  case 13: // expr: STRING
#line 93 "parser.y"
             { yylhs.value.as < std::shared_ptr<object::Object> > () = yystack_[0].value.as < std::shared_ptr<object::Object> > (); }
#line 885 "parser.cpp"
    break;

  case 14: // expr: vars
#line 94 "parser.y"
           { yylhs.value.as < std::shared_ptr<object::Object> > () = static_pointer_cast<variable::Variable>(yystack_[0].value.as < std::shared_ptr<object::Object> > ())->get_value(); }
#line 891 "parser.cpp"
    break;

  case 15: // expr: expr '+' expr
#line 95 "parser.y"
                    { yylhs.value.as < std::shared_ptr<object::Object> > () = exec_binary_op(yystack_[2].value.as < std::shared_ptr<object::Object> > (),yystack_[0].value.as < std::shared_ptr<object::Object> > (),"+",yystack_[1].location); }
#line 897 "parser.cpp"
    break;

  case 16: // expr: expr '-' expr
#line 96 "parser.y"
                    { yylhs.value.as < std::shared_ptr<object::Object> > () = exec_binary_op(yystack_[2].value.as < std::shared_ptr<object::Object> > (),yystack_[0].value.as < std::shared_ptr<object::Object> > (),"-",yystack_[1].location); }
#line 903 "parser.cpp"
    break;

  case 17: // expr: expr '*' expr
#line 97 "parser.y"
                    { yylhs.value.as < std::shared_ptr<object::Object> > () = exec_binary_op(yystack_[2].value.as < std::shared_ptr<object::Object> > (),yystack_[0].value.as < std::shared_ptr<object::Object> > (),"*",yystack_[1].location); }
#line 909 "parser.cpp"
    break;

  case 18: // expr: expr '/' expr
#line 98 "parser.y"
                    { yylhs.value.as < std::shared_ptr<object::Object> > () = exec_binary_op(yystack_[2].value.as < std::shared_ptr<object::Object> > (),yystack_[0].value.as < std::shared_ptr<object::Object> > (),"/",yystack_[1].location); }
#line 915 "parser.cpp"
    break;

  case 19: // expr: expr '%' expr
#line 99 "parser.y"
                    { yylhs.value.as < std::shared_ptr<object::Object> > () = exec_binary_op(yystack_[2].value.as < std::shared_ptr<object::Object> > (),yystack_[0].value.as < std::shared_ptr<object::Object> > (),"%",yystack_[1].location); }
#line 921 "parser.cpp"
    break;

  case 20: // expr: '(' expr ')'
#line 100 "parser.y"
                   { yylhs.value.as < std::shared_ptr<object::Object> > () = yystack_[1].value.as < std::shared_ptr<object::Object> > (); }
#line 927 "parser.cpp"
    break;

  case 21: // expr: '+' expr
#line 101 "parser.y"
                           { yylhs.value.as < std::shared_ptr<object::Object> > () = exec_unary_op(yystack_[0].value.as < std::shared_ptr<object::Object> > (),"+",yystack_[1].location); }
#line 933 "parser.cpp"
    break;

  case 22: // expr: '-' expr
#line 102 "parser.y"
                           { yylhs.value.as < std::shared_ptr<object::Object> > () = exec_unary_op(yystack_[0].value.as < std::shared_ptr<object::Object> > (),"-",yystack_[1].location); }
#line 939 "parser.cpp"
    break;

  case 23: // expr: '~' expr
#line 103 "parser.y"
                           { yylhs.value.as < std::shared_ptr<object::Object> > () = exec_unary_op(yystack_[0].value.as < std::shared_ptr<object::Object> > (),"~",yystack_[1].location); }
#line 945 "parser.cpp"
    break;

  case 24: // expr: '(' expr_list ')'
#line 104 "parser.y"
                        { yylhs.value.as < std::shared_ptr<object::Object> > () = make_shared<tuple::Tuple>(yystack_[1].value.as < std::vector<std::shared_ptr<object::Object>> > ()); }
#line 951 "parser.cpp"
    break;

  case 25: // expr: '[' arg_list ']'
#line 105 "parser.y"
                       { yylhs.value.as < std::shared_ptr<object::Object> > () = exec_build_array(yystack_[1].value.as < std::vector<std::shared_ptr<object::Object>> > (),yystack_[1].location); }
#line 957 "parser.cpp"
    break;

  case 26: // expr: type '(' arg_list ')'
#line 106 "parser.y"
                            { yylhs.value.as < std::shared_ptr<object::Object> > () = exec_constructor(yystack_[3].value.as < std::shared_ptr<type::Type> > (),yystack_[1].value.as < std::vector<std::shared_ptr<object::Object>> > (),yystack_[2].location); }
#line 963 "parser.cpp"
    break;

  case 27: // expr: expr '?' expr ':' expr
#line 107 "parser.y"
                             { yylhs.value.as < std::shared_ptr<object::Object> > () = exec_conditional(yystack_[4].value.as < std::shared_ptr<object::Object> > (),yystack_[2].value.as < std::shared_ptr<object::Object> > (),yystack_[0].value.as < std::shared_ptr<object::Object> > (),yystack_[3].location); }
#line 969 "parser.cpp"
    break;

  case 28: // arg_list: %empty
#line 109 "parser.y"
                 { yylhs.value.as < std::vector<std::shared_ptr<object::Object>> > () = std::vector<std::shared_ptr<object::Object>>{}; }
#line 975 "parser.cpp"
    break;

  case 29: // arg_list: expr
#line 110 "parser.y"
               { yylhs.value.as < std::vector<std::shared_ptr<object::Object>> > () = std::vector<std::shared_ptr<object::Object>>{yystack_[0].value.as < std::shared_ptr<object::Object> > ()}; }
#line 981 "parser.cpp"
    break;

  case 30: // arg_list: arg_list ',' expr
#line 111 "parser.y"
                            { yylhs.value.as < std::vector<std::shared_ptr<object::Object>> > () = std::move(yystack_[2].value.as < std::vector<std::shared_ptr<object::Object>> > ()); yylhs.value.as < std::vector<std::shared_ptr<object::Object>> > ().push_back(yystack_[0].value.as < std::shared_ptr<object::Object> > ()); }
#line 987 "parser.cpp"
    break;

  case 31: // expr_list: expr ',' expr
#line 113 "parser.y"
                         { yylhs.value.as < std::vector<std::shared_ptr<object::Object>> > () = std::vector<std::shared_ptr<object::Object>>{yystack_[2].value.as < std::shared_ptr<object::Object> > (),yystack_[0].value.as < std::shared_ptr<object::Object> > ()}; }
#line 993 "parser.cpp"
    break;

  case 32: // expr_list: expr_list ',' expr
#line 114 "parser.y"
                              { yylhs.value.as < std::vector<std::shared_ptr<object::Object>> > () = std::move(yystack_[2].value.as < std::vector<std::shared_ptr<object::Object>> > ()); yylhs.value.as < std::vector<std::shared_ptr<object::Object>> > ().push_back(yystack_[0].value.as < std::shared_ptr<object::Object> > ()); }
#line 999 "parser.cpp"
    break;

  case 33: // var_list: IDENTIFIER
#line 116 "parser.y"
                     { yylhs.value.as < std::vector<std::shared_ptr<object::Object>> > () = std::vector<std::shared_ptr<object::Object>>{yystack_[0].value.as < std::shared_ptr<object::Object> > ()}; }
#line 1005 "parser.cpp"
    break;

  case 34: // var_list: vars
#line 117 "parser.y"
               { yylhs.value.as < std::vector<std::shared_ptr<object::Object>> > () = std::vector<std::shared_ptr<object::Object>>{yystack_[0].value.as < std::shared_ptr<object::Object> > ()}; }
#line 1011 "parser.cpp"
    break;

  case 35: // var_list: var_list ',' IDENTIFIER
#line 118 "parser.y"
                                  { yylhs.value.as < std::vector<std::shared_ptr<object::Object>> > () = std::move(yystack_[2].value.as < std::vector<std::shared_ptr<object::Object>> > ()); yylhs.value.as < std::vector<std::shared_ptr<object::Object>> > ().push_back(yystack_[0].value.as < std::shared_ptr<object::Object> > ()); }
#line 1017 "parser.cpp"
    break;

  case 36: // var_list: var_list ',' vars
#line 119 "parser.y"
                            { yylhs.value.as < std::vector<std::shared_ptr<object::Object>> > () = std::move(yystack_[2].value.as < std::vector<std::shared_ptr<object::Object>> > ()); yylhs.value.as < std::vector<std::shared_ptr<object::Object>> > ().push_back(yystack_[0].value.as < std::shared_ptr<object::Object> > ()); }
#line 1023 "parser.cpp"
    break;

  case 38: // stmt: expr
#line 123 "parser.y"
           { on_orphan_value(yystack_[0].value.as < std::shared_ptr<object::Object> > (),lexer,err_rp); }
#line 1029 "parser.cpp"
    break;

  case 39: // stmt: IDENTIFIER
#line 124 "parser.y"
                 { throw error::eval_error("Undefined variable: " + static_pointer_cast<variable::Variable>(yystack_[0].value.as < std::shared_ptr<object::Object> > ())->name, yystack_[0].location); }
#line 1035 "parser.cpp"
    break;

  case 40: // stmt: var_list '=' expr
#line 125 "parser.y"
                        { exec_assign(yystack_[2].value.as < std::vector<std::shared_ptr<object::Object>> > (),yystack_[0].value.as < std::shared_ptr<object::Object> > (),yystack_[1].location); }
#line 1041 "parser.cpp"
    break;

  case 41: // stmt: var_list ':' expr
#line 126 "parser.y"
                        { exec_declare(yystack_[2].value.as < std::vector<std::shared_ptr<object::Object>> > (),yystack_[0].value.as < std::shared_ptr<object::Object> > (),lexer,yystack_[1].location); }
#line 1047 "parser.cpp"
    break;

  case 42: // stmt: var_list FORWARD_DECLARE type
#line 127 "parser.y"
                                    { exec_declare(yystack_[2].value.as < std::vector<std::shared_ptr<object::Object>> > (),yystack_[0].value.as < std::shared_ptr<type::Type> > (),lexer,yystack_[1].location); }
#line 1053 "parser.cpp"
    break;

  case 43: // stmt: var_list ':' type
#line 128 "parser.y"
                        { exec_declare(yystack_[2].value.as < std::vector<std::shared_ptr<object::Object>> > (),yystack_[0].value.as < std::shared_ptr<type::Type> > (),lexer,yystack_[1].location); }
#line 1059 "parser.cpp"
    break;


#line 1063 "parser.cpp"

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
        context yyctx (*this, yyla);
        std::string msg = yysyntax_error_ (yyctx);
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

  /* Return YYSTR after stripping away unnecessary quotes and
     backslashes, so that it's suitable for yyerror.  The heuristic is
     that double-quoting is unnecessary unless the string contains an
     apostrophe, a comma, or backslash (other than backslash-backslash).
     YYSTR is taken from yytname.  */
  std::string
  Parser::yytnamerr_ (const char *yystr)
  {
    if (*yystr == '"')
      {
        std::string yyr;
        char const *yyp = yystr;

        for (;;)
          switch (*++yyp)
            {
            case '\'':
            case ',':
              goto do_not_strip_quotes;

            case '\\':
              if (*++yyp != '\\')
                goto do_not_strip_quotes;
              else
                goto append;

            append:
            default:
              yyr += *yyp;
              break;

            case '"':
              return yyr;
            }
      do_not_strip_quotes: ;
      }

    return yystr;
  }

  std::string
  Parser::symbol_name (symbol_kind_type yysymbol)
  {
    return yytnamerr_ (yytname_[yysymbol]);
  }



  // Parser::context.
  Parser::context::context (const Parser& yyparser, const symbol_type& yyla)
    : yyparser_ (yyparser)
    , yyla_ (yyla)
  {}

  int
  Parser::context::expected_tokens (symbol_kind_type yyarg[], int yyargn) const
  {
    // Actual number of expected tokens
    int yycount = 0;

    const int yyn = yypact_[+yyparser_.yystack_[0].state];
    if (!yy_pact_value_is_default_ (yyn))
      {
        /* Start YYX at -YYN if negative to avoid negative indexes in
           YYCHECK.  In other words, skip the first -YYN actions for
           this state because they are default actions.  */
        const int yyxbegin = yyn < 0 ? -yyn : 0;
        // Stay within bounds of both yycheck and yytname.
        const int yychecklim = yylast_ - yyn + 1;
        const int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
        for (int yyx = yyxbegin; yyx < yyxend; ++yyx)
          if (yycheck_[yyx + yyn] == yyx && yyx != symbol_kind::S_YYerror
              && !yy_table_value_is_error_ (yytable_[yyx + yyn]))
            {
              if (!yyarg)
                ++yycount;
              else if (yycount == yyargn)
                return 0;
              else
                yyarg[yycount++] = YY_CAST (symbol_kind_type, yyx);
            }
      }

    if (yyarg && yycount == 0 && 0 < yyargn)
      yyarg[0] = symbol_kind::S_YYEMPTY;
    return yycount;
  }






  int
  Parser::yy_syntax_error_arguments_ (const context& yyctx,
                                                 symbol_kind_type yyarg[], int yyargn) const
  {
    /* There are many possibilities here to consider:
       - If this state is a consistent state with a default action, then
         the only way this function was invoked is if the default action
         is an error action.  In that case, don't check for expected
         tokens because there are none.
       - The only way there can be no lookahead present (in yyla) is
         if this state is a consistent state with a default action.
         Thus, detecting the absence of a lookahead is sufficient to
         determine that there is no unexpected or expected token to
         report.  In that case, just report a simple "syntax error".
       - Don't assume there isn't a lookahead just because this state is
         a consistent state with a default action.  There might have
         been a previous inconsistent state, consistent state with a
         non-default action, or user semantic action that manipulated
         yyla.  (However, yyla is currently not documented for users.)
       - Of course, the expected token list depends on states to have
         correct lookahead information, and it depends on the parser not
         to perform extra reductions after fetching a lookahead from the
         scanner and before detecting a syntax error.  Thus, state merging
         (from LALR or IELR) and default reductions corrupt the expected
         token list.  However, the list is correct for canonical LR with
         one exception: it will still contain any token that will not be
         accepted due to an error action in a later state.
    */

    if (!yyctx.lookahead ().empty ())
      {
        if (yyarg)
          yyarg[0] = yyctx.token ();
        int yyn = yyctx.expected_tokens (yyarg ? yyarg + 1 : yyarg, yyargn - 1);
        return yyn + 1;
      }
    return 0;
  }

  // Generate an error message.
  std::string
  Parser::yysyntax_error_ (const context& yyctx) const
  {
    // Its maximum.
    enum { YYARGS_MAX = 5 };
    // Arguments of yyformat.
    symbol_kind_type yyarg[YYARGS_MAX];
    int yycount = yy_syntax_error_arguments_ (yyctx, yyarg, YYARGS_MAX);

    char const* yyformat = YY_NULLPTR;
    switch (yycount)
      {
#define YYCASE_(N, S)                         \
        case N:                               \
          yyformat = S;                       \
        break
      default: // Avoid compiler warnings.
        YYCASE_ (0, YY_("syntax error"));
        YYCASE_ (1, YY_("syntax error, unexpected %s"));
        YYCASE_ (2, YY_("syntax error, unexpected %s, expecting %s"));
        YYCASE_ (3, YY_("syntax error, unexpected %s, expecting %s or %s"));
        YYCASE_ (4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
        YYCASE_ (5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
#undef YYCASE_
      }

    std::string yyres;
    // Argument number.
    std::ptrdiff_t yyi = 0;
    for (char const* yyp = yyformat; *yyp; ++yyp)
      if (yyp[0] == '%' && yyp[1] == 's' && yyi < yycount)
        {
          yyres += symbol_name (yyarg[yyi++]);
          ++yyp;
        }
      else
        yyres += *yyp;
    return yyres;
  }


  const signed char Parser::yypact_ninf_ = -32;

  const signed char Parser::yytable_ninf_ = -40;

  const short
  Parser::yypact_[] =
  {
     -32,    63,   -32,   -32,   -32,   -32,   -32,   -24,   -32,   -32,
     110,   110,   110,   110,   110,   110,     5,    -7,   147,    16,
      29,   -32,    -9,   147,    10,     4,    41,   124,    58,   -32,
     -32,   157,   -32,   110,   110,   110,   110,   110,   110,   110,
     110,   110,    38,   110,   110,    87,   -32,   -32,   -32,   110,
      38,   -32,    38,   -32,   110,   -32,   110,    59,   137,    36,
      61,     1,     1,   -32,   -32,   -32,    38,    38,   -32,    -7,
     147,   147,   -32,    21,   147,   147,   -32,   -32,   147,   147,
     -32,   110,   -32,   -32,    13,     9,   147
  };

  const signed char
  Parser::yydefact_[] =
  {
      45,     0,     1,    44,    11,    12,    13,    33,     2,     5,
      28,     0,     0,     0,     0,     0,    14,     0,    38,     0,
       0,    14,     0,    29,     0,     0,     0,     0,     0,    21,
      22,     6,    23,    28,     0,    28,    28,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    46,     7,    25,     0,
       0,     8,     0,    20,     0,    24,     0,     0,     0,     0,
       0,    15,    16,    17,    18,    19,     0,     0,    42,    43,
      41,    40,    35,    14,     0,    30,     9,    10,    31,    32,
      26,     0,     3,     4,     0,     0,    27
  };

  const signed char
  Parser::yypgoto_[] =
  {
     -32,     0,     7,   -32,    -1,   -31,   -32,   -32,   -32,   -32
  };

  const signed char
  Parser::yydefgoto_[] =
  {
       0,    21,    17,    26,    23,    24,    28,    19,    20,     1
  };

  const signed char
  Parser::yytable_[] =
  {
      18,    16,    57,   -39,    59,    60,    47,    33,   -34,    33,
      27,    29,    30,    31,    32,   -34,   -34,    22,    25,    42,
      33,    39,    40,    41,   -36,    48,    43,    44,    47,    50,
     -34,   -36,   -36,    58,    50,    49,    61,    62,    63,    64,
      65,    45,    70,    71,    74,    73,   -36,     9,    75,    68,
      69,    82,    66,    78,    67,    79,    46,    76,    51,    77,
       0,    49,    14,     2,     3,     0,    52,     4,     5,     6,
       7,     8,     9,    84,    85,    55,    80,    10,    83,    11,
      86,    12,    13,    56,    49,     0,    49,    14,     0,    15,
     -37,     4,     5,     6,    72,     8,     9,     0,     0,     0,
       0,    10,     0,    11,     0,    12,    13,     0,     0,     0,
       0,    14,     0,    15,     4,     5,     6,     0,     8,     9,
       0,     0,     0,     0,    10,     0,    11,     0,    12,    13,
       0,     0,     0,     0,    14,     0,    15,    34,    35,     0,
      36,    53,    37,    38,    39,    40,    41,    81,     0,    54,
      34,    35,     0,    36,     0,    37,    38,    39,    40,    41,
      34,    35,     0,    36,     0,    37,    38,    39,    40,    41,
      34,    35,     0,     0,     0,    37,    38,    39,    40,    41
  };

  const signed char
  Parser::yycheck_[] =
  {
       1,     1,    33,    27,    35,    36,    15,    16,     3,    16,
      11,    12,    13,    14,    15,    10,    11,    10,    11,     3,
      16,    20,    21,    22,     3,    15,    10,    11,    15,    25,
      25,    10,    11,    34,    25,    25,    37,    38,    39,    40,
      41,    25,    43,    44,    45,    45,    25,     9,    49,    42,
      43,    15,    14,    54,    16,    56,    27,    50,    17,    52,
      -1,    25,    24,     0,     1,    -1,    25,     4,     5,     6,
       7,     8,     9,    66,    67,    17,    17,    14,    17,    16,
      81,    18,    19,    25,    25,    -1,    25,    24,    -1,    26,
      27,     4,     5,     6,     7,     8,     9,    -1,    -1,    -1,
      -1,    14,    -1,    16,    -1,    18,    19,    -1,    -1,    -1,
      -1,    24,    -1,    26,     4,     5,     6,    -1,     8,     9,
      -1,    -1,    -1,    -1,    14,    -1,    16,    -1,    18,    19,
      -1,    -1,    -1,    -1,    24,    -1,    26,    13,    14,    -1,
      16,    17,    18,    19,    20,    21,    22,    10,    -1,    25,
      13,    14,    -1,    16,    -1,    18,    19,    20,    21,    22,
      13,    14,    -1,    16,    -1,    18,    19,    20,    21,    22,
      13,    14,    -1,    -1,    -1,    18,    19,    20,    21,    22
  };

  const signed char
  Parser::yystos_[] =
  {
       0,    37,     0,     1,     4,     5,     6,     7,     8,     9,
      14,    16,    18,    19,    24,    26,    29,    30,    32,    35,
      36,    29,    30,    32,    33,    30,    31,    32,    34,    32,
      32,    32,    32,    16,    13,    14,    16,    18,    19,    20,
      21,    22,     3,    10,    11,    25,    27,    15,    15,    25,
      25,    17,    25,    17,    25,    17,    25,    33,    32,    33,
      33,    32,    32,    32,    32,    32,    14,    16,    30,    30,
      32,    32,     7,    29,    32,    32,    30,    30,    32,    32,
      17,    10,    15,    17,    30,    30,    32
  };

  const signed char
  Parser::yyr1_[] =
  {
       0,    28,    29,    29,    29,    30,    30,    30,    30,    31,
      31,    32,    32,    32,    32,    32,    32,    32,    32,    32,
      32,    32,    32,    32,    32,    32,    32,    32,    33,    33,
      33,    34,    34,    35,    35,    35,    35,    36,    36,    36,
      36,    36,    36,    36,    36,    37,    37
  };

  const signed char
  Parser::yyr2_[] =
  {
       0,     2,     1,     4,     4,     1,     2,     3,     3,     3,
       3,     1,     1,     1,     1,     3,     3,     3,     3,     3,
       3,     2,     2,     2,     3,     3,     4,     5,     0,     1,
       3,     3,     3,     1,     1,     3,     3,     0,     1,     1,
       3,     3,     3,     3,     1,     0,     3
  };


#if YYDEBUG || 1
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const Parser::yytname_[] =
  {
  "\"end of file\"", "error", "\"invalid token\"", "FORWARD_DECLARE",
  "INTEGER", "FRACTION", "STRING", "IDENTIFIER", "VARIABLE", "TYPE", "':'",
  "'='", "TYPE_PREC", "'?'", "'['", "']'", "'('", "')'", "'+'", "'-'",
  "'*'", "'/'", "'%'", "UNARY", "'`'", "','", "'~'", "';'", "$accept",
  "vars", "type", "type_list", "expr", "arg_list", "expr_list", "var_list",
  "stmt", "scope", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const unsigned char
  Parser::yyrline_[] =
  {
       0,    79,    79,    80,    81,    83,    84,    85,    86,    88,
      89,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   109,   110,
     111,   113,   114,   116,   117,   118,   119,   122,   123,   124,
     125,   126,   127,   128,   130,   134,   135
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
       2,     2,     2,     2,     2,     2,     2,    22,     2,     2,
      16,    17,    20,    18,    25,    19,     2,    21,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    10,    27,
       2,    11,     2,    13,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    14,     2,    15,     2,     2,    24,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,    26,     2,     2,     2,
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
       5,     6,     7,     8,     9,    12,    23
    };
    // Last valid token kind.
    const int code_max = 266;

    if (t <= 0)
      return symbol_kind::S_YYEOF;
    else if (t <= code_max)
      return static_cast <symbol_kind_type> (translate_table[t]);
    else
      return symbol_kind::S_YYUNDEF;
  }

#line 6 "parser.y"
} // parser
#line 1644 "parser.cpp"

#line 138 "parser.y"


namespace parser {
  void Parser::error(const Parser::location_type& location, const std::string& message) {
    err_rp->report(lexer, location, message);
  }
}
