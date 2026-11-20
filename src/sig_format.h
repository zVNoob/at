#pragma once

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace at {

// Represents a parsed signature from .at.sigs
struct SigType {
    enum class Kind { Primitive, Pointer, Reference, Array, Function, Class, Alias, Template, Unknown };
    Kind kind = Kind::Unknown;
    std::string name; // type name or "int", "str", etc.
    std::vector<SigType> params; // for function params, template args, class bases
    std::vector<std::pair<std::string, SigType>> fields; // for classes
    std::shared_ptr<SigType> returnType; // for functions (nullable)
    int templateArity = 0; // number of template params
    bool isMeta = false; // type-erased template

    bool operator==(const SigType& other) const;
};

// A parsed signature declaration
struct SigDecl {
    enum class Kind { Function, Class, Alias, Meta };
    Kind kind = Kind::Function;
    std::string name;
    std::vector<std::string> templateParams; // template param names
    std::vector<std::pair<std::string, SigType>> params; // (name, type) for functions
    SigType returnType;
    std::vector<std::string> bases; // for classes
    std::vector<std::pair<std::string, SigType>> fields; // for classes
    std::vector<SigDecl> methods; // for classes
    SigType aliasTarget; // for aliases
};

// Parser for the .at.sigs format
class SigParser {
public:
    // Parse a single signature line
    static SigDecl parse(const std::string& line);

    // Parse type string into SigType
    static SigType parseType(const std::string& s);

    // Convert SigType to string
    static std::string typeToString(const SigType& t);

    // Convert SigDecl to string
    static std::string declToString(const SigDecl& d);
};

} // namespace at
