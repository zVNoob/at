#pragma once

#include "object_importer.h"
#include "symbols.h"

#include <memory>
#include <string>
#include <vector>

namespace at {

// Manages pre-compiled standard library modules.
// Each std module is compiled to a .o file and loaded via ObjectImporter.
class StdModuleManager {
public:
    StdModuleManager();

    // Initialize std modules from a directory containing .o files.
    // Returns number of modules loaded.
    size_t loadFromDirectory(const std::string& dir);

    // Load a specific std module by name (e.g., "math", "io").
    // Returns the symbol table for that module, or nullptr on failure.
    std::shared_ptr<SymbolTable> loadModule(const std::string& name);

    // Get the symbol table for a loaded module.
    std::shared_ptr<SymbolTable> moduleTable(const std::string& name) const;

    // List loaded module names.
    std::vector<std::string> moduleNames() const;

    // Get all symbols from all loaded modules.
    std::vector<SymbolPtr> allSymbols() const;

    // Register a module directory (where .o files are found).
    void addSearchPath(const std::string& path);

private:
    std::vector<std::string> searchPaths_;
    std::map<std::string, std::shared_ptr<ObjectImporter>> modules_;
    std::map<std::string, std::shared_ptr<SymbolTable>> moduleTables_;
};

} // namespace at
