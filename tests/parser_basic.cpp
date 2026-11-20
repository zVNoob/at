#include "parser.h"

#include <cassert>
#include <iostream>
#include <string>

int main() {
    std::string input = "x : int = 42;\ny : int = x + 1;\n";
    at::AstPtr ast = at::parseSource(input);
    assert(ast != nullptr);
    std::cout << "parser_basic: PASS\n";
    return 0;
}