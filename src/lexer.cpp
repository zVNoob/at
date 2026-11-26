#include "lexer.h"

#include "generated/ast_parser.hpp"

#include <algorithm>
#include <cctype>
#include <map>
#include <string>
#include <utility>

namespace at {

namespace {
using parser::Parser;

int tokenForOperator(const std::string& op) {
    static const std::map<std::string, int> table = {
        {"::", Parser::token::SCOPE},
        {":=", Parser::token::ALIAS_PUBLIC},
        {"!:=", Parser::token::ALIAS_PRIVATE},
        {"<|", Parser::token::TEMPLATE_OPEN},
        {"|>", Parser::token::TEMPLATE_CLOSE},
        {"||", Parser::token::OROR},
        {"&&", Parser::token::ANDAND},
        {"^^", Parser::token::XORXOR},
        {"<=", Parser::token::LE},
        {"==", Parser::token::EQEQ},
        {">=", Parser::token::GE},
        {"!=", Parser::token::NE},
        {"<{", Parser::token::CONSTRUCT_OPEN},
        {"}>", Parser::token::CONSTRUCT_CLOSE},
        {"->", Parser::token::ARROW},
        {"@?", Parser::token::FUNPTR},
        {"@<", Parser::token::RETURN},
        {"$^", Parser::token::CONTINUE},
        {"$!", Parser::token::BREAK},
        {"@$", Parser::token::SELF_FUNC},
        {"#$", Parser::token::SELF_CLASS},
        {"^:", Parser::token::PROT},
        {"~:", Parser::token::PRIV},
        {"^=", Parser::token::OVERLOAD},
        {"!:", Parser::token::CONST_PUB},
        {"!^:", Parser::token::CONST_PROT},
        {"!~:", Parser::token::CONST_PRIV},
    };
    auto it = table.find(op);
    if (it != table.end()) return it->second;
    return 0;
}
} // namespace

Lexer::Lexer(std::string source, std::string filename)
    : source_(std::move(source)), filename_(std::move(filename)) {}

Token Lexer::next() {
    skipWhitespace();
    if (pos_ >= source_.size()) {
        return makeToken(0, line_, column_, "");
    }

    const std::size_t startLine = line_;
    const std::size_t startColumn = column_;

    const unsigned char c = static_cast<unsigned char>(source_[pos_]);

    if (c == '"') return parseString(startLine, startColumn, '"', Parser::token::STRING);
    if (c == '\'') return parseString(startLine, startColumn, '\'', Parser::token::IMPORT);

    if (std::isdigit(c) || (c == '.' && pos_ + 1 < source_.size() &&
                             std::isdigit(static_cast<unsigned char>(source_[pos_ + 1])))) {
        return parseNumber(startLine, startColumn);
    }

    if (isIdentStart(c)) {
        return parseIdentifier(startLine, startColumn);
    }

    return parseOperator(startLine, startColumn);
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (true) {
        Token t = next();
        tokens.push_back(t);
        if (t.kind == 0) break;
    }
    return tokens;
}

void Lexer::skipWhitespace() {
    while (pos_ < source_.size()) {
        while (pos_ < source_.size() && isSpace(static_cast<unsigned char>(source_[pos_]))) {
            advanceOne();
        }
        if (pos_ + 1 < source_.size() && source_[pos_] == '/') {
            if (source_[pos_ + 1] == '/') {
                while (pos_ < source_.size() && source_[pos_] != '\n') {
                    advanceOne();
                }
                continue;
            } else if (source_[pos_ + 1] == '*') {
                pos_ += 2;
                while (pos_ + 1 < source_.size() && !(source_[pos_] == '*' && source_[pos_ + 1] == '/')) {
                    advanceOne();
                }
                if (pos_ + 1 < source_.size()) {
                    pos_ += 2;
                }
                continue;
            }
        }
        break;
    }
}

Token Lexer::makeToken(int kind, std::size_t startLine, std::size_t startColumn, std::string lexeme) {
    Token t;
    t.kind = kind;
    t.lexeme = std::move(lexeme);
    t.location.line = startLine;
    t.location.column = startColumn;
    return t;
}

char Lexer::peek(std::size_t offset) const {
    if (pos_ + offset >= source_.size()) return '\0';
    return source_[pos_ + offset];
}

bool Lexer::isSpace(unsigned char c) const {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' ||
           c == '\f' || c == '\v';
}

bool Lexer::isIdentStart(unsigned char c) const {
    return std::isalpha(c) || c == '_' || c >= 0x80;
}

bool Lexer::isIdentContinue(unsigned char c) const {
    return std::isalnum(c) || c == '_' || c >= 0x80;
}

void Lexer::advanceOne() {
    if (pos_ >= source_.size()) return;
    if (source_[pos_] == '\n') {
        ++line_;
        column_ = 1;
    } else {
        ++column_;
    }
    ++pos_;
}

void Lexer::advanceBy(std::size_t count) {
    for (std::size_t i = 0; i < count; ++i) advanceOne();
}

Token Lexer::parseIdentifier(std::size_t startLine, std::size_t startColumn) {
    std::string lexeme;

    if (pos_ >= source_.size() || !isIdentStart(static_cast<unsigned char>(source_[pos_]))) {
        std::string one(1, pos_ < source_.size() ? source_[pos_] : '\0');
        advanceOne();
        return makeToken(-1, startLine, startColumn, one);
    }

    lexeme.push_back(source_[pos_]);
    advanceOne();

    while (pos_ < source_.size()) {
        unsigned char c = static_cast<unsigned char>(source_[pos_]);
        if (isIdentContinue(c)) {
            lexeme.push_back(source_[pos_]);
            advanceOne();
        } else if (isSpace(c)) {
            std::size_t j = pos_;
            while (j < source_.size() && isSpace(static_cast<unsigned char>(source_[j]))) ++j;
            if (j < source_.size() && isIdentContinue(static_cast<unsigned char>(source_[j]))) {
                while (pos_ < j) {
                    lexeme.push_back(source_[pos_]);
                    advanceOne();
                }
            } else {
                break;
            }
        } else {
            break;
        }
    }

    return makeToken(Parser::token::IDENTIFIER, startLine, startColumn, lexeme);
}

Token Lexer::parseNumber(std::size_t startLine, std::size_t startColumn) {
    std::string lexeme;
    std::size_t start = pos_;

    auto consumeDigits = [&](bool hex) {
        while (pos_ < source_.size()) {
            unsigned char c = static_cast<unsigned char>(source_[pos_]);
            if (hex ? (std::isxdigit(c) || c == '_') : (std::isdigit(c) || c == '_')) {
                advanceOne();
            } else {
                break;
            }
        }
    };

    bool isDecimal = false;

    // Prefixes
    if (pos_ + 1 < source_.size() && source_[pos_] == '0') {
        char p = source_[pos_ + 1];
        if (p == 'x' || p == 'X') {
            advanceBy(2);
            consumeDigits(true);
        } else if (p == 'o' || p == 'O') {
            advanceBy(2);
            consumeDigits(false); // octal digits are subset; underscores ok
        } else if (p == 'b' || p == 'B') {
            advanceBy(2);
            while (pos_ < source_.size()) {
                unsigned char c = static_cast<unsigned char>(source_[pos_]);
                if (c == '0' || c == '1' || c == '_') advanceOne();
                else break;
            }
        } else {
            consumeDigits(false);
        }
    } else if (source_[pos_] == '.') {
        // handled below
    } else {
        consumeDigits(false);
    }

    // Fractional part
    if (pos_ < source_.size() && source_[pos_] == '.') {
        std::size_t nextPos = pos_ + 1;
        if (nextPos < source_.size() && std::isdigit(static_cast<unsigned char>(source_[nextPos]))) {
            isDecimal = true;
            advanceOne();
            consumeDigits(false);
        }
    }

    // Exponent
    if (pos_ < source_.size() && (source_[pos_] == 'e' || source_[pos_] == 'E')) {
        std::size_t expPos = pos_ + 1;
        if (expPos < source_.size() && (source_[expPos] == '+' || source_[expPos] == '-')) ++expPos;
        if (expPos < source_.size() && std::isdigit(static_cast<unsigned char>(source_[expPos]))) {
            isDecimal = true;
            advanceOne();
            if (pos_ < source_.size() && (source_[pos_] == '+' || source_[pos_] == '-')) advanceOne();
            consumeDigits(false);
        }
    }

    lexeme = source_.substr(start, pos_ - start);
    if (lexeme.empty()) lexeme = std::string(1, source_[start]);

    int kind = isDecimal ? Parser::token::DECIMAL : Parser::token::INTEGER;
    return makeToken(kind, startLine, startColumn, lexeme);
}

Token Lexer::parseString(std::size_t startLine, std::size_t startColumn, char delimiter, int kind) {
    std::string lexeme;
    lexeme.push_back(source_[pos_]);
    advanceOne();

    while (pos_ < source_.size()) {
        char c = source_[pos_];
        if (c == '\\') {
            lexeme.push_back(c);
            advanceOne();
            if (pos_ < source_.size()) {
                lexeme.push_back(source_[pos_]);
                advanceOne();
            }
            continue;
        }
        if (c == delimiter) {
            lexeme.push_back(c);
            advanceOne();
            return makeToken(kind, startLine, startColumn, lexeme);
        }
        lexeme.push_back(c);
        advanceOne();
    }

    return makeToken(-1, startLine, startColumn, lexeme);
}

Token Lexer::parseOperator(std::size_t startLine, std::size_t startColumn) {
    // Try longest match first.
    for (std::size_t len = 4; len >= 1; --len) {
        if (pos_ + len > source_.size()) continue;
        std::string candidate = source_.substr(pos_, len);
        // Only consider if it starts with a non-identifier char to avoid eating id chars.
        int kind = tokenForOperator(candidate);
        if (kind != 0) {
            // Ensure we don't accidentally match a prefix of an identifier.
            advanceBy(len);
            return makeToken(kind, startLine, startColumn, candidate);
        }
        // Single-character operators: if candidate length 1 and not alnum/underscore/utf8, return its code.
        if (len == 1) {
            unsigned char c = static_cast<unsigned char>(candidate[0]);
            if (!isIdentStart(c) && !isSpace(c) && !std::isdigit(c)) {
                advanceOne();
                return makeToken(static_cast<int>(c), startLine, startColumn, candidate);
            }
        }
    }

    // Fallback: consume one byte as error.
    std::string one(1, source_[pos_]);
    advanceOne();
    return makeToken(-1, startLine, startColumn, one);
}

std::string tokenKindName(int kind) {
    switch (kind) {
        case 0: return "EOF";
        case Parser::token::INTEGER: return "INTEGER";
        case Parser::token::DECIMAL: return "DECIMAL";
        case Parser::token::STRING: return "STRING";
        case Parser::token::IMPORT: return "IMPORT";
        case Parser::token::IDENTIFIER: return "IDENTIFIER";
        case Parser::token::SCOPE: return "SCOPE";
        case Parser::token::TEMPLATE_OPEN: return "TEMPLATE_OPEN";
        case Parser::token::TEMPLATE_CLOSE: return "TEMPLATE_CLOSE";
        case Parser::token::OROR: return "OROR";
        case Parser::token::ANDAND: return "ANDAND";
        case Parser::token::XORXOR: return "XORXOR";
        case Parser::token::LE: return "LE";
        case Parser::token::EQEQ: return "EQEQ";
        case Parser::token::GE: return "GE";
        case Parser::token::NE: return "NE";
        case Parser::token::CONSTRUCT_OPEN: return "CONSTRUCT_OPEN";
        case Parser::token::CONSTRUCT_CLOSE: return "CONSTRUCT_CLOSE";
        case Parser::token::ARROW: return "ARROW";
        case Parser::token::OVERLOAD: return "OVERLOAD";
        case Parser::token::FUNPTR: return "FUNPTR";
        case Parser::token::RETURN: return "RETURN";
        case Parser::token::CONTINUE: return "CONTINUE";
        case Parser::token::BREAK: return "BREAK";
        case Parser::token::SELF_FUNC: return "SELF_FUNC";
        case Parser::token::SELF_CLASS: return "SELF_CLASS";
        case Parser::token::PROT: return "PROT";
        case Parser::token::PRIV: return "PRIV";
        case Parser::token::CONST_PUB: return "CONST_PUB";
        case Parser::token::CONST_PROT: return "CONST_PROT";
        case Parser::token::CONST_PRIV: return "CONST_PRIV";
        case Parser::token::ALIAS_PUBLIC: return "ALIAS_PUBLIC";
        case Parser::token::ALIAS_PRIVATE: return "ALIAS_PRIVATE";
        default: return "CHAR(" + std::to_string(kind) + ")";
    }
}

} // namespace at
