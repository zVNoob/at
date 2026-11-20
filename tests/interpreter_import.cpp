#include "interpreter.h"
#include "parser.h"
#include "semantic.h"

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

int main() {
    // Write a small module file
    std::ofstream mod("tests/import_mod.at");
    mod << "100;\n";
    mod.close();

    std::string input = "'tests/import_mod.at';\n";
    auto ast = at::parseSource(input);
    assert(ast != nullptr);

    at::SemanticAnalyzer sem;
    sem.analyze(ast);
    if (!sem.errors().empty()) {
        std::cout << "sem errors: " << sem.errors().size() << "\n";
        return 1;
    }

    at::Interpreter interp;
    at::Value r = interp.run(ast);
    // Module returns an object; check __result__
    assert(r.kind == at::Value::Kind::Object);
    assert(r.objectValue != nullptr);
    assert(r.objectValue->count("__result__") > 0);
    assert((*r.objectValue)["__result__"].intValue == 100);

    // Second import should hit cache
    at::Value r2 = interp.run(ast);
    assert(r2.objectValue->count("__result__") > 0);
    assert((*r2.objectValue)["__result__"].intValue == 100);

    std::cout << "interpreter_import: PASS\n";
    return 0;
}