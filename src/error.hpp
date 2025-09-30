#pragma once

#include "lexer.hpp"
#include "location.hh"
#include "object.hpp"
#include "parser.hpp"
#include <source_location>
#include <string>
#include <format>

namespace error {
class ErrorReporter {
protected:
  virtual void on_orphan_value(std::shared_ptr<object::Object> value) {
    // do nothing
  }
public:
  int tabstop = 4;
  bool is_assigning = false;
  virtual void report(lexer::Lexer* lexer,parser::location loc,const std::string& msg) = 0;
  void orphan_value(std::shared_ptr<object::Object> value) {
    if (is_assigning) {
      is_assigning = false;
      return;
    }
    on_orphan_value(value);
  }
};
class StreamErrorReporter : public ErrorReporter {
  void on_orphan_value(std::shared_ptr<object::Object> value) override;
public:
  std::ostream &output;
  explicit StreamErrorReporter(std::ostream &output) : output(output) {}
  void report(lexer::Lexer* lexer,parser::location loc,const std::string& msg) override;
  void dump_object(object::Object* obj);
};

class eval_error : public parser::Parser::syntax_error {
public:
  explicit eval_error(const std::string& msg,parser::location loc = parser::location()) : parser::Parser::syntax_error(loc,msg) {}
};

class internal_error : public parser::Parser::syntax_error {
  std::string floc;
public:
  explicit internal_error(const std::string& msg,std::source_location loc = std::source_location::current()) : parser::Parser::syntax_error(parser::location(&floc,loc.line(),loc.column()),
                                std::format("{}: {}",loc.function_name(), msg)), floc(loc.file_name()) {}
};



}
