#include "sig_format.h"

#include <sstream>

namespace at {

// Helper: trim whitespace
static std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

// Helper: split by delimiter (respecting nested brackets)
static std::vector<std::string> splitTopLevel(const std::string& s, char delim) {
    std::vector<std::string> result;
    int depth = 0;
    std::string current;
    for (char c : s) {
        if (c == '<' || c == '(' || c == '{') { ++depth; current += c; }
        else if (c == '>' || c == ')' || c == '}') { --depth; current += c; }
        else if (c == delim && depth == 0) {
            result.push_back(trim(current));
            current.clear();
        } else {
            current += c;
        }
    }
    if (!trim(current).empty()) result.push_back(trim(current));
    return result;
}

SigType SigParser::parseType(const std::string& s) {
    std::string t = trim(s);
    SigType result;

    if (t.empty()) {
        result.kind = SigType::Kind::Unknown;
        result.name = "?";
        return result;
    }

    // Pointer: *type
    if (t[0] == '*') {
        result.kind = SigType::Kind::Pointer;
        result.params.push_back(parseType(t.substr(1)));
        result.name = "*" + result.params[0].name;
        return result;
    }

    // Reference: &type
    if (t[0] == '&') {
        result.kind = SigType::Kind::Reference;
        result.params.push_back(parseType(t.substr(1)));
        result.name = "&" + result.params[0].name;
        return result;
    }

    // Array: type[]
    if (t.size() >= 2 && t.substr(t.size() - 2) == "[]") {
        result.kind = SigType::Kind::Array;
        result.params.push_back(parseType(t.substr(0, t.size() - 2)));
        result.name = result.params[0].name + "[]";
        return result;
    }

    // Template instantiation: name<args>
    size_t lt = t.find('<');
    if (lt != std::string::npos && t.back() == '>') {
        std::string base = trim(t.substr(0, lt));
        std::string argsStr = t.substr(lt + 1, t.size() - lt - 2);
        auto args = splitTopLevel(argsStr, ',');
        SigType baseType;
        baseType.kind = SigType::Kind::Class;
        baseType.name = base;
        for (const auto& a : args) {
            baseType.params.push_back(parseType(a));
        }
        return baseType;
    }

    // Function type: fn(params) -> ret
    if (t.rfind("fn", 0) == 0 || t.rfind("function", 0) == 0) {
        result.kind = SigType::Kind::Function;
        // Find (params) and -> ret
        size_t openParen = t.find('(');
        size_t closeParen = t.find(')', openParen);
        size_t arrow = t.find("->", closeParen);
        if (openParen != std::string::npos && closeParen != std::string::npos) {
            std::string paramsStr = t.substr(openParen + 1, closeParen - openParen - 1);
            auto paramTypes = splitTopLevel(paramsStr, ',');
            for (const auto& pt : paramTypes) {
                if (!pt.empty()) result.params.push_back(parseType(pt));
            }
            if (arrow != std::string::npos) {
                result.returnType = std::make_shared<SigType>(parseType(t.substr(arrow + 2)));
            } else {
                result.returnType = std::make_shared<SigType>();
                result.returnType->name = "void";
                result.returnType->kind = SigType::Kind::Primitive;
            }
        }
        result.name = "function";
        return result;
    }

    // Primitive or named type
    result.name = t;
    if (t == "int" || t == "BigInt" || t == "i64") result.kind = SigType::Kind::Primitive;
    else if (t == "decimal" || t == "double" || t == "float") result.kind = SigType::Kind::Primitive;
    else if (t == "string" || t == "str") result.kind = SigType::Kind::Primitive;
    else if (t == "bool") result.kind = SigType::Kind::Primitive;
    else if (t == "void") result.kind = SigType::Kind::Primitive;
    else if (t == "ptr" || t == "pointer") result.kind = SigType::Kind::Primitive;
    else result.kind = SigType::Kind::Class;

    return result;
}

SigDecl SigParser::parse(const std::string& line) {
    SigDecl decl;
    std::string t = trim(line);
    if (t.empty()) return decl;

    // Determine kind
    if (t.rfind("fn ", 0) == 0 || t.rfind("function ", 0) == 0) {
        decl.kind = SigDecl::Kind::Function;
        // Remove "fn " or "function "
        size_t pos = (t.rfind("fn ", 0) == 0) ? 3 : 9;
        t = trim(t.substr(pos));
    } else if (t.rfind("class ", 0) == 0) {
        decl.kind = SigDecl::Kind::Class;
        t = trim(t.substr(6));
    } else if (t.rfind("alias ", 0) == 0) {
        decl.kind = SigDecl::Kind::Alias;
        t = trim(t.substr(6));
    } else if (t.rfind("meta ", 0) == 0) {
        decl.kind = SigDecl::Kind::Meta;
        t = trim(t.substr(5));
    } else {
        // Default to function
        decl.kind = SigDecl::Kind::Function;
    }

    if (decl.kind == SigDecl::Kind::Alias) {
        // alias name = type
        size_t eq = t.find('=');
        if (eq != std::string::npos) {
            decl.name = trim(t.substr(0, eq));
            decl.aliasTarget = parseType(trim(t.substr(eq + 1)));
        } else {
            decl.name = t;
        }
        return decl;
    }

    // Parse template params: name<T1, T2>
    size_t lt = t.find('<');
    size_t gt = t.rfind('>');
    if (lt != std::string::npos && gt != std::string::npos && gt > lt) {
        decl.name = trim(t.substr(0, lt));
        std::string tmplStr = t.substr(lt + 1, gt - lt - 1);
        auto tmplArgs = splitTopLevel(tmplStr, ',');
        for (const auto& a : tmplArgs) {
            if (!a.empty()) decl.templateParams.push_back(trim(a));
        }
        t = trim(t.substr(gt + 1));
    } else {
        // Name is first token
        size_t space = t.find(' ');
        if (space != std::string::npos) {
            decl.name = t.substr(0, space);
            t = trim(t.substr(space));
        } else {
            decl.name = t;
            t = "";
        }
    }

    // Parse bases for class: name : base1, base2 { ... }
    if (decl.kind == SigDecl::Kind::Class) {
        // Check for colon (inheritance)
        size_t colon = t.find(':');
        size_t brace = t.find('{');
        if (colon != std::string::npos && (brace == std::string::npos || colon < brace)) {
            std::string baseStr = trim(t.substr(colon + 1, (brace != std::string::npos ? brace : t.size()) - colon - 1));
            auto bases = splitTopLevel(baseStr, ',');
            for (const auto& b : bases) {
                if (!b.empty()) decl.bases.push_back(trim(b));
            }
            if (brace != std::string::npos) {
                t = t.substr(brace);
            } else {
                t = "";
            }
        }

        // Parse body: { field: type; method: fn(...); }
        if (!t.empty() && t[0] == '{') {
            size_t closeBrace = t.rfind('}');
            std::string body = (closeBrace != std::string::npos) ? t.substr(1, closeBrace - 1) : t.substr(1);
            auto members = splitTopLevel(body, ';');
            for (const auto& m : members) {
                std::string mt = trim(m);
                if (mt.empty()) continue;
                // Check if it's a field: name: type
                size_t colon = mt.find(':');
                if (colon != std::string::npos) {
                    std::string memberName = trim(mt.substr(0, colon));
                    std::string memberType = trim(mt.substr(colon + 1));
                    // Check if method: name: fn(...) or name: type (field)
                    if (memberType.rfind("fn", 0) == 0 || memberType.rfind("function", 0) == 0) {
                        // Method
                        SigDecl method;
                        method.kind = SigDecl::Kind::Function;
                        method.name = memberName;
                        // Parse as function type
                        SigType ft = parseType(memberType);
                        if (ft.returnType) method.returnType = *ft.returnType;
                        for (size_t pi = 0; pi < ft.params.size(); ++pi)
                            method.params.push_back({"_" + std::to_string(pi), ft.params[pi]});
                        decl.methods.push_back(method);
                    } else {
                        // Field
                        decl.fields.push_back({memberName, parseType(memberType)});
                    }
                }
            }
        }
        return decl;
    }

    // Parse function params: (param_types) or (name: type, ...)
    size_t openParen = t.find('(');
    size_t closeParen = t.find(')', openParen);
    if (openParen != std::string::npos && closeParen != std::string::npos) {
        std::string paramsStr = t.substr(openParen + 1, closeParen - openParen - 1);
        auto paramList = splitTopLevel(paramsStr, ',');
        for (const auto& p : paramList) {
            std::string pt = trim(p);
            if (pt.empty()) continue;
            // Check if named: name: type
            size_t colon = pt.find(':');
            if (colon != std::string::npos) {
                std::string paramName = trim(pt.substr(0, colon));
                SigType paramType = parseType(trim(pt.substr(colon + 1)));
                decl.params.push_back({paramName, paramType});
            } else {
                decl.params.push_back({"", parseType(pt)});
            }
        }
        t = trim(t.substr(closeParen + 1));
    }

    // Parse return type: -> type
    size_t arrow = t.find("->");
    if (arrow != std::string::npos) {
        decl.returnType = parseType(trim(t.substr(arrow + 2)));
    } else {
        decl.returnType.name = "void";
        decl.returnType.kind = SigType::Kind::Primitive;
    }

    return decl;
}

std::string SigParser::typeToString(const SigType& t) {
    switch (t.kind) {
        case SigType::Kind::Primitive: return t.name;
        case SigType::Kind::Pointer: return "*" + (t.params.empty() ? "?" : typeToString(t.params[0]));
        case SigType::Kind::Reference: return "&" + (t.params.empty() ? "?" : typeToString(t.params[0]));
        case SigType::Kind::Array: return (t.params.empty() ? "?" : typeToString(t.params[0])) + "[]";
        case SigType::Kind::Function: {
            std::string s = "fn(";
            for (size_t i = 0; i < t.params.size(); ++i) {
                if (i > 0) s += ", ";
                s += typeToString(t.params[i]);
            }
            s += ")";
            if (t.returnType && t.returnType->name != "void") {
                s += " -> " + typeToString(*t.returnType);
            }
            return s;
        }
        case SigType::Kind::Class: {
            if (t.params.empty()) return t.name;
            std::string s = t.name + "<";
            for (size_t i = 0; i < t.params.size(); ++i) {
                if (i > 0) s += ", ";
                s += typeToString(t.params[i]);
            }
            s += ">";
            return s;
        }
        case SigType::Kind::Alias: return "alias " + t.name;
        case SigType::Kind::Template: return "template " + t.name;
        default: return t.name.empty() ? "?" : t.name;
    }
}

std::string SigParser::declToString(const SigDecl& d) {
    std::ostringstream ss;
    switch (d.kind) {
        case SigDecl::Kind::Function:
            ss << "fn " << d.name;
            if (!d.templateParams.empty()) {
                ss << "<";
                for (size_t i = 0; i < d.templateParams.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << d.templateParams[i];
                }
                ss << ">";
            }
            ss << " (";
            for (size_t i = 0; i < d.params.size(); ++i) {
                if (i > 0) ss << ", ";
                if (!d.params[i].first.empty()) ss << d.params[i].first << ": ";
                ss << typeToString(d.params[i].second);
            }
            ss << ")";
            if (d.returnType.name != "void" && d.returnType.name != "") {
                ss << " -> " << typeToString(d.returnType);
            }
            break;
        case SigDecl::Kind::Class:
            ss << "class " << d.name;
            if (!d.templateParams.empty()) {
                ss << "<";
                for (size_t i = 0; i < d.templateParams.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << d.templateParams[i];
                }
                ss << ">";
            }
            if (!d.bases.empty()) {
                ss << " : ";
                for (size_t i = 0; i < d.bases.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << d.bases[i];
                }
            }
            ss << " { ";
            for (const auto& f : d.fields) {
                ss << f.first << ": " << typeToString(f.second) << "; ";
            }
            for (const auto& m : d.methods) {
                ss << declToString(m) << "; ";
            }
            ss << "}";
            break;
        case SigDecl::Kind::Alias:
            ss << "alias " << d.name << " = " << typeToString(d.aliasTarget);
            break;
        case SigDecl::Kind::Meta:
            ss << "meta " << d.name;
            if (!d.templateParams.empty()) {
                ss << "<";
                for (size_t i = 0; i < d.templateParams.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << d.templateParams[i];
                }
                ss << ">";
            }
            ss << " (";
            for (size_t i = 0; i < d.params.size(); ++i) {
                if (i > 0) ss << ", ";
                if (!d.params[i].first.empty()) ss << d.params[i].first << ": ";
                ss << typeToString(d.params[i].second);
            }
            ss << ")";
            if (d.returnType.name != "void" && d.returnType.name != "") {
                ss << " -> " << typeToString(d.returnType);
            }
            break;
    }
    return ss.str();
}

bool SigType::operator==(const SigType& other) const {
    if (kind != other.kind) return false;
    if (name != other.name) return false;
    if (params.size() != other.params.size()) return false;
    for (size_t i = 0; i < params.size(); ++i) {
        if (!(params[i] == other.params[i])) return false;
    }
    if (!returnType || !other.returnType) return !returnType && !other.returnType;
    return *returnType == *other.returnType;
}

} // namespace at
