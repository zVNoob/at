#include "lexer.hpp"

#include "error.hpp"
#include "parser.hpp"

#include "bigint.hpp"
#include "bigfraction.hpp"

#include "variable.hpp"

#include <memory>

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
  yylval->emplace<std::string>(s);
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
  if (token_type == parser::Parser::token::INTEGER) yylval->emplace<BigInt>(s);
  if (token_type == parser::Parser::token::FRACTION) yylval->emplace<BigFraction>(s);
  if (token_type == parser::Parser::token::IDENTIFIER) yylval->emplace<std::string>(s);
  return token_type;
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
  yylloc->begin.filename = p.second.filename;
  yylloc->end = p.second;
  char current_char = p.first;

  if (current_char == -1) return parser::Parser::token::YYEOF;
  if (keyword.find(current_char) != std::string::npos)  return current_char;
  
  if (current_char == '\"') {
    process_string(yylval,yylloc);
    return parser::Parser::token::STRING;
  }

  return process_multichar_token(current_char, yylval, yylloc);
}

int Lexer::scope_lex(parser::Parser::value_type* yylval,parser::Parser::location_type* yylloc) {
  int token = pure_lex(yylval,yylloc);
  if (token == parser::Parser::token::IDENTIFIER) {
    auto value = scope->get_member(yylval->as<std::string>()).first;
    if (value) {
      yylval->destroy<std::string>();
      yylval->emplace<std::shared_ptr<variable::Variable>>(std::move(value));
      return parser::Parser::token::VARIABLE;
    }
  }
  return token;

}

int Lexer::lex(parser::Parser::value_type* yylval,parser::Parser::location_type* yylloc) {
  if (side == -1) if (buffered_token.size()) {
    auto [str, loc, var] = buffered_token.front();
    buffered_token.pop_front();
    if (str[0] == ',') return str[0];
    if (str[0] == ':' || str[0] == '=') {side = 1; return str[0];}
    if (is_declare) {
      yylval->emplace<std::string>(str);
      return parser::Parser::token::IDENTIFIER;
    }
    if (!var) {
      buffered_token.clear();
      throw error::eval_error("Invalid declaration, expected variable, found identifier",loc);
    }
    yylval->emplace<std::shared_ptr<variable::Variable>>(std::move(var));
    return parser::Parser::token::LHS_VARIABLE;
  }
  if (side == 1) if (buffered_token.size()) {
    auto [str, loc, var] = buffered_token.front();
    buffered_token.pop_front();
    if (str[0] == ';') {side = 0; return str[0];}
    if (keyword.find(str[0]) != std::string::npos) return str[0];
    yylval->emplace<std::shared_ptr<variable::Variable>>(std::move(var));
    return parser::Parser::token::VARIABLE;
  }
  int token = scope_lex(yylval,yylloc);
  if (token == ':' || token == '=') {
    is_declare = token == ':';
    if (side == -1) {side = 1;return token;}
    if (side == 0) {
      side = -1;
      buffered_token.emplace_back(std::string(1, token), *yylloc, std::shared_ptr<variable::Variable>());
      return lex(yylval,yylloc);
    }
  }
  if (token == ',') if (side == 0) {
    buffered_token.emplace_back(std::string(1, token), *yylloc, std::shared_ptr<variable::Variable>());
    return lex(yylval,yylloc);
  }
  if (token == parser::Parser::token::IDENTIFIER) {
    is_declare = true;
    side = -1;
    buffered_token.emplace_back(yylval->as<std::string>(), *yylloc, std::shared_ptr<variable::Variable>());
    yylval->destroy<std::string>();
    return lex(yylval,yylloc);
  }
  if (token == parser::Parser::token::VARIABLE) {
    if (side == 0) {
      buffered_token.emplace_back(yylval->as<std::shared_ptr<variable::Variable>>()->name, *yylloc, yylval->as<std::shared_ptr<variable::Variable>>());
      yylval->destroy<std::shared_ptr<variable::Variable>>();
      return lex(yylval,yylloc);
    }
    if (side == -1) {
      std::string name = yylval->as<std::shared_ptr<variable::Variable>>()->name;
      yylval->destroy<std::shared_ptr<variable::Variable>>();
      yylval->emplace<std::string>(name);
      return parser::Parser::token::IDENTIFIER;
    }
  }
  if (buffered_token.size()) {
    side = 1;
    buffered_token.emplace_back(std::string(1, token), *yylloc, std::shared_ptr<variable::Variable>());
    return lex(yylval,yylloc);
  }
  if (token == ';') {
    side = 0;
    is_declare = false;
  }
  return token;
}

char StreamLexer::next() {
  char c = 0;
  if (input.eof()) return -1;
  input.get(c);
  return c;
}


}
