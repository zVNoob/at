#include "lexer.h"
#include "generated/ast_parser.hpp"

#include <cassert>
#include <iostream>
#include <string>

int main() {
    at::Lexer lexer("  my man   ", "ws");
    auto tokens = lexer.tokenize();
    assert(tokens.size() >= 2); // IDENTIFIER + EOF
    assert(tokens[0].kind == parser::Parser::token::IDENTIFIER);
    assert(tokens[0].lexeme == "my man");
    std::cout << "lexer_ws: PASS\n";
    return 0;
}