#pragma once

#include "elf_reader.h"
#include "sig_binary.h"
#include "symbols.h"

#include <memory>
#include <string>

namespace at {

// A SymbolTable subclass that imports symbols from ELF .o files.
// This enables incremental building: compile a module to .o, then
// import its symbols without re-parsing the source.
class ObjectImporter : public SymbolTable {
public:
    ObjectImporter();
    ~ObjectImporter() override = default;

    // Import symbols from an ELF object file.
    // Returns the number of symbols imported.
    size_t importObjectFile(const std::string& path) override;

    // Get the ELF reader for the last imported file.
    const ElfReader& reader() const { return reader_; }

    // Get imported function symbols.
    std::vector<std::string> importedFunctions() const;

    // Get imported object symbols.
    std::vector<std::string> importedObjects() const;

    // Check if a symbol was imported from an object file.
    bool isImported(const std::string& name) const;

    // Signatures decoded from the .at.sigs section of the last imported file
    // (binary ATSG format, or the legacy text format converted to BinSigDecl).
    const std::vector<BinSigDecl>& signatures() const { return sigs_; }

    // All imported symbols (public or not), for feeding a semantic analyzer.
    std::vector<SymbolPtr> allSymbols() const;

private:
    std::vector<BinSigDecl> sigs_;
    ElfReader reader_;
    std::vector<std::string> importedFuncs_;
    std::vector<std::string> importedObjs_;
};

} // namespace at
