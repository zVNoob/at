#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace at {

struct SourceLocation {
    std::size_t line = 1;
    std::size_t column = 1;
};

struct Token {
    int kind = 0;              // 0 == end of file
    std::string lexeme;
    SourceLocation location;
};

class Lexer {
public:
    explicit Lexer(std::string source, std::string filename = "");

    // Return the next token. Repeated calls eventually return kind == 0.
    Token next();

    // Tokenize the entire input, including a final EOF token.
    std::vector<Token> tokenize();

    const std::string& filename() const { return filename_; }

private:
    std::string source_;
    std::string filename_;
    std::size_t pos_ = 0;
    std::size_t line_ = 1;
    std::size_t column_ = 1;

    void skipWhitespace();
    Token makeToken(int kind, std::size_t startLine, std::size_t startColumn, std::string lexeme);

    char peek(std::size_t offset = 0) const;
    bool isSpace(unsigned char c) const;
    bool isIdentStart(unsigned char c) const;
    bool isIdentContinue(unsigned char c) const;

    void advanceOne();
    void advanceBy(std::size_t count);

    Token parseIdentifier(std::size_t startLine, std::size_t startColumn);
    Token parseNumber(std::size_t startLine, std::size_t startColumn);
    Token parseString(std::size_t startLine, std::size_t startColumn, char delimiter, int kind);
    Token parseOperator(std::size_t startLine, std::size_t startColumn);
};

std::string tokenKindName(int kind);

} // namespace at
