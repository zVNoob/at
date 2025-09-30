#include "lexer.hpp"

#include "error.hpp"
#include "object.hpp"
#include "parser.hpp"

#include "bigint.hpp"
#include "bigfraction.hpp"

#include "integer.hpp"
#include "fraction.hpp"
#include "string.hpp"
#include "type.hpp"
#include "unknown_variable.hpp"
#include "variable.hpp"

#include <memory>
#include <tuple>
#include <utility>

namespace lexer {

std::pair<char, parser::position> Lexer::get_char() {
  char c = next();
  buffer.back().push_back(c);
  int tabstop = 8;
  if (error_reporter) tabstop = error_reporter->tabstop;
  if (c == '\t') {
    current_col = (current_col / tabstop + 1) * tabstop;
  }
  else {
    current_col++;
  }
  if (c == '\n') {
    buffer.emplace_back("");
    current_col = 1;
  }
  
  return std::make_pair(c, parser::position(&filename, buffer.size(), current_col));
}

const std::string keyword = "!@#$%^&*()-=+[]{};:,<>/?~`";

void Lexer::process_string(parser::Parser::value_type* yylval,parser::Parser::location_type* yylloc) {
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
  yylval->emplace<std::shared_ptr<object::Object>>(std::make_shared<string::String>(s));
  yylloc->end = next_char.second;
}


int Lexer::process_multichar_token(char current_char,
                                   parser::Parser::value_type* yylval,
                                   parser::Parser::location_type* yylloc) {
  auto s = std::string(1, current_char);
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
  std::shared_ptr<object::Object> result;
  if (token_type == parser::Parser::token::INTEGER) result = std::make_shared<integer::Integer>(BigInt(s));
  if (token_type == parser::Parser::token::FRACTION) result = std::make_shared<fraction::Fraction>(BigFraction(s));
  if (token_type == parser::Parser::token::IDENTIFIER) std::tie(token_type, result) = process_identifier(s);
  yylval->emplace<std::shared_ptr<object::Object>>(result);
  return token_type;
}

std::pair<int, std::shared_ptr<object::Object>> Lexer::process_identifier(std::string s) {
  auto [obj,found] = current_scope->get_variable(s);
  auto var_ptr = std::make_shared<variable::Variable>(obj);
  if (found) {
    if (dynamic_cast<type::Type*>(var_ptr.get()->get_value().get()) != nullptr) 
      return {parser::Parser::token::TYPE, var_ptr->get_value()};
    if (current_scope->auto_resolve)
      return {parser::Parser::token::VARIABLE, std::move(var_ptr)};
    return {parser::Parser::token::UNRESOLVED_VARIABLE, 
      std::make_shared<ast::UnknownVariable>(var_ptr)};
  }
  else return {parser::Parser::token::IDENTIFIER, std::move(var_ptr)};
}

int lookup(std::string s) {
  if (s == "==") return parser::Parser::token::EQUAL;
  return 0;
}

int Lexer::process_keyword(char current_char, parser::Parser::location_type* yylloc) {
  pending_char = get_char();
  if (keyword.find(pending_char.first) == std::string::npos) {
    return current_char;
  }
  std::string s = std::string(1, current_char) + std::string(1, pending_char.first);
  auto temp = lookup(s);
  if (temp) {
    yylloc->end = pending_char.second;
    pending_char.first = 0;
    return temp;
  }
  return current_char;
}

void Lexer::skip_comment(std::pair<char, parser::position>& p) {
  pending_char = get_char();
  if (pending_char.first == '*') {
    pending_char = get_char();
    while (pending_char.first != '*' || get_char().first != '\\') {
      pending_char = get_char();
    }
  }
  else while (pending_char.first != '\n') {
    pending_char = get_char();
  }
  p = get_char();
}
  

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
  if (p.first == '\\') skip_comment(p);
  while (p.first == 0 || p.first == '\n' || p.first == '\t' || p.first == ' ') {
    yylloc->begin = p.second;
    p = get_char();
  }
  yylloc->begin.filename = p.second.filename;
  yylloc->end = p.second;
  char current_char = p.first;
  if (current_char == -1) return parser::Parser::token::YYEOF;
  if (keyword.find(current_char) != std::string::npos)  return process_keyword(current_char, yylloc);
  
  if (current_char == '\"') {
    process_string(yylval,yylloc);
    return parser::Parser::token::STRING;
  }

  return process_multichar_token(current_char, yylval, yylloc);
}

int Lexer::scope_lex(parser::Parser::value_type* yylval,parser::Parser::location_type* yylloc) {
  return pure_lex(yylval,yylloc);
}

int Lexer::lex(parser::Parser::value_type* yylval,parser::Parser::location_type* yylloc) {
  return scope_lex(yylval,yylloc);
}
char StreamLexer::next() {
  char c = 0;
  if (input.eof()) return -1;
  input.get(c);
  return c;
}


}
