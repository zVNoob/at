#include "error.hpp"
#include "location.hh"

void error::StreamErrorReporter::report(lexer::Lexer* lexer,parser::location loc,const std::string& msg) {
  output << loc << ": " << msg << std::endl;
}
