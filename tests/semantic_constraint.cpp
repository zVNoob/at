#include "parser.h"
#include "semantic.h"

#include <cassert>
#include <iostream>
#include <string>

int main() {
    std::string input = "Foo := <| T |> @ (a : T) { @< a + 1; };\nFoo(\"a\");\n";
    at::AstPtr ast = at::parseSource(input);
    assert(ast != nullptr);
    at::SemanticAnalyzer sem;
    sem.analyze(ast);
    bool found = false;
    for (const auto& e : sem.errors()) {
        if (e.message.find("constraint not satisfied") != std::string::npos) found = true;
    }
    assert(found);
    std::cout << "semantic_constraint: PASS\n";
    return 0;
}