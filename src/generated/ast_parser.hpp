// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton interface for Bison LALR(1) parsers in C++

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


/**
 ** \file src/generated/ast_parser.hpp
 ** Define the parser::parser class.
 */

// C++ LALR(1) parser skeleton written by Akim Demaille.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.

#ifndef YY_YY_SRC_GENERATED_AST_PARSER_HPP_INCLUDED
# define YY_YY_SRC_GENERATED_AST_PARSER_HPP_INCLUDED
// "%code requires" blocks.
#line 12 "src/syntax.ast.y"

#include "ast.h"
namespace at { extern AstPtr g_parse_result; }

#line 54 "src/generated/ast_parser.hpp"


# include <cstdlib> // std::abort
# include <iostream>
# include <stdexcept>
# include <string>
# include <vector>

#if defined __cplusplus
# define YY_CPLUSPLUS __cplusplus
#else
# define YY_CPLUSPLUS 199711L
#endif

// Support move semantics when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_MOVE           std::move
# define YY_MOVE_OR_COPY   move
# define YY_MOVE_REF(Type) Type&&
# define YY_RVREF(Type)    Type&&
# define YY_COPY(Type)     Type
#else
# define YY_MOVE
# define YY_MOVE_OR_COPY   copy
# define YY_MOVE_REF(Type) Type&
# define YY_RVREF(Type)    const Type&
# define YY_COPY(Type)     const Type&
#endif

// Support noexcept when possible.
#if 201103L <= YY_CPLUSPLUS
# define YY_NOEXCEPT noexcept
# define YY_NOTHROW
#else
# define YY_NOEXCEPT
# define YY_NOTHROW throw ()
#endif

// Support constexpr when possible.
#if 201703 <= YY_CPLUSPLUS
# define YY_CONSTEXPR constexpr
#else
# define YY_CONSTEXPR
#endif
# include "location.hh"


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

#line 6 "src/syntax.ast.y"
namespace parser {
#line 190 "src/generated/ast_parser.hpp"




  /// A Bison parser.
  class Parser
  {
  public:
#ifdef YYSTYPE
# ifdef __GNUC__
#  pragma GCC message "bison: do not #define YYSTYPE in C++, use %define api.value.type"
# endif
    typedef YYSTYPE value_type;
#else
  /// A buffer to store and retrieve objects.
  ///
  /// Sort of a variant, but does not keep track of the nature
  /// of the stored data, since that knowledge is available
  /// via the current parser state.
  class value_type
  {
  public:
    /// Type of *this.
    typedef value_type self_type;

    /// Empty construction.
    value_type () YY_NOEXCEPT
      : yyraw_ ()
    {}

    /// Construct and fill.
    template <typename T>
    value_type (YY_RVREF (T) t)
    {
      new (yyas_<T> ()) T (YY_MOVE (t));
    }

#if 201103L <= YY_CPLUSPLUS
    /// Non copyable.
    value_type (const self_type&) = delete;
    /// Non copyable.
    self_type& operator= (const self_type&) = delete;
#endif

    /// Destruction, allowed only if empty.
    ~value_type () YY_NOEXCEPT
    {}

# if 201103L <= YY_CPLUSPLUS
    /// Instantiate a \a T in here from \a t.
    template <typename T, typename... U>
    T&
    emplace (U&&... u)
    {
      return *new (yyas_<T> ()) T (std::forward <U>(u)...);
    }
# else
    /// Instantiate an empty \a T in here.
    template <typename T>
    T&
    emplace ()
    {
      return *new (yyas_<T> ()) T ();
    }

    /// Instantiate a \a T in here from \a t.
    template <typename T>
    T&
    emplace (const T& t)
    {
      return *new (yyas_<T> ()) T (t);
    }
# endif

    /// Instantiate an empty \a T in here.
    /// Obsolete, use emplace.
    template <typename T>
    T&
    build ()
    {
      return emplace<T> ();
    }

    /// Instantiate a \a T in here from \a t.
    /// Obsolete, use emplace.
    template <typename T>
    T&
    build (const T& t)
    {
      return emplace<T> (t);
    }

    /// Accessor to a built \a T.
    template <typename T>
    T&
    as () YY_NOEXCEPT
    {
      return *yyas_<T> ();
    }

    /// Const accessor to a built \a T (for %printer).
    template <typename T>
    const T&
    as () const YY_NOEXCEPT
    {
      return *yyas_<T> ();
    }

    /// Swap the content with \a that, of same type.
    ///
    /// Both variants must be built beforehand, because swapping the actual
    /// data requires reading it (with as()), and this is not possible on
    /// unconstructed variants: it would require some dynamic testing, which
    /// should not be the variant's responsibility.
    /// Swapping between built and (possibly) non-built is done with
    /// self_type::move ().
    template <typename T>
    void
    swap (self_type& that) YY_NOEXCEPT
    {
      std::swap (as<T> (), that.as<T> ());
    }

    /// Move the content of \a that to this.
    ///
    /// Destroys \a that.
    template <typename T>
    void
    move (self_type& that)
    {
# if 201103L <= YY_CPLUSPLUS
      emplace<T> (std::move (that.as<T> ()));
# else
      emplace<T> ();
      swap<T> (that);
# endif
      that.destroy<T> ();
    }

# if 201103L <= YY_CPLUSPLUS
    /// Move the content of \a that to this.
    template <typename T>
    void
    move (self_type&& that)
    {
      emplace<T> (std::move (that.as<T> ()));
      that.destroy<T> ();
    }
#endif

    /// Copy the content of \a that to this.
    template <typename T>
    void
    copy (const self_type& that)
    {
      emplace<T> (that.as<T> ());
    }

    /// Destroy the stored \a T.
    template <typename T>
    void
    destroy ()
    {
      as<T> ().~T ();
    }

  private:
#if YY_CPLUSPLUS < 201103L
    /// Non copyable.
    value_type (const self_type&);
    /// Non copyable.
    self_type& operator= (const self_type&);
#endif

    /// Accessor to raw memory as \a T.
    template <typename T>
    T*
    yyas_ () YY_NOEXCEPT
    {
      void *yyp = yyraw_;
      return static_cast<T*> (yyp);
     }

    /// Const accessor to raw memory as \a T.
    template <typename T>
    const T*
    yyas_ () const YY_NOEXCEPT
    {
      const void *yyp = yyraw_;
      return static_cast<const T*> (yyp);
     }

    /// An auxiliary type to compute the largest semantic type.
    union union_type
    {
      // INTEGER
      // DECIMAL
      // STRING
      // IMPORT
      // IDENTIFIER
      // "::"
      // "<|"
      // "|>"
      // "||"
      // "&&"
      // "^^"
      // "<="
      // "=="
      // ">="
      // "!="
      // "<{"
      // "}>"
      // "->"
      // "@?"
      // "@<"
      // "$^"
      // "$!"
      // "@$"
      // "#$"
      // "^:"
      // "~:"
      // "!:"
      // "!^:"
      // "!~:"
      // ":="
      // "!:="
      // "^="
      // '+'
      // '-'
      // '*'
      // '/'
      // '%'
      // '^'
      // '|'
      // '&'
      // '<'
      // '>'
      // ':'
      // '='
      // '?'
      // '['
      // ']'
      // '('
      // ')'
      // '{'
      // '}'
      // '.'
      // '`'
      // '!'
      // '@'
      // '#'
      // '$'
      // '~'
      // ','
      // ';'
      // expr
      // func_arg_declare
      // func_arg_declare_list
      // expr_list
      // type
      // base_type
      // complex_type
      // type_list
      // class
      // template_arg_declare_list
      // template_arg_list
      // anonymous_template
      // declare
      // cast_stmt
      // cast_stmt_list
      // tuple
      // id_tuple_list
      // stmt
      // scope
      // start
      char dummy1[sizeof (at::AstPtr)];
    };

    /// The size of the largest semantic type.
    enum { size = sizeof (union_type) };

    /// A buffer to store semantic values.
    union
    {
      /// Strongest alignment constraints.
      long double yyalign_me_;
      /// A buffer large enough to store any of the semantic values.
      char yyraw_[size];
    };
  };

#endif
    /// Backward compatibility (Bison 3.8).
    typedef value_type semantic_type;

    /// Symbol locations.
    typedef location location_type;

    /// Syntax errors thrown from user actions.
    struct syntax_error : std::runtime_error
    {
      syntax_error (const location_type& l, const std::string& m)
        : std::runtime_error (m)
        , location (l)
      {}

      syntax_error (const syntax_error& s)
        : std::runtime_error (s.what ())
        , location (s.location)
      {}

      ~syntax_error () YY_NOEXCEPT YY_NOTHROW;

      location_type location;
    };

    /// Token kinds.
    struct token
    {
      enum token_kind_type
      {
        YYEMPTY = -2,
    YYEOF = 0,                     // "end of file"
    YYerror = 256,                 // error
    YYUNDEF = 257,                 // "invalid token"
    INTEGER = 258,                 // INTEGER
    DECIMAL = 259,                 // DECIMAL
    STRING = 260,                  // STRING
    IMPORT = 261,                  // IMPORT
    IDENTIFIER = 262,              // IDENTIFIER
    SCOPE = 263,                   // "::"
    TEMPLATE_OPEN = 264,           // "<|"
    TEMPLATE_CLOSE = 265,          // "|>"
    OROR = 266,                    // "||"
    ANDAND = 267,                  // "&&"
    XORXOR = 268,                  // "^^"
    LE = 269,                      // "<="
    EQEQ = 270,                    // "=="
    GE = 271,                      // ">="
    NE = 272,                      // "!="
    CONSTRUCT_OPEN = 273,          // "<{"
    CONSTRUCT_CLOSE = 274,         // "}>"
    ARROW = 275,                   // "->"
    FUNPTR = 276,                  // "@?"
    RETURN = 277,                  // "@<"
    CONTINUE = 278,                // "$^"
    BREAK = 279,                   // "$!"
    SELF_FUNC = 280,               // "@$"
    SELF_CLASS = 281,              // "#$"
    PROT = 282,                    // "^:"
    PRIV = 283,                    // "~:"
    CONST_PUB = 284,               // "!:"
    CONST_PROT = 285,              // "!^:"
    CONST_PRIV = 286,              // "!~:"
    ALIAS_PUBLIC = 287,            // ":="
    ALIAS_PRIVATE = 288,           // "!:="
    OVERLOAD = 289,                // "^="
    TYPE_REDUCE = 290,             // TYPE_REDUCE
    UNARY = 291                    // UNARY
      };
      /// Backward compatibility alias (Bison 3.6).
      typedef token_kind_type yytokentype;
    };

    /// Token kind, as returned by yylex.
    typedef token::token_kind_type token_kind_type;

    /// Backward compatibility alias (Bison 3.6).
    typedef token_kind_type token_type;

    /// Symbol kinds.
    struct symbol_kind
    {
      enum symbol_kind_type
      {
        YYNTOKENS = 65, ///< Number of tokens.
        S_YYEMPTY = -2,
        S_YYEOF = 0,                             // "end of file"
        S_YYerror = 1,                           // error
        S_YYUNDEF = 2,                           // "invalid token"
        S_INTEGER = 3,                           // INTEGER
        S_DECIMAL = 4,                           // DECIMAL
        S_STRING = 5,                            // STRING
        S_IMPORT = 6,                            // IMPORT
        S_IDENTIFIER = 7,                        // IDENTIFIER
        S_SCOPE = 8,                             // "::"
        S_TEMPLATE_OPEN = 9,                     // "<|"
        S_TEMPLATE_CLOSE = 10,                   // "|>"
        S_OROR = 11,                             // "||"
        S_ANDAND = 12,                           // "&&"
        S_XORXOR = 13,                           // "^^"
        S_LE = 14,                               // "<="
        S_EQEQ = 15,                             // "=="
        S_GE = 16,                               // ">="
        S_NE = 17,                               // "!="
        S_CONSTRUCT_OPEN = 18,                   // "<{"
        S_CONSTRUCT_CLOSE = 19,                  // "}>"
        S_ARROW = 20,                            // "->"
        S_FUNPTR = 21,                           // "@?"
        S_RETURN = 22,                           // "@<"
        S_CONTINUE = 23,                         // "$^"
        S_BREAK = 24,                            // "$!"
        S_SELF_FUNC = 25,                        // "@$"
        S_SELF_CLASS = 26,                       // "#$"
        S_PROT = 27,                             // "^:"
        S_PRIV = 28,                             // "~:"
        S_CONST_PUB = 29,                        // "!:"
        S_CONST_PROT = 30,                       // "!^:"
        S_CONST_PRIV = 31,                       // "!~:"
        S_ALIAS_PUBLIC = 32,                     // ":="
        S_ALIAS_PRIVATE = 33,                    // "!:="
        S_OVERLOAD = 34,                         // "^="
        S_35_ = 35,                              // '+'
        S_36_ = 36,                              // '-'
        S_37_ = 37,                              // '*'
        S_38_ = 38,                              // '/'
        S_39_ = 39,                              // '%'
        S_40_ = 40,                              // '^'
        S_41_ = 41,                              // '|'
        S_42_ = 42,                              // '&'
        S_43_ = 43,                              // '<'
        S_44_ = 44,                              // '>'
        S_45_ = 45,                              // ':'
        S_46_ = 46,                              // '='
        S_47_ = 47,                              // '?'
        S_48_ = 48,                              // '['
        S_49_ = 49,                              // ']'
        S_50_ = 50,                              // '('
        S_51_ = 51,                              // ')'
        S_52_ = 52,                              // '{'
        S_53_ = 53,                              // '}'
        S_54_ = 54,                              // '.'
        S_55_ = 55,                              // '`'
        S_56_ = 56,                              // '!'
        S_57_ = 57,                              // '@'
        S_58_ = 58,                              // '#'
        S_59_ = 59,                              // '$'
        S_60_ = 60,                              // '~'
        S_61_ = 61,                              // ','
        S_62_ = 62,                              // ';'
        S_TYPE_REDUCE = 63,                      // TYPE_REDUCE
        S_UNARY = 64,                            // UNARY
        S_YYACCEPT = 65,                         // $accept
        S_expr = 66,                             // expr
        S_func_arg_declare = 67,                 // func_arg_declare
        S_func_arg_declare_list = 68,            // func_arg_declare_list
        S_expr_list = 69,                        // expr_list
        S_type = 70,                             // type
        S_base_type = 71,                        // base_type
        S_complex_type = 72,                     // complex_type
        S_type_list = 73,                        // type_list
        S_class = 74,                            // class
        S_template_arg_declare_list = 75,        // template_arg_declare_list
        S_template_arg_list = 76,                // template_arg_list
        S_anonymous_template = 77,               // anonymous_template
        S_declare = 78,                          // declare
        S_cast_stmt = 79,                        // cast_stmt
        S_cast_stmt_list = 80,                   // cast_stmt_list
        S_tuple = 81,                            // tuple
        S_id_tuple_list = 82,                    // id_tuple_list
        S_stmt = 83,                             // stmt
        S_scope = 84,                            // scope
        S_start = 85                             // start
      };
    };

    /// (Internal) symbol kind.
    typedef symbol_kind::symbol_kind_type symbol_kind_type;

    /// The number of tokens.
    static const symbol_kind_type YYNTOKENS = symbol_kind::YYNTOKENS;

    /// A complete symbol.
    ///
    /// Expects its Base type to provide access to the symbol kind
    /// via kind ().
    ///
    /// Provide access to semantic value and location.
    template <typename Base>
    struct basic_symbol : Base
    {
      /// Alias to Base.
      typedef Base super_type;

      /// Default constructor.
      basic_symbol () YY_NOEXCEPT
        : value ()
        , location ()
      {}

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      basic_symbol (basic_symbol&& that)
        : Base (std::move (that))
        , value ()
        , location (std::move (that.location))
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
        value.move< at::AstPtr > (std::move (that.value));
        break;

      default:
        break;
    }

      }
#endif

      /// Copy constructor.
      basic_symbol (const basic_symbol& that);

      /// Constructors for typed symbols.
#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, location_type&& l)
        : Base (t)
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const location_type& l)
        : Base (t)
        , location (l)
      {}
#endif

#if 201103L <= YY_CPLUSPLUS
      basic_symbol (typename Base::kind_type t, at::AstPtr&& v, location_type&& l)
        : Base (t)
        , value (std::move (v))
        , location (std::move (l))
      {}
#else
      basic_symbol (typename Base::kind_type t, const at::AstPtr& v, const location_type& l)
        : Base (t)
        , value (v)
        , location (l)
      {}
#endif

      /// Destroy the symbol.
      ~basic_symbol ()
      {
        clear ();
      }



      /// Destroy contents, and record that is empty.
      void clear () YY_NOEXCEPT
      {
        // User destructor.
        symbol_kind_type yykind = this->kind ();
        basic_symbol<Base>& yysym = *this;
        (void) yysym;
        switch (yykind)
        {
       default:
          break;
        }

        // Value type destructor.
switch (yykind)
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
        value.template destroy< at::AstPtr > ();
        break;

      default:
        break;
    }

        Base::clear ();
      }

#if YYDEBUG || 0
      /// The user-facing name of this symbol.
      const char *name () const YY_NOEXCEPT
      {
        return Parser::symbol_name (this->kind ());
      }
#endif // #if YYDEBUG || 0


      /// Backward compatibility (Bison 3.6).
      symbol_kind_type type_get () const YY_NOEXCEPT;

      /// Whether empty.
      bool empty () const YY_NOEXCEPT;

      /// Destructive move, \a s is emptied into this.
      void move (basic_symbol& s);

      /// The semantic value.
      value_type value;

      /// The location.
      location_type location;

    private:
#if YY_CPLUSPLUS < 201103L
      /// Assignment operator.
      basic_symbol& operator= (const basic_symbol& that);
#endif
    };

    /// Type access provider for token (enum) based symbols.
    struct by_kind
    {
      /// The symbol kind as needed by the constructor.
      typedef token_kind_type kind_type;

      /// Default constructor.
      by_kind () YY_NOEXCEPT;

#if 201103L <= YY_CPLUSPLUS
      /// Move constructor.
      by_kind (by_kind&& that) YY_NOEXCEPT;
#endif

      /// Copy constructor.
      by_kind (const by_kind& that) YY_NOEXCEPT;

      /// Constructor from (external) token numbers.
      by_kind (kind_type t) YY_NOEXCEPT;



      /// Record that this symbol is empty.
      void clear () YY_NOEXCEPT;

      /// Steal the symbol kind from \a that.
      void move (by_kind& that);

      /// The (internal) type number (corresponding to \a type).
      /// \a empty when empty.
      symbol_kind_type kind () const YY_NOEXCEPT;

      /// Backward compatibility (Bison 3.6).
      symbol_kind_type type_get () const YY_NOEXCEPT;

      /// The symbol kind.
      /// \a S_YYEMPTY when empty.
      symbol_kind_type kind_;
    };

    /// Backward compatibility for a private implementation detail (Bison 3.6).
    typedef by_kind by_type;

    /// "External" symbols: returned by the scanner.
    struct symbol_type : basic_symbol<by_kind>
    {
      /// Superclass.
      typedef basic_symbol<by_kind> super_type;

      /// Empty symbol.
      symbol_type () YY_NOEXCEPT {}

      /// Constructor for valueless symbols, and symbols from each type.
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, location_type l)
        : super_type (token_kind_type (tok), std::move (l))
#else
      symbol_type (int tok, const location_type& l)
        : super_type (token_kind_type (tok), l)
#endif
      {}
#if 201103L <= YY_CPLUSPLUS
      symbol_type (int tok, at::AstPtr v, location_type l)
        : super_type (token_kind_type (tok), std::move (v), std::move (l))
#else
      symbol_type (int tok, const at::AstPtr& v, const location_type& l)
        : super_type (token_kind_type (tok), v, l)
#endif
      {}
    };

    /// Build a parser object.
    Parser ();
    virtual ~Parser ();

#if 201103L <= YY_CPLUSPLUS
    /// Non copyable.
    Parser (const Parser&) = delete;
    /// Non copyable.
    Parser& operator= (const Parser&) = delete;
#endif

    /// Parse.  An alias for parse ().
    /// \returns  0 iff parsing succeeded.
    int operator() ();

    /// Parse.
    /// \returns  0 iff parsing succeeded.
    virtual int parse ();

#if YYDEBUG
    /// The current debugging stream.
    std::ostream& debug_stream () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging stream.
    void set_debug_stream (std::ostream &);

    /// Type for debugging levels.
    typedef int debug_level_type;
    /// The current debugging level.
    debug_level_type debug_level () const YY_ATTRIBUTE_PURE;
    /// Set the current debugging level.
    void set_debug_level (debug_level_type l);
#endif

    /// Report a syntax error.
    /// \param loc    where the syntax error is found.
    /// \param msg    a description of the syntax error.
    virtual void error (const location_type& loc, const std::string& msg);

    /// Report a syntax error.
    void error (const syntax_error& err);

#if YYDEBUG || 0
    /// The user-facing name of the symbol whose (internal) number is
    /// YYSYMBOL.  No bounds checking.
    static const char *symbol_name (symbol_kind_type yysymbol);
#endif // #if YYDEBUG || 0


    // Implementation of make_symbol for each token kind.
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_YYEOF (location_type l)
      {
        return symbol_type (token::YYEOF, std::move (l));
      }
#else
      static
      symbol_type
      make_YYEOF (const location_type& l)
      {
        return symbol_type (token::YYEOF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_YYerror (location_type l)
      {
        return symbol_type (token::YYerror, std::move (l));
      }
#else
      static
      symbol_type
      make_YYerror (const location_type& l)
      {
        return symbol_type (token::YYerror, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_YYUNDEF (location_type l)
      {
        return symbol_type (token::YYUNDEF, std::move (l));
      }
#else
      static
      symbol_type
      make_YYUNDEF (const location_type& l)
      {
        return symbol_type (token::YYUNDEF, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_INTEGER (at::AstPtr v, location_type l)
      {
        return symbol_type (token::INTEGER, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_INTEGER (const at::AstPtr& v, const location_type& l)
      {
        return symbol_type (token::INTEGER, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_DECIMAL (at::AstPtr v, location_type l)
      {
        return symbol_type (token::DECIMAL, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_DECIMAL (const at::AstPtr& v, const location_type& l)
      {
        return symbol_type (token::DECIMAL, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_STRING (at::AstPtr v, location_type l)
      {
        return symbol_type (token::STRING, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_STRING (const at::AstPtr& v, const location_type& l)
      {
        return symbol_type (token::STRING, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_IMPORT (at::AstPtr v, location_type l)
      {
        return symbol_type (token::IMPORT, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_IMPORT (const at::AstPtr& v, const location_type& l)
      {
        return symbol_type (token::IMPORT, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_IDENTIFIER (at::AstPtr v, location_type l)
      {
        return symbol_type (token::IDENTIFIER, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_IDENTIFIER (const at::AstPtr& v, const location_type& l)
      {
        return symbol_type (token::IDENTIFIER, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SCOPE (at::AstPtr v, location_type l)
      {
        return symbol_type (token::SCOPE, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_SCOPE (const at::AstPtr& v, const location_type& l)
      {
        return symbol_type (token::SCOPE, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TEMPLATE_OPEN (at::AstPtr v, location_type l)
      {
        return symbol_type (token::TEMPLATE_OPEN, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TEMPLATE_OPEN (const at::AstPtr& v, const location_type& l)
      {
        return symbol_type (token::TEMPLATE_OPEN, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TEMPLATE_CLOSE (at::AstPtr v, location_type l)
      {
        return symbol_type (token::TEMPLATE_CLOSE, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_TEMPLATE_CLOSE (const at::AstPtr& v, const location_type& l)
      {
        return symbol_type (token::TEMPLATE_CLOSE, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OROR (at::AstPtr v, location_type l)
      {
        return symbol_type (token::OROR, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_OROR (const at::AstPtr& v, const location_type& l)
      {
        return symbol_type (token::OROR, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ANDAND (at::AstPtr v, location_type l)
      {
        return symbol_type (token::ANDAND, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_ANDAND (const at::AstPtr& v, const location_type& l)
      {
        return symbol_type (token::ANDAND, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_XORXOR (at::AstPtr v, location_type l)
      {
        return symbol_type (token::XORXOR, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_XORXOR (const at::AstPtr& v, const location_type& l)
      {
        return symbol_type (token::XORXOR, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_LE (at::AstPtr v, location_type l)
      {
        return symbol_type (token::LE, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_LE (const at::AstPtr& v, const location_type& l)
      {
        return symbol_type (token::LE, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_EQEQ (at::AstPtr v, location_type l)
      {
        return symbol_type (token::EQEQ, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_EQEQ (const at::AstPtr& v, const location_type& l)
      {
        return symbol_type (token::EQEQ, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_GE (at::AstPtr v, location_type l)
      {
        return symbol_type (token::GE, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_GE (const at::AstPtr& v, const location_type& l)
      {
        return symbol_type (token::GE, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_NE (at::AstPtr v, location_type l)
      {
        return symbol_type (token::NE, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_NE (const at::AstPtr& v, const location_type& l)
      {
        return symbol_type (token::NE, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CONSTRUCT_OPEN (at::AstPtr v, location_type l)
      {
        return symbol_type (token::CONSTRUCT_OPEN, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_CONSTRUCT_OPEN (const at::AstPtr& v, const location_type& l)
      {
        return symbol_type (token::CONSTRUCT_OPEN, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CONSTRUCT_CLOSE (at::AstPtr v, location_type l)
      {
        return symbol_type (token::CONSTRUCT_CLOSE, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_CONSTRUCT_CLOSE (const at::AstPtr& v, const location_type& l)
      {
        return symbol_type (token::CONSTRUCT_CLOSE, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ARROW (at::AstPtr v, location_type l)
      {
        return symbol_type (token::ARROW, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_ARROW (const at::AstPtr& v, const location_type& l)
      {
        return symbol_type (token::ARROW, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_FUNPTR (at::AstPtr v, location_type l)
      {
        return symbol_type (token::FUNPTR, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_FUNPTR (const at::AstPtr& v, const location_type& l)
      {
        return symbol_type (token::FUNPTR, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_RETURN (at::AstPtr v, location_type l)
      {
        return symbol_type (token::RETURN, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_RETURN (const at::AstPtr& v, const location_type& l)
      {
        return symbol_type (token::RETURN, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CONTINUE (at::AstPtr v, location_type l)
      {
        return symbol_type (token::CONTINUE, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_CONTINUE (const at::AstPtr& v, const location_type& l)
      {
        return symbol_type (token::CONTINUE, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_BREAK (at::AstPtr v, location_type l)
      {
        return symbol_type (token::BREAK, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_BREAK (const at::AstPtr& v, const location_type& l)
      {
        return symbol_type (token::BREAK, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SELF_FUNC (at::AstPtr v, location_type l)
      {
        return symbol_type (token::SELF_FUNC, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_SELF_FUNC (const at::AstPtr& v, const location_type& l)
      {
        return symbol_type (token::SELF_FUNC, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_SELF_CLASS (at::AstPtr v, location_type l)
      {
        return symbol_type (token::SELF_CLASS, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_SELF_CLASS (const at::AstPtr& v, const location_type& l)
      {
        return symbol_type (token::SELF_CLASS, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PROT (at::AstPtr v, location_type l)
      {
        return symbol_type (token::PROT, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_PROT (const at::AstPtr& v, const location_type& l)
      {
        return symbol_type (token::PROT, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_PRIV (at::AstPtr v, location_type l)
      {
        return symbol_type (token::PRIV, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_PRIV (const at::AstPtr& v, const location_type& l)
      {
        return symbol_type (token::PRIV, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CONST_PUB (at::AstPtr v, location_type l)
      {
        return symbol_type (token::CONST_PUB, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_CONST_PUB (const at::AstPtr& v, const location_type& l)
      {
        return symbol_type (token::CONST_PUB, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CONST_PROT (at::AstPtr v, location_type l)
      {
        return symbol_type (token::CONST_PROT, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_CONST_PROT (const at::AstPtr& v, const location_type& l)
      {
        return symbol_type (token::CONST_PROT, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_CONST_PRIV (at::AstPtr v, location_type l)
      {
        return symbol_type (token::CONST_PRIV, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_CONST_PRIV (const at::AstPtr& v, const location_type& l)
      {
        return symbol_type (token::CONST_PRIV, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ALIAS_PUBLIC (at::AstPtr v, location_type l)
      {
        return symbol_type (token::ALIAS_PUBLIC, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_ALIAS_PUBLIC (const at::AstPtr& v, const location_type& l)
      {
        return symbol_type (token::ALIAS_PUBLIC, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_ALIAS_PRIVATE (at::AstPtr v, location_type l)
      {
        return symbol_type (token::ALIAS_PRIVATE, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_ALIAS_PRIVATE (const at::AstPtr& v, const location_type& l)
      {
        return symbol_type (token::ALIAS_PRIVATE, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_OVERLOAD (at::AstPtr v, location_type l)
      {
        return symbol_type (token::OVERLOAD, std::move (v), std::move (l));
      }
#else
      static
      symbol_type
      make_OVERLOAD (const at::AstPtr& v, const location_type& l)
      {
        return symbol_type (token::OVERLOAD, v, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_TYPE_REDUCE (location_type l)
      {
        return symbol_type (token::TYPE_REDUCE, std::move (l));
      }
#else
      static
      symbol_type
      make_TYPE_REDUCE (const location_type& l)
      {
        return symbol_type (token::TYPE_REDUCE, l);
      }
#endif
#if 201103L <= YY_CPLUSPLUS
      static
      symbol_type
      make_UNARY (location_type l)
      {
        return symbol_type (token::UNARY, std::move (l));
      }
#else
      static
      symbol_type
      make_UNARY (const location_type& l)
      {
        return symbol_type (token::UNARY, l);
      }
#endif


  private:
#if YY_CPLUSPLUS < 201103L
    /// Non copyable.
    Parser (const Parser&);
    /// Non copyable.
    Parser& operator= (const Parser&);
#endif


    /// Stored state numbers (used for stacks).
    typedef short state_type;

    /// Compute post-reduction state.
    /// \param yystate   the current state
    /// \param yysym     the nonterminal to push on the stack
    static state_type yy_lr_goto_state_ (state_type yystate, int yysym);

    /// Whether the given \c yypact_ value indicates a defaulted state.
    /// \param yyvalue   the value to check
    static bool yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT;

    /// Whether the given \c yytable_ value indicates a syntax error.
    /// \param yyvalue   the value to check
    static bool yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT;

    static const signed char yypact_ninf_;
    static const short yytable_ninf_;

    /// Convert a scanner token kind \a t to a symbol kind.
    /// In theory \a t should be a token_kind_type, but character literals
    /// are valid, yet not members of the token_kind_type enum.
    static symbol_kind_type yytranslate_ (int t) YY_NOEXCEPT;

#if YYDEBUG || 0
    /// For a symbol, its name in clear.
    static const char* const yytname_[];
#endif // #if YYDEBUG || 0


    // Tables.
    // YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
    // STATE-NUM.
    static const short yypact_[];

    // YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
    // Performed when YYTABLE does not specify something else to do.  Zero
    // means the default is an error.
    static const unsigned char yydefact_[];

    // YYPGOTO[NTERM-NUM].
    static const short yypgoto_[];

    // YYDEFGOTO[NTERM-NUM].
    static const short yydefgoto_[];

    // YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
    // positive, shift that token.  If negative, reduce the rule whose
    // number is the opposite.  If YYTABLE_NINF, syntax error.
    static const short yytable_[];

    static const short yycheck_[];

    // YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
    // state STATE-NUM.
    static const signed char yystos_[];

    // YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.
    static const signed char yyr1_[];

    // YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.
    static const signed char yyr2_[];


#if YYDEBUG
    // YYRLINE[YYN] -- Source line where rule number YYN was defined.
    static const unsigned char yyrline_[];
    /// Report on the debug stream that the rule \a r is going to be reduced.
    virtual void yy_reduce_print_ (int r) const;
    /// Print the state stack on the debug stream.
    virtual void yy_stack_print_ () const;

    /// Debugging level.
    int yydebug_;
    /// Debug stream.
    std::ostream* yycdebug_;

    /// \brief Display a symbol kind, value and location.
    /// \param yyo    The output stream.
    /// \param yysym  The symbol.
    template <typename Base>
    void yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const;
#endif

    /// \brief Reclaim the memory associated to a symbol.
    /// \param yymsg     Why this token is reclaimed.
    ///                  If null, print nothing.
    /// \param yysym     The symbol.
    template <typename Base>
    void yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const;

  private:
    /// Type access provider for state based symbols.
    struct by_state
    {
      /// Default constructor.
      by_state () YY_NOEXCEPT;

      /// The symbol kind as needed by the constructor.
      typedef state_type kind_type;

      /// Constructor.
      by_state (kind_type s) YY_NOEXCEPT;

      /// Copy constructor.
      by_state (const by_state& that) YY_NOEXCEPT;

      /// Record that this symbol is empty.
      void clear () YY_NOEXCEPT;

      /// Steal the symbol kind from \a that.
      void move (by_state& that);

      /// The symbol kind (corresponding to \a state).
      /// \a symbol_kind::S_YYEMPTY when empty.
      symbol_kind_type kind () const YY_NOEXCEPT;

      /// The state number used to denote an empty symbol.
      /// We use the initial state, as it does not have a value.
      enum { empty_state = 0 };

      /// The state.
      /// \a empty when empty.
      state_type state;
    };

    /// "Internal" symbol: element of the stack.
    struct stack_symbol_type : basic_symbol<by_state>
    {
      /// Superclass.
      typedef basic_symbol<by_state> super_type;
      /// Construct an empty symbol.
      stack_symbol_type ();
      /// Move or copy construction.
      stack_symbol_type (YY_RVREF (stack_symbol_type) that);
      /// Steal the contents from \a sym to build this.
      stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) sym);
#if YY_CPLUSPLUS < 201103L
      /// Assignment, needed by push_back by some old implementations.
      /// Moves the contents of that.
      stack_symbol_type& operator= (stack_symbol_type& that);

      /// Assignment, needed by push_back by other implementations.
      /// Needed by some other old implementations.
      stack_symbol_type& operator= (const stack_symbol_type& that);
#endif
    };

    /// A stack with random access from its top.
    template <typename T, typename S = std::vector<T> >
    class stack
    {
    public:
      // Hide our reversed order.
      typedef typename S::iterator iterator;
      typedef typename S::const_iterator const_iterator;
      typedef typename S::size_type size_type;
      typedef typename std::ptrdiff_t index_type;

      stack (size_type n = 200) YY_NOEXCEPT
        : seq_ (n)
      {}

#if 201103L <= YY_CPLUSPLUS
      /// Non copyable.
      stack (const stack&) = delete;
      /// Non copyable.
      stack& operator= (const stack&) = delete;
#endif

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      const T&
      operator[] (index_type i) const
      {
        return seq_[size_type (size () - 1 - i)];
      }

      /// Random access.
      ///
      /// Index 0 returns the topmost element.
      T&
      operator[] (index_type i)
      {
        return seq_[size_type (size () - 1 - i)];
      }

      /// Steal the contents of \a t.
      ///
      /// Close to move-semantics.
      void
      push (YY_MOVE_REF (T) t)
      {
        seq_.push_back (T ());
        operator[] (0).move (t);
      }

      /// Pop elements from the stack.
      void
      pop (std::ptrdiff_t n = 1) YY_NOEXCEPT
      {
        for (; 0 < n; --n)
          seq_.pop_back ();
      }

      /// Pop all elements from the stack.
      void
      clear () YY_NOEXCEPT
      {
        seq_.clear ();
      }

      /// Number of elements on the stack.
      index_type
      size () const YY_NOEXCEPT
      {
        return index_type (seq_.size ());
      }

      /// Iterator on top of the stack (going downwards).
      const_iterator
      begin () const YY_NOEXCEPT
      {
        return seq_.begin ();
      }

      /// Bottom of the stack.
      const_iterator
      end () const YY_NOEXCEPT
      {
        return seq_.end ();
      }

      /// Present a slice of the top of a stack.
      class slice
      {
      public:
        slice (const stack& stack, index_type range) YY_NOEXCEPT
          : stack_ (stack)
          , range_ (range)
        {}

        const T&
        operator[] (index_type i) const
        {
          return stack_[range_ - i];
        }

      private:
        const stack& stack_;
        index_type range_;
      };

    private:
#if YY_CPLUSPLUS < 201103L
      /// Non copyable.
      stack (const stack&);
      /// Non copyable.
      stack& operator= (const stack&);
#endif
      /// The wrapped container.
      S seq_;
    };


    /// Stack type.
    typedef stack<stack_symbol_type> stack_type;

    /// The stack.
    stack_type yystack_;

    /// Push a new state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param sym  the symbol
    /// \warning the contents of \a s.value is stolen.
    void yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym);

    /// Push a new look ahead token on the state on the stack.
    /// \param m    a debug message to display
    ///             if null, no trace is output.
    /// \param s    the state
    /// \param sym  the symbol (for its value and location).
    /// \warning the contents of \a sym.value is stolen.
    void yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym);

    /// Pop \a n symbols from the stack.
    void yypop_ (int n = 1) YY_NOEXCEPT;

    /// Constants.
    enum
    {
      yylast_ = 2039,     ///< Last index in yytable_.
      yynnts_ = 21,  ///< Number of nonterminal symbols.
      yyfinal_ = 44 ///< Termination state number.
    };



  };


#line 6 "src/syntax.ast.y"
} // parser
#line 1946 "src/generated/ast_parser.hpp"


// "%code provides" blocks.
#line 10 "src/syntax.ast.y"
 int yylex(parser::Parser::semantic_type*, parser::Parser::location_type*); 

#line 1953 "src/generated/ast_parser.hpp"


#endif // !YY_YY_SRC_GENERATED_AST_PARSER_HPP_INCLUDED
