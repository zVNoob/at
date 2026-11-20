#pragma once

#include "symbols.h"

#include <elf.h>
#include <gelf.h>
#include <libelf.h>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace at {

// ELF section info
struct ElfSection {
    std::string name;
    uint64_t addr = 0;
    uint64_t size = 0;
    uint64_t flags = 0;
    uint32_t type = 0;
};

// ELF symbol info
struct ElfSymbol {
    std::string name;
    uint64_t value = 0;
    uint64_t size = 0;
    unsigned char bind = 0;
    unsigned char type = 0;
    unsigned char visibility = 0;
    uint16_t sectionIndex = 0;
};

// Reads ELF object files and extracts symbols/sections.
class ElfReader {
public:
    ElfReader();
    ~ElfReader();

    // Open an ELF file. Returns false on failure.
    bool open(const std::string& path);
    void close();

    // Get all symbols (functions, objects, etc.)
    std::vector<ElfSymbol> symbols() const;

    // Get all sections
    std::vector<ElfSection> sections() const;

    // Get the architecture (e.g., "x86-64")
    std::string architecture() const;

    // Get whether the file is 32 or 64 bit
    bool is64Bit() const;

    // Get the file type (ET_REL, ET_EXEC, ET_DYN)
    uint16_t fileType() const;

    // Raw bytes of the named section (empty if absent).
    std::vector<uint8_t> sectionData(const std::string& name) const;

    // Get function signatures from a legacy *text* .at.sigs section.
    // Format: "name param_types... -> return_type" (one per line).
    // Returns empty if the section is in the binary "ATSG" format.
    std::vector<std::string> signatures() const;

    bool isOpen() const { return elf_ != nullptr; }

private:
    Elf* elf_ = nullptr;
    int fd_ = -1;
    std::string path_;
};

} // namespace at
