#include "object_importer.h"

#include "sig_bridge.h"

#include <iostream>
#include <map>
#include <set>
#include <sstream>

namespace at {

ObjectImporter::ObjectImporter() = default;

size_t ObjectImporter::importObjectFile(const std::string& path) {
    if (!reader_.open(path)) return 0;

    importedFuncs_.clear();
    importedObjs_.clear();
    sigs_.clear();

    // Decode .at.sigs: binary "ATSG" format, or legacy one-line-per-function text.
    std::vector<uint8_t> raw = reader_.sectionData(".at.sigs");
    if (raw.size() >= 4 && raw[0] == 'A' && raw[1] == 'T' && raw[2] == 'S' && raw[3] == 'G') {
        BinSigReader br;
        if (br.read(raw)) sigs_ = br.decls();
        else std::cerr << "ObjectImporter: malformed .at.sigs in " << path << "\n";
    } else {
        for (const auto& line : reader_.signatures()) {
            BinSigDecl d;
            if (parseLegacySigLine(line, d)) sigs_.push_back(std::move(d));
        }
    }

    // ELF symbol name -> signature
    std::map<std::string, const BinSigDecl*> byElfName;
    for (const auto& d : sigs_) byElfName[elfSymbolNameFor(d)] = &d;

    size_t count = 0;
    std::set<const BinSigDecl*> used;
    for (const auto& esym : reader_.symbols()) {
        // Skip undefined symbols (they're references, not definitions)
        if (esym.sectionIndex == SHN_UNDEF) continue;
        // Skip file/local symbols
        if (esym.bind == STB_LOCAL && esym.type != STT_FUNC) continue;
        if (esym.name.empty()) continue;

        Visibility vis = esym.visibility == STV_HIDDEN ? Visibility::Private : Visibility::Public;

        SymbolPtr sym;
        auto sit = byElfName.find(esym.name);
        if (sit != byElfName.end()) {
            // Typed: expose under the language-level name (Foo, not Foo_meta).
            sym = binSigDeclToSymbol(*sit->second);
            used.insert(sit->second);
        } else {
            sym = std::make_shared<Symbol>();
            sym->name = esym.name;
            sym->kind = esym.type == STT_FUNC ? SymbolKind::Function : SymbolKind::Variable;
            sym->type = nullptr; // no signature recorded
        }
        sym->visibility = vis;

        if (insert(sym)) {
            ++count;
            if (esym.type == STT_FUNC) importedFuncs_.push_back(sym->name);
            else importedObjs_.push_back(sym->name);
        }
    }

    // Declarations with no code of their own (classes, aliases) only exist in .at.sigs.
    for (const auto& d : sigs_) {
        if (used.count(&d)) continue;
        if (d.kind != BinSigDecl::Class && d.kind != BinSigDecl::Alias) continue;
        if (insert(binSigDeclToSymbol(d))) {
            ++count;
            importedObjs_.push_back(d.name);
        }
    }

    return count;
}

std::vector<SymbolPtr> ObjectImporter::allSymbols() const {
    std::vector<SymbolPtr> result;
    for (const auto& [name, sym] : locals_) result.push_back(sym);
    return result;
}

std::vector<std::string> ObjectImporter::importedFunctions() const {
    return importedFuncs_;
}

std::vector<std::string> ObjectImporter::importedObjects() const {
    return importedObjs_;
}

bool ObjectImporter::isImported(const std::string& name) const {
    for (const auto& f : importedFuncs_) if (f == name) return true;
    for (const auto& o : importedObjs_) if (o == name) return true;
    return false;
}

} // namespace at
