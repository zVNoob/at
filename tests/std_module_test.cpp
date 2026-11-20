#include "std_modules.h"
#include "object_importer.h"

#include <cassert>
#include <iostream>
#include <string>

int main() {
    at::StdModuleManager mgr;
    size_t count = mgr.loadFromDirectory("std_modules");
    std::cout << "Loaded " << count << " modules\n";
    assert(count >= 1);

    auto names = mgr.moduleNames();
    assert(!names.empty());

    auto table = mgr.moduleTable("math");
    assert(table != nullptr);

    auto sym = table->lookup("main");
    assert(sym != nullptr);

    std::cout << "std_module_test: PASS\n";
    return 0;
}
