#include "lexer.hpp"

#include "error.hpp"
#include "parser.hpp"

#include "type/bigint.hpp"
#include "type/bigfraction.hpp"
#include <cstdio>

namespace lexer {

std::pair<char, parser::position> Lexer::get_char() {
  char c = next();
  buffer.back().push_back(c);
  if (c == '\t') {
    current_col = (current_col / error_reporter->tabstop + 1) * error_reporter->tabstop;
  }
  else {
    current_col++;
  }
  if (c == '\n') {
    buffer.push_back("");
    current_col = 1;
  }
  
  return std::make_pair(c, parser::position(&filename, buffer.size(), current_col));
}

const std::string keyword = "!@#$%^&*()-=+[]{};:,<>/?~`";

int Lexer::pure_lex(parser::Parser::value_type* yylval,parser::Parser::location_type* yylloc) {
  std::pair<char, parser::position> p = {0, yylloc->end};
  if (pending_char.first) {
    p = pending_char;
    yylloc->begin = yylloc->end;
    pending_char.first = 0;
  }
  while (p.first == 0 || p.first == '\n' || p.first == '\t' || p.first == ' ') {
    yylloc->begin = p.second;
    p = get_char();
  }
  yylloc->begin.filename = p.second.filename;
  yylloc->end = p.second;
  char current_char = p.first;

  if (current_char == -1) return parser::Parser::token::YYEOF;
  if (keyword.find(current_char) != std::string::npos)  return current_char;
  
  if (current_char == '\"') {
    std::string s = "";
    std::pair<char, parser::position> next_char;
    while ((next_char = get_char()).first != '\0' && next_char.first != '\"') {
      if (next_char.first == '\\') {
        if ((next_char = get_char()).first == '\0') break;
        switch (next_char.first) {
          case 'a': s += '\a'; break;
          case 'b': s += '\b'; break;
          case 'f': s += '\f'; break;
          case 'n': s += '\n'; break;
          case 'r': s += '\r'; break;
          case 't': s += '\t'; break;
          case 'v': s += '\v'; break;
          case '\\': s += '\\'; break;
          case '\"': s += '\"'; break;
          default: s += next_char.first;
        }
      }
      else s += next_char.first;
      
    }
    yylval->emplace<std::string>(s);
    yylloc->end = next_char.second;
    return parser::Parser::token::STRING;
  }

  std::string s = std::string(1, current_char);
  int token_type;
  if (current_char >= '0' && current_char <= '9') token_type = parser::Parser::token::INTEGER;
  else token_type = parser::Parser::token::IDENTIFIER;
  while ((pending_char = get_char()).first != '\0' && 
          keyword.find(pending_char.first) == std::string::npos) {
    if (pending_char.first == '.') { 
      if (token_type == parser::Parser::token::INTEGER)
        token_type = parser::Parser::token::FRACTION;
      else break;
    }
    else if (pending_char.first == ' ' || pending_char.first == '\n' || pending_char.first == '\t') {
      if (token_type != parser::Parser::token::IDENTIFIER) continue;
    }
    else if (pending_char.first < '0' || pending_char.first > '9')
      token_type = parser::Parser::token::IDENTIFIER;
    s += pending_char.first;
    if (!(pending_char.first == '\n'|| pending_char.first == '\t' || pending_char.first == ' '))
      yylloc->end = pending_char.second;
  }
  while (s.back() == ' ' || s.back() == '\n' || s.back() == '\t') s.pop_back();
  if (token_type == parser::Parser::token::INTEGER) yylval->emplace<BigInt>(s);
  if (token_type == parser::Parser::token::FRACTION) yylval->emplace<BigFraction>(s);
  if (token_type == parser::Parser::token::IDENTIFIER) yylval->emplace<std::string>(s);
  return token_type;
}

int Lexer::lex(parser::Parser::value_type* yylval,parser::Parser::location_type* yylloc) {
  return pure_lex(yylval,yylloc);
}

char StreamLexer::next() {
  char c = 0;
  if (input.eof()) return -1;
  input.get(c);
  return c;
}

}
