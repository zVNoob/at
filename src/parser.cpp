#include "ast.h"
#include "lexer.h"

#include "generated/ast_parser.hpp"

#include <iostream>
#include <string>

namespace at {
AstPtr g_parse_result;
}

namespace {
at::Lexer* g_lexer = nullptr;
}

int yylex(parser::Parser::semantic_type* yylval,
          parser::Parser::location_type* yylloc) {
    if (!g_lexer) return 0;

    at::Token tok = g_lexer->next();

    if (yylloc) {
        yylloc->begin.line = static_cast<int>(tok.location.line);
        yylloc->begin.column = static_cast<int>(tok.location.column);
        yylloc->end = yylloc->begin;
    }

    if (tok.kind == 0) return 0;

    if (yylval) {
        yylval->emplace<at::AstPtr>(
            at::makeLeaf(at::tokenKindName(tok.kind), tok.lexeme, {tok.location.line, tok.location.column}));
    }
    return tok.kind;
}

void parser::Parser::error(const location_type& loc, const std::string& msg) {
    std::cerr << "parse error at " << loc.begin.line << ":" << loc.begin.column
              << ": " << msg << "\n";
}

namespace at {

namespace {

// The grammar's list rules are left-recursive (`x_list: x_list ',' x`), which
// produces a chain of nested *_list nodes. Every consumer wants a flat list,
// so splice nested same-kind lists into their parent once, after parsing.
// `scope` is intentionally left alone; consumers already recurse into it.
bool isFlattenedList(const std::string& kind) {
    return kind.size() > 5 && kind.compare(kind.size() - 5, 5, "_list") == 0;
}

void flattenLists(const AstPtr& node) {
    if (!node) return;
    for (const auto& c : node->children) flattenLists(c);
    if (!isFlattenedList(node->kind)) return;
    std::vector<AstPtr> flat;
    for (const auto& c : node->children) {
        if (c && c->kind == node->kind) {
            flat.insert(flat.end(), c->children.begin(), c->children.end());
        } else {
            flat.push_back(c);
        }
    }
    node->children = std::move(flat);
}

} // namespace

AstPtr parseSource(const std::string& source) {
    g_parse_result.reset();
    Lexer lexer(source, "<input>");
    g_lexer = &lexer;
    parser::Parser parser;
    parser.parse();
    g_lexer = nullptr;
    flattenLists(g_parse_result);
    return g_parse_result;
}

} // namespace at
