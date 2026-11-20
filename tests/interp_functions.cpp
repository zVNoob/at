// Regression tests for multi-element lists (flattened by parseSource),
// return-type-annotated function literals, and @? declared signatures.
#include "interpreter.h"
#include "parser.h"
#include "semantic.h"

#include <cassert>
#include <functional>
#include <iostream>
#include <string>

namespace {

struct Result {
    size_t semErrors;
    at::Value value;
};

Result run(const std::string& src) {
    auto ast = at::parseSource(src);
    assert(ast != nullptr);
    at::SemanticAnalyzer sem;
    sem.analyze(ast);
    Result r{sem.errors().size(), {}};
    if (r.semErrors == 0) {
        at::Interpreter interp;
        r.value = interp.run(ast);
    }
    return r;
}

bool isInt(const at::Value& v, long long n) {
    return v.kind == at::Value::Kind::Int && v.intValue.toInt64() == n;
}

} // namespace

int main() {
    // Lists are flat: every element is a direct child.
    {
        auto ast = at::parseSource("f(1, 2, 3);\n");
        assert(ast);
        std::function<at::AstPtr(const at::AstPtr&)> find = [&](const at::AstPtr& n) -> at::AstPtr {
            if (!n) return nullptr;
            if (n->kind == "expr_list") return n;
            for (const auto& c : n->children) if (auto r = find(c)) return r;
            return nullptr;
        };
        auto list = find(ast);
        assert(list);
        size_t exprs = 0;
        for (const auto& c : list->children) {
            assert(c->kind != "expr_list");
            if (c->kind == "expr") ++exprs;
        }
        assert(exprs == 3);
    }

    // Two-parameter function, both params bound in order.
    assert(isInt(run("f = @ (a, b) { @< a - b; };\nf(10, 3);\n").value, 7));
    // Function literal with a return-type annotation.
    assert(isInt(run("f = @ () -> (`0`) { @< 1; };\nf();\n").value, 1));
    // Declared @? signature with params and return type.
    const std::string sub =
        "sub : @? (a : `0`, b : `0`) -> (`0`) = @ (a : `0`, b : `0`) -> (`0`) { @< a - b; };\n";
    {
        auto r = run(sub + "y : `0` = sub(10, 3);\ny;\n");
        assert(r.semErrors == 0);
        assert(isInt(r.value, 7));
    }
    // Arity and argument types are checked against the declared signature.
    assert(run(sub + "sub(1);\n").semErrors == 1);
    assert(run(sub + "sub(1, \"x\");\n").semErrors == 1);
    // Return type flows to the call site.
    assert(run(sub + "s : `\"\"` = sub(1, 2);\n").semErrors == 1);
    // Typeof annotations are honoured.
    assert(run("x : `\"\"` = 1;\n").semErrors == 1);
    assert(run("x : `0` = 1;\n").semErrors == 0);

    std::cout << "interp_functions: PASS\n";
    return 0;
}
