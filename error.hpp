#pragma once

#include "lexer.hpp"
#include "location.hh"
#include "object.hpp"
#include "parser.hpp"
#include <string>
#include <memory>

namespace error {
class ErrorReporter {
public:
  int tabstop = 4;
  virtual void report(lexer::Lexer* lexer,parser::location loc,const std::string& msg) = 0;
  virtual void orphan_return(const std::vector<std::shared_ptr<object::Object>>& obj) {
    // do nothing
  };
};
class StreamErrorReporter : public ErrorReporter {
public:
  std::ostream &output;
  explicit StreamErrorReporter(std::ostream &output) : output(output) {}
  void report(lexer::Lexer* lexer,parser::location loc,const std::string& msg) override;
  void orphan_return(const std::vector<std::shared_ptr<object::Object>>& obj) override;
};

class eval_error : public parser::Parser::syntax_error {
public:
  explicit eval_error(const std::string& msg,parser::location loc = parser::location()) : parser::Parser::syntax_error(loc,msg) {}
};

class type_mismatch : public eval_error {
public:
  explicit type_mismatch(object::Object* obj) : eval_error("type mismatch: " + obj->to_string()) {}
};

class unsupported_operator : public eval_error {
public:
  explicit unsupported_operator(std::shared_ptr<object::Object> obj, std::string op) : 
    eval_error(obj->to_string() + ": unsupported operator: " + op, obj->loc) {}
};

}
