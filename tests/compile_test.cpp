#include "codegen.h"
#include "parser.h"
#include "semantic.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

int main() {
    std::string input = "x : `0` = 41;\ny : `0` = x + 1;\ny;\n";
    auto ast = at::parseSource(input);
    assert(ast != nullptr);

    at::SemanticAnalyzer sem;
    sem.analyze(ast);
    if (!sem.errors().empty()) {
        std::cout << "sem errors: " << sem.errors().size() << "\n";
        return 1;
    }

    at::CodeGen cg;
    std::string out = "/tmp/opencode/compile_test.o";
    assert(cg.compile(ast, out));

    // Verify it's a valid ELF object
    std::ifstream f(out, std::ios::binary);
    assert(f.is_open());
    char magic[4];
    f.read(magic, 4);
    assert(magic[0] == 0x7f && magic[1] == 'E' && magic[2] == 'L' && magic[3] == 'F');

    std::cout << "compile_test: PASS\n";
    return 0;
}