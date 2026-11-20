#include "elf_reader.h"

#include <fcntl.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <sstream>

namespace at {

ElfReader::ElfReader() {
    if (elf_version(EV_CURRENT) == EV_NONE) {
        std::cerr << "ELF: library initialization failed\n";
    }
}

ElfReader::~ElfReader() {
    close();
}

bool ElfReader::open(const std::string& path) {
    close();
    path_ = path;

    fd_ = ::open(path.c_str(), O_RDONLY);
    if (fd_ < 0) {
        std::cerr << "ELF: cannot open '" << path << "'\n";
        return false;
    }

    elf_ = elf_begin(fd_, ELF_C_READ, nullptr);
    if (!elf_) {
        std::cerr << "ELF: elf_begin failed for '" << path << "': " << elf_errmsg(-1) << "\n";
        ::close(fd_);
        fd_ = -1;
        return false;
    }

    if (elf_kind(elf_) != ELF_K_ELF) {
        std::cerr << "ELF: '" << path << "' is not an ELF file\n";
        close();
        return false;
    }

    return true;
}

void ElfReader::close() {
    if (elf_) {
        elf_end(elf_);
        elf_ = nullptr;
    }
    if (fd_ >= 0) {
        ::close(fd_);
        fd_ = -1;
    }
    path_.clear();
}

std::vector<ElfSymbol> ElfReader::symbols() const {
    std::vector<ElfSymbol> result;
    if (!elf_) return result;

    // Iterate over all sections looking for symbol tables
    Elf_Scn* scn = nullptr;
    while ((scn = elf_nextscn(elf_, scn)) != nullptr) {
        GElf_Shdr shdr;
        if (!gelf_getshdr(scn, &shdr)) continue;
        if (shdr.sh_type != SHT_SYMTAB && shdr.sh_type != SHT_DYNSYM) continue;

        // Get the section data
        Elf_Data* data = elf_getdata(scn, nullptr);
        if (!data) continue;

        // Get the string table for this symbol table
        Elf_Scn* strScn = elf_getscn(elf_, shdr.sh_link);
        if (!strScn) continue;
        Elf_Data* strData = elf_getdata(strScn, nullptr);
        if (!strData) continue;

        size_t count = shdr.sh_size / shdr.sh_entsize;
        for (size_t i = 0; i < count; ++i) {
            GElf_Sym sym;
            if (!gelf_getsym(data, (int)i, &sym)) continue;

            const char* name = elf_strptr(elf_, shdr.sh_link, sym.st_name);
            if (!name || name[0] == '\0') continue;

            ElfSymbol es;
            es.name = name;
            es.value = sym.st_value;
            es.size = sym.st_size;
            es.bind = GELF_ST_BIND(sym.st_info);
            es.type = GELF_ST_TYPE(sym.st_info);
            es.visibility = GELF_ST_VISIBILITY(sym.st_other);
            es.sectionIndex = sym.st_shndx;

            result.push_back(es);
        }
    }

    return result;
}

std::vector<ElfSection> ElfReader::sections() const {
    std::vector<ElfSection> result;
    if (!elf_) return result;

    Elf_Scn* scn = nullptr;
    while ((scn = elf_nextscn(elf_, scn)) != nullptr) {
        GElf_Shdr shdr;
        if (!gelf_getshdr(scn, &shdr)) continue;

        size_t shdrstrndx = 0;
        elf_getshdrstrndx(elf_, &shdrstrndx);
        const char* name = elf_strptr(elf_, shdrstrndx, shdr.sh_name);
        if (!name) name = "";

        ElfSection es;
        es.name = name;
        es.addr = shdr.sh_addr;
        es.size = shdr.sh_size;
        es.flags = shdr.sh_flags;
        es.type = shdr.sh_type;

        result.push_back(es);
    }

    return result;
}

std::string ElfReader::architecture() const {
    if (!elf_) return "";

    GElf_Ehdr ehdr;
    if (!gelf_getehdr(elf_, &ehdr)) return "";

    switch (ehdr.e_machine) {
        case EM_X86_64: return "x86-64";
        case EM_386: return "x86";
        case EM_AARCH64: return "aarch64";
        case EM_ARM: return "arm";
        case EM_RISCV: return "riscv";
        default: return "unknown (" + std::to_string(ehdr.e_machine) + ")";
    }
}

bool ElfReader::is64Bit() const {
    if (!elf_) return false;
    GElf_Ehdr ehdr;
    if (!gelf_getehdr(elf_, &ehdr)) return false;
    return (ehdr.e_ident[EI_CLASS] == ELFCLASS64);
}

uint16_t ElfReader::fileType() const {
    if (!elf_) return 0;
    GElf_Ehdr ehdr;
    if (!gelf_getehdr(elf_, &ehdr)) return 0;
    return ehdr.e_type;
}

std::vector<uint8_t> ElfReader::sectionData(const std::string& wanted) const {
    std::vector<uint8_t> result;
    if (!elf_) return result;

    size_t shstrndx = 0;
    if (elf_getshdrstrndx(elf_, &shstrndx) != 0) return result;

    Elf_Scn* scn = nullptr;
    while ((scn = elf_nextscn(elf_, scn)) != nullptr) {
        GElf_Shdr shdr;
        if (!gelf_getshdr(scn, &shdr)) continue;
        const char* name = elf_strptr(elf_, shstrndx, shdr.sh_name);
        if (!name || wanted != name) continue;

        Elf_Data* data = nullptr;
        while ((data = elf_getdata(scn, data)) != nullptr) {
            if (!data->d_buf) continue;
            const uint8_t* p = static_cast<const uint8_t*>(data->d_buf);
            result.insert(result.end(), p, p + data->d_size);
        }
        break;
    }
    return result;
}

std::vector<std::string> ElfReader::signatures() const {
    std::vector<std::string> result;
    std::vector<uint8_t> bytes = sectionData(".at.sigs");
    if (bytes.size() >= 4 && bytes[0] == 'A' && bytes[1] == 'T' && bytes[2] == 'S' && bytes[3] == 'G') return result;

    std::string content(bytes.begin(), bytes.end());
    std::istringstream iss(content);
    std::string line;
    while (std::getline(iss, line)) {
        // The C string literal carries a trailing NUL; drop it and any stray CR.
        while (!line.empty() && (line.back() == '\0' || line.back() == '\r')) line.pop_back();
        if (!line.empty()) result.push_back(line);
    }
    return result;
}

} // namespace at
