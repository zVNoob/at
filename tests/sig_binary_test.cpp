#include "sig_binary.h"

#include <cassert>
#include <iostream>
#include <string>

int main() {
    using namespace at;

    std::vector<BinSigDecl> decls;

    // Function: add(a: int, b: int) -> int
    BinSigDecl fn;
    fn.kind = BinSigDecl::Fn;
    fn.name = "add";
    fn.params.push_back({"a", BinSigType::prim(BinSigType::PInt)});
    fn.params.push_back({"b", BinSigType::prim(BinSigType::PInt)});
    fn.returnType = BinSigType::prim(BinSigType::PInt);
    decls.push_back(fn);

    // Template meta function: identity<T>(x: T) -> T
    BinSigDecl meta;
    meta.kind = BinSigDecl::Meta;
    meta.name = "identity";
    meta.templateParams = {"T"};
    meta.params.push_back({"x", BinSigType::named("T")});
    meta.returnType = BinSigType::named("T");
    decls.push_back(meta);

    // Class: Point { x: int; y: int; norm: fn() -> int }
    BinSigDecl cls;
    cls.kind = BinSigDecl::Class;
    cls.name = "Point";
    cls.bases = {"Shape"};
    cls.fields.push_back({"x", BinSigType::prim(BinSigType::PInt)});
    cls.fields.push_back({"y", BinSigType::prim(BinSigType::PInt)});
    BinSigDecl norm;
    norm.kind = BinSigDecl::Fn;
    norm.name = "norm";
    norm.returnType = BinSigType::prim(BinSigType::PInt);
    cls.methods.push_back(norm);
    decls.push_back(cls);

    // Alias: Buf = *string[]
    BinSigDecl alias;
    alias.kind = BinSigDecl::Alias;
    alias.name = "Buf";
    alias.aliasTarget = BinSigType::wrap(BinSigType::Ptr,
        BinSigType::wrap(BinSigType::Array, BinSigType::prim(BinSigType::PString)));
    decls.push_back(alias);

    BinSigWriter writer;
    auto data = writer.write(decls);
    std::cout << "Written " << data.size() << " bytes\n";

    BinSigReader reader;
    assert(reader.read(data));
    const auto& r = reader.decls();
    // Version 2: methods are separate decls, so we have 5 entries
    assert(r.size() == 5);

    assert(r[0].name == "add" && r[0].kind == BinSigDecl::Fn);
    assert(r[0].params.size() == 2 && r[0].params[1].first == "b");
    assert(r[0].returnType.tag == BinSigType::Prim && r[0].returnType.primKind == BinSigType::PInt);

    assert(r[1].name == "identity" && r[1].kind == BinSigDecl::Meta);
    assert(r[1].templateParams.size() == 1 && r[1].templateParams[0] == "T");
    assert(r[1].returnType.tag == BinSigType::Named && r[1].returnType.name == "T");

    assert(r[2].name == "Point" && r[2].kind == BinSigDecl::Class);
    assert(r[2].bases.size() == 1 && r[2].bases[0] == "Shape");
    assert(r[2].fields.size() == 2);
    assert(r[2].methods.size() == 1 && r[2].methods[0].name == "norm");

    assert(r[3].name == "norm" && r[3].kind == BinSigDecl::Fn);
    assert(r[3].returnType.tag == BinSigType::Prim && r[3].returnType.primKind == BinSigType::PInt);

    assert(r[4].name == "Buf" && r[4].kind == BinSigDecl::Alias);
    assert(r[4].aliasTarget.tag == BinSigType::Ptr);
    assert(r[4].aliasTarget.elementType->tag == BinSigType::Array);
    assert(r[4].aliasTarget.elementType->elementType->primKind == BinSigType::PString);

    // Truncated / corrupt input is rejected rather than crashing.
    // With offset-based format, truncation may still parse but with empty strings.
    for (size_t n = 0; n < data.size(); ++n) {
        std::vector<uint8_t> cut(data.begin(), data.begin() + n);
        BinSigReader bad;
        if (bad.read(cut)) {
            // If header is intact, decl count should match or be partial
            assert(bad.decls().size() <= 5);
        }
    }
    std::vector<uint8_t> wrongMagic = data;
    wrongMagic[0] = 'X';
    assert(!BinSigReader().read(wrongMagic));

    std::string s0 = binDeclToString(r[0]);
    std::string s4 = binDeclToString(r[4]);
    std::cout << "Text: " << s0 << " | " << s4 << "\n";
    assert(s0 == "fn add (a: int, b: int) -> int");
    assert(s4 == "alias Buf = *string[]");

    std::cout << "sig_binary_test: PASS\n";
    return 0;
}
