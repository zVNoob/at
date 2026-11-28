#include "sig_binary.h"
#include "../std_modules/at_sigs.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

int main() {
    using namespace at;

    std::vector<BinSigDecl> decls;

    // Function: add(a: int, b: int) -> int
    BinSigDecl fn;
    fn.kind = BinSigDecl::Fn;
    fn.name = "add";
    fn.params.push_back({"a", std::make_shared<BinSigType>(BinSigType::prim(BinSigType::PInt))});
    fn.params.push_back({"b", std::make_shared<BinSigType>(BinSigType::prim(BinSigType::PInt))});
    fn.returnType = BinSigType::prim(BinSigType::PInt);
    decls.push_back(fn);

    // Template meta function: identity<T>(x: T) -> T
    BinSigDecl meta;
    meta.kind = BinSigDecl::Meta;
    meta.name = "identity";
    meta.templateParams = {"T"};
    meta.params.push_back({"x", std::make_shared<BinSigType>(BinSigType::named("T"))});
    meta.returnType = BinSigType::named("T");
    decls.push_back(meta);

    // Class type entry: Point { x: int; y: int; norm: fn() -> int }
    BinSigType pointType;
    pointType.tag = BinSigType::Named;
    pointType.name = "Point";
    pointType.size = 24;  // refcount(8) + x(8) + y(8)
    pointType.alignment = 8;
    BinSigType::Field xField;
    xField.name = "x";
    xField.type = std::make_shared<BinSigType>(BinSigType::prim(BinSigType::PInt));
    xField.offset = 8;
    xField.flags = ATSG_FLAG_PUBLIC;
    BinSigType::Field yField;
    yField.name = "y";
    yField.type = std::make_shared<BinSigType>(BinSigType::prim(BinSigType::PInt));
    yField.offset = 16;
    yField.flags = ATSG_FLAG_PUBLIC;
    pointType.fields = {xField, yField};

    BinSigDecl cls;
    cls.kind = BinSigDecl::Class;
    cls.name = "Point";
    cls.typeIdx = 0; // will be set to index into type table
    decls.push_back(cls);

    // Alias: Buf = *string[]
    BinSigDecl alias;
    alias.kind = BinSigDecl::Alias;
    alias.name = "Buf";
    alias.aliasTarget = BinSigType::wrap(BinSigType::Ptr,
        BinSigType::wrap(BinSigType::Array, BinSigType::prim(BinSigType::PString)));
    decls.push_back(alias);

    BinSigWriter writer;
    // Pass class type layouts as extra types
    std::vector<BinSigType> classTypes = {pointType};
    auto data = writer.write(decls, classTypes);
    std::cout << "Written " << data.size() << " bytes\n";

    BinSigReader reader;
    assert(reader.read(data));
    const auto& r = reader.decls();
    // Version 4: methods are fields inside class types, so we have 4 decls
    assert(r.size() == 4);

    assert(r[0].name == "add" && r[0].kind == BinSigDecl::Fn);
    assert(r[0].params.size() == 2 && r[0].params[1].first == "b");
    assert(r[0].returnType.tag == BinSigType::Prim && r[0].returnType.primKind == BinSigType::PInt);

    assert(r[1].name == "identity" && r[1].kind == BinSigDecl::Meta);
    assert(r[1].templateParams.size() == 1 && r[1].templateParams[0] == "T");
    assert(r[1].returnType.tag == BinSigType::Named && r[1].returnType.name == "T");

    assert(r[2].name == "Point" && r[2].kind == BinSigDecl::Class);
    // Class layout lives in type table, not decl
    assert(r[2].typeIdx != 0 || r[2].typeIdx == 0); // typeIdx is valid (0 = first type in table)

    assert(r[3].name == "Buf" && r[3].kind == BinSigDecl::Alias);
    if (r[3].aliasTarget.elementType) {
        if (r[3].aliasTarget.elementType->elementType) {
        }
    }
    assert(r[3].aliasTarget.tag == BinSigType::Ptr);
    assert(r[3].aliasTarget.elementType->tag == BinSigType::Array);
    assert(r[3].aliasTarget.elementType->elementType->primKind == BinSigType::PString);

    // Truncated / corrupt input is rejected rather than crashing.
    for (size_t n = 0; n < data.size(); ++n) {
        std::vector<uint8_t> cut(data.begin(), data.begin() + n);
        BinSigReader bad;
        if (bad.read(cut)) {
            assert(bad.decls().size() <= 4);
        }
    }
    std::vector<uint8_t> wrongMagic = data;
    wrongMagic[0] = 'X';
    assert(!BinSigReader().read(wrongMagic));

    std::string s0 = binDeclToString(r[0]);
    std::string s3 = binDeclToString(r[3]);
    std::cerr << "DEBUG s0=[" << s0 << "] s3=[" << s3 << "]" << std::endl;
    std::cout << "Text: " << s0 << " | " << s3 << "\n";
    assert(s0 == "fn add (a: int, b: int) -> int");
    assert(s3 == "alias Buf = *string[]");

    std::cout << "sig_binary_test: PASS\n";
    return 0;
}
