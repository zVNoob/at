#include <iostream>

#include "lexer.hpp"
#include "parser.hpp"
#include "error.hpp"

using namespace std;

int main() {
  error::StreamErrorReporter error_reporter(std::cerr);
  lexer::StreamLexer lexer(std::cin, &error_reporter);
  parser::Parser parser(&lexer, &error_reporter);
  //parser.set_debug_level(1);
  parser();
  return 0;
}
