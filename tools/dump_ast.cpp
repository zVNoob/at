#include "parser.h"

#include <functional>
#include <iostream>
#include <sstream>
#include <string>

int main(int argc, char** argv) {
    std::string input;
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            if (i > 1) input += " ";
            input += argv[i];
        }
    } else {
        std::ostringstream oss;
        oss << std::cin.rdbuf();
        input = oss.str();
    }

    at::AstPtr ast = at::parseSource(input);
    if (!ast) {
        std::cerr << "parse failed\n";
        return 1;
    }

    std::function<void(const at::AstPtr&, int)> dump = [&](const at::AstPtr& node, int indent) {
        if (!node) return;
        for (int i = 0; i < indent; ++i) std::cout << "  ";
        std::cout << node->kind;
        if (!node->text.empty()) std::cout << " [" << node->text << "]";
        std::cout << "\n";
        for (const auto& c : node->children) dump(c, indent + 1);
    };
    dump(ast, 0);
    return 0;
}
