#include "std_modules.h"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace at {

StdModuleManager::StdModuleManager() = default;

void StdModuleManager::addSearchPath(const std::string& path) {
    searchPaths_.push_back(path);
}

size_t StdModuleManager::loadFromDirectory(const std::string& dir) {
    addSearchPath(dir);
    size_t count = 0;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(dir)) {
            if (entry.path().extension() == ".o") {
                std::string name = entry.path().stem().string();
                if (loadModule(name)) ++count;
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "StdModuleManager: cannot read directory '" << dir << "': " << e.what() << "\n";
    }
    return count;
}

std::shared_ptr<SymbolTable> StdModuleManager::loadModule(const std::string& name) {
    auto it = moduleTables_.find(name);
    if (it != moduleTables_.end()) return it->second;

    // Search for the .o file
    for (const auto& path : searchPaths_) {
        std::string fullPath = path + "/" + name + ".o";
        std::ifstream f(fullPath);
        if (f.good()) {
            f.close();
            auto importer = std::make_shared<ObjectImporter>();
            size_t count = importer->importObjectFile(fullPath);
            if (count > 0) {
                moduleTables_[name] = importer;
                modules_[name] = importer;
                return importer;
            }
        }
    }

    std::cerr << "StdModuleManager: module '" << name << "' not found\n";
    return nullptr;
}

std::shared_ptr<SymbolTable> StdModuleManager::moduleTable(const std::string& name) const {
    auto it = moduleTables_.find(name);
    if (it == moduleTables_.end()) return nullptr;
    return it->second;
}

std::vector<std::string> StdModuleManager::moduleNames() const {
    std::vector<std::string> result;
    for (const auto& [name, table] : moduleTables_) {
        result.push_back(name);
    }
    return result;
}

std::vector<SymbolPtr> StdModuleManager::allSymbols() const {
    std::vector<SymbolPtr> result;
    for (const auto& [name, table] : moduleTables_) {
        if (table) {
            // Get exported symbols from the table
            // ObjectImporter doesn't override exportedSymbols, so use base
            // We'll iterate through the table's locals by casting
            auto* importer = dynamic_cast<ObjectImporter*>(table.get());
            if (importer) {
                for (const auto& sym : importer->exportedSymbols()) {
                    result.push_back(sym);
                }
            }
        }
    }
    return result;
}

} // namespace at
