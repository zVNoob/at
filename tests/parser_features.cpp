#include "parser.h"

#include <cassert>
#include <iostream>
#include <string>
#include <vector>

int main() {
    std::vector<std::string> samples = {
        // class declaration via alias
        "Foo := # (x : `0`) { x; };\n",
        "Foo := # (x : `0`) : `0` { x; };\n",
        // function declaration
        "f : @? (a : `0`) = @ (a : `0`) { @< a + 1; };\n",
        // unwrap
        "* (x) { x; };\n",
        "* (x) { x; } : { 0; };\n",
        // try cast
        "? (x -> `0`) { x; };\n",
        "? (x -> `0`) { x; } : { 0; };\n",
        // cast only valid in try-cast statements
        "? (x -> `0`) { x; };\n",
        // array and construction
        "arr = [1, 2, 3];\n",
        "p = Point <{ 1, 2 }>;\n",
        // control flow
        "(x <= 10) ? { 0; } : { 1; };\n",
        "$ (x) { x; };\n",
        "$ (i : 0) { i; };\n",
        // template
        "Foo := <| T |> # () { };\n",
        // overloading
        "+ ^= f;\n",
        "g ^= h;\n",
        // borrowing
        "r = &x;\n",
        // pointer declaration
        "ptr : * `0` = 0;\n",
    };

    for (const auto& sample : samples) {
        at::AstPtr ast = at::parseSource(sample);
        if (!ast) {
            std::cerr << "FAILED to parse: " << sample;
            return 1;
        }
    }
    std::cout << "parser_features: PASS (" << samples.size() << " samples)\n";
    return 0;
}