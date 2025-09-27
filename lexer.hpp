#pragma once

#include "location.hh"
#include "parser.hpp"
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "error.hpp"
#include "scope.hpp"


namespace error {class ErrorReporter;}

namespace lexer {

  class Lexer {
  std::string filename;
  unsigned int current_col = 1;
  error::ErrorReporter* error_reporter = nullptr;
  std::pair<char, parser::position> pending_char;
public:
    std::vector<std::string> buffer = {""};
    std::shared_ptr<scope::Scope> current_scope = std::make_shared<scope::Scope>();
    Lexer(const std::string& filename, error::ErrorReporter* error_reporter) : 
    filename(filename), error_reporter(error_reporter) {};
    int lex(parser::Parser::value_type* yylval,parser::Parser::location_type* yylloc);
  protected:
    virtual char next() = 0;
  private:
    std::pair<char, parser::position> get_char();
    int scope_lex(parser::Parser::value_type* yylval, parser::Parser::location_type* yylloc);
    int pure_lex(parser::Parser::value_type* yylval, parser::Parser::location_type* yylloc);
    void process_string(parser::Parser::value_type* yylval, parser::Parser::location_type* yylloc);
    int process_multichar_token(char current_char, parser::Parser::value_type* yylval, parser::Parser::location_type* yylloc);
    int process_keyword(char current_char, parser::Parser::location_type* yylloc);
    std::pair<int,std::shared_ptr<object::Object>> process_identifier(std::string s);
  };

  class StreamLexer : public Lexer {
  std::istream &input;
  public:
    StreamLexer(std::istream &input, error::ErrorReporter* error_reporter) : Lexer("stream", error_reporter), input(input) {}
    char next() override;
  };

}
