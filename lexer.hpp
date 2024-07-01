#pragma once
#include "misc/bigfraction.hpp"
#include "misc/bigint.hpp"
#include "misc/stringlist.hpp"
#include "parser.tab.hh"
#include <cstddef>
#include <istream>
#include <string>

class InputLexer {
protected:
  std::istream &input;
  enum Token_Type { INT, DECIMAL, STRING, IDENTIFIER };
  typedef std::variant<BigInt, BigFraction, std::string> const_value_type;

public:
  StringList buffer;

  InputLexer(std::istream &input) : input(input) {}
  int lex(yy::parser::value_type *yylval, yy::parser::location_type *yylloc) {
    std::string result;
    std::string spacing;
    Token_Type token_t = INT;
    while (1) {
      // get input if needed
      if (buffer.size() < yylloc->end.line) {
        std::string line;
        if (!input.eof())
          std::getline(input, line);
        if (input.eof() && line.empty()) {
          // EOF
          if (result.empty())
            return yy::parser::token::YYEOF;
          else {
            buffer.add_line(line);
            if (token_t == INT) {
              yylval->emplace<const_value_type>(BigInt(result));
              return yy::parser::token::CONSTANT;
            }
            if (token_t == DECIMAL) {
              yylval->emplace<const_value_type>(BigFraction(result));
              return yy::parser::token::CONSTANT;
            }
            if (token_t == IDENTIFIER) {
              yylval->emplace<const_value_type>(std::move(result));
              return yy::parser::token::IDENTIFIER;
            }
          }
        }
        buffer.add_line(line);
      }
      if (token_t != STRING) {
        // non-string token
        for (; yylloc->end.column <= buffer[yylloc->end.line - 1].size();
             ++yylloc->end.column) {
          char token = buffer[yylloc->end.line - 1][yylloc->end.column - 1];
          if (token == ' ' || token == '\t' || token == '\r') {
            spacing += token;
            continue;
          }
          if (token >= '0' && token <= '9') {
            if (result.empty())
              yylloc->step();
            else if (!spacing.empty()) {
              token_t = IDENTIFIER;
              result += spacing;
              spacing.clear();
            }
            result += token;
            spacing.clear();
            continue;
          }
          if (token == '.') {
            if (result.empty())
              return '.';
            if (token_t == INT) {
              token_t = DECIMAL;
              result += token;
            } else {
              break;
            }
            continue;
          }
          if ((token >= 'a' && token <= 'z') ||
              (token >= 'A' && token <= 'Z')) {
            if (result.empty())
              yylloc->step();
            token_t = IDENTIFIER;
            result += spacing;
            result += token;
            spacing.clear();
            continue;
          }
          if (token == '"') {
            if (!result.empty())
              throw yy::parser::syntax_error(
                  *yylloc, "Expected identifier, unexpected \"");
            token_t = STRING;
            spacing.clear();
            yylloc->step();
            yylloc->end.column++;

            break;
          }
          if (!result.empty()) {
            if (token_t == INT) {
              yylval->emplace<const_value_type>(BigInt(result));
              return yy::parser::token::CONSTANT;
            }
            if (token_t == DECIMAL) {
              yylval->emplace<const_value_type>(BigFraction(result));
              return yy::parser::token::CONSTANT;
            }
            if (token_t == IDENTIFIER) {
              yylval->emplace<const_value_type>(std::move(result));
              return yy::parser::token::IDENTIFIER;
            }
          } else {
            yylloc->step();
            yylloc->end.column++;
            if (yylloc->end.column > buffer[yylloc->end.line - 1].size()) {
              yylloc->end.line++;
              yylloc->end.column = 1;
            }
            return token;
          }
        }
      }
      if (token_t == STRING) {
        for (; yylloc->end.column <= buffer[yylloc->end.line - 1].size();
             ++yylloc->end.column) {
          char token = buffer[yylloc->end.line - 1][yylloc->end.column - 1];
          if (token == '"') {
            yylloc->end.column++;
            yylval->emplace<const_value_type>(std::move(result));
            return yy::parser::token::CONSTANT;
          }
          if (token == '\\') {
            yylloc->end.column++;
            if (yylloc->end.column >= buffer[yylloc->end.line - 1].size())
              throw yy::parser::syntax_error(*yylloc,
                                             "Invalid escape sequence");
            token = buffer[yylloc->end.line - 1][yylloc->end.column - 1];
            switch (token) {
            case 'n':
              result += '\n';
              break;
            case 't':
              result += '\t';
              break;
            case 'r':
              result += '\r';
              break;
            case '\\':
              result += '\\';
              break;
            case '"':
              result += '"';
              break;
            default:
              throw yy::parser::syntax_error(*yylloc,
                                             "Invalid escape sequence");
            }
          }
          result += spacing + token;
        }
      }
      spacing += '\n';
      yylloc->end.line++;
      yylloc->end.column = 1;
    }
  }
};

class Lexer : public InputLexer {
public:
  Lexer(std::istream &input) : InputLexer(input) {}
};
