#pragma once

#include <memory>
#include <string>
#include <vector>

namespace at {

struct SourceSpan {
    std::size_t line = 0;
    std::size_t column = 0;
};

struct AstNode;
using AstPtr = std::shared_ptr<AstNode>;

struct AstNode {
    std::string kind;          // production / node kind
    std::string text;          // lexeme or operator text
    SourceSpan span;
    std::vector<AstPtr> children;

    AstNode() = default;
    AstNode(std::string k, std::string t = "", SourceSpan s = {})
        : kind(std::move(k)), text(std::move(t)), span(s) {}
};

inline AstPtr makeNode(const std::string& kind, const std::string& text = "", SourceSpan span = {}) {
    return std::make_shared<AstNode>(kind, text, span);
}

inline AstPtr makeLeaf(const std::string& kind, const std::string& text, SourceSpan span) {
    return makeNode(kind, text, span);
}

} // namespace at
