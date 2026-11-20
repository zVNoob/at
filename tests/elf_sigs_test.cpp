#include "elf_reader.h"

#include <cassert>
#include <iostream>
#include <string>

int main() {
    at::ElfReader reader;
    assert(reader.open("std_modules/io.o"));
    auto sigs = reader.signatures();
    assert(!sigs.empty());
    // Check for at_fopen
    bool found = false;
    for (const auto& s : sigs) {
        if (s.find("at_fopen") == 0) { found = true; break; }
    }
    assert(found);
    reader.close();
    std::cout << "elf_sigs_test: PASS (" << sigs.size() << " signatures)\n";
    return 0;
}