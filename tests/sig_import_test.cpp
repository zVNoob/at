// Cross-module type resolution through the .at.sigs section:
//   module source --(semantic + codegen)--> .o with binary ATSG signatures
//   .o --(ObjectImporter)--> typed symbols --(importSymbols)--> type checker
#include "codegen.h"
#include "object_importer.h"
#include "parser.h"
#include "semantic.h"
#include "sig_bridge.h"

#include <cassert>
#include <filesystem>
#include <iostream>
#include <string>

namespace {

size_t errorsFor(const std::string& src, const std::vector<at::SymbolPtr>& imports, bool print = false) {
    auto ast = at::parseSource(src);
    assert(ast != nullptr);
    at::SemanticAnalyzer sem;
    sem.importSymbols(imports);
    sem.analyze(ast);
    if (print) for (const auto& e : sem.errors()) std::cout << "    " << e.message << "\n";
    return sem.errors().size();
}

const at::BinSigDecl* findSig(const at::ObjectImporter& imp, const std::string& name) {
    for (const auto& d : imp.signatures()) if (d.name == name) return &d;
    return nullptr;
}

} // namespace

int main() {
    std::filesystem::create_directories("/tmp/opencode");
    const std::string obj = "/tmp/opencode/sig_import_mod.o";

    // ---- 1. Compile a module with typed exports -------------------------
    const std::string modSrc =
        "Id := <| T |> @ (x : T) { @< x; };\n"
        "P := # (x : `0`, name : `\"\"`) { @< self; };\n"
        "N := `0`;\n"
        "S := `\"\"`;\n";
    {
        auto ast = at::parseSource(modSrc);
        assert(ast != nullptr);
        at::SemanticAnalyzer sem;
        sem.analyze(ast);
        for (const auto& e : sem.errors()) std::cerr << "module error: " << e.message << "\n";
        assert(sem.errors().empty());

        at::CodeGen cg;
        cg.setSignatures(at::declsFromScope(*sem.globalScope()));
        assert(cg.compile(ast, obj));
    }

    // ---- 2. Read signatures back from the object ------------------------
    at::ObjectImporter imp;
    assert(imp.importObjectFile(obj) > 0);
    assert(imp.reader().sectionData(".at.sigs").size() >= 12);

    const at::BinSigDecl* id = findSig(imp, "Id");
    assert(id && id->kind == at::BinSigDecl::Meta);
    assert(id->templateParams.size() == 1 && id->templateParams[0] == "T");
    assert(id->params.size() == 1 && id->params[0].first == "x");
    assert(id->params[0].second.tag == at::BinSigType::Named && id->params[0].second.name == "T");

    const at::BinSigDecl* p = findSig(imp, "P");
    assert(p && p->kind == at::BinSigDecl::Class);
    // Constructor params are NOT auto-members; only explicit declarations
    // (e.g. `a : a;`) become fields. P has none here.
    assert(p->fields.size() == 0);

    const at::BinSigDecl* n = findSig(imp, "N");
    assert(n && n->kind == at::BinSigDecl::Alias);
    assert(n->aliasTarget.tag == at::BinSigType::Prim && n->aliasTarget.primKind == at::BinSigType::PInt);

    // The template is exposed under its language name, not the ELF one.
    auto idSym = imp.lookupLocal("Id");
    assert(idSym && idSym->type && idSym->type->templateParams == 1);
    assert(!imp.lookupLocal("Id_meta"));
    // Class/alias decls have no ELF symbol but are imported from .at.sigs.
    assert(imp.lookupLocal("P") && imp.lookupLocal("N") && imp.lookupLocal("S"));

    for (const auto& d : imp.signatures()) std::cout << "  " << at::binDeclToString(d) << "\n";

    auto imports = imp.allSymbols();

    // ---- 3. Feed the importing module's type checker --------------------
    // Imported aliases resolve to their target type.
    assert(errorsFor("y : N = 5;\n", imports) == 0);
    assert(errorsFor("y : S = \"hi\";\n", imports) == 0);
    assert(errorsFor("y : N = \"no\";\n", imports) == 1);
    assert(errorsFor("y : S = 5;\n", imports) == 1);
    // Imported template: arity is checked, T is deduced (any type accepted).
    assert(errorsFor("Id(1);\nId(\"s\");\n", imports) == 0);
    assert(errorsFor("Id(1, 2);\n", imports) == 1);

    // ---- 4. Native runtime signatures (legacy text .at.sigs) ------------
    at::ObjectImporter io;
    assert(io.importObjectFile("std_modules/io.o") > 0);
    const at::BinSigDecl* fopen = findSig(io, "at_fopen");
    assert(fopen && fopen->params.size() == 2);
    assert(fopen->params[0].second.primKind == at::BinSigType::PString);
    auto natives = io.allSymbols();
    assert(errorsFor("at_strlen(\"abc\");\n", natives) == 0);
    assert(errorsFor("at_strlen(1);\n", natives) == 1);       // expected string
    assert(errorsFor("at_strlen(\"a\", \"b\");\n", natives) == 1); // arity
    assert(errorsFor("n : `0` = at_strlen(\"abc\");\n", natives) == 0);
    assert(errorsFor("s : `\"\"` = at_strlen(\"abc\");\n", natives) == 1); // returns int
    // Natives stay permissive when no signatures are loaded.
    assert(errorsFor("at_strlen(1, 2, 3);\n", {}) == 0);

    std::cout << "sig_import_test: PASS\n";
    return 0;
}
