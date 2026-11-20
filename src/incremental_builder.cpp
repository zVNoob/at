#include "incremental_builder.h"

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/stat.h>

namespace at {

IncrementalBuilder::IncrementalBuilder() = default;

long long IncrementalBuilder::getFileMtime(const std::string& path) const {
    struct stat st;
    if (stat(path.c_str(), &st) != 0) return 0;
    return (long long)st.st_mtime;
}

void IncrementalBuilder::addFile(const std::string& sourcePath, const std::string& outputPath) {
    FileInfo info;
    info.sourcePath = sourcePath;
    info.outputPath = outputPath;
    info.sourceMtime = getFileMtime(sourcePath);
    info.outputMtime = getFileMtime(outputPath);
    files_[sourcePath] = info;
}

void IncrementalBuilder::addDependency(const std::string& outputFile, const std::string& dependencyPath) {
    for (auto& [src, info] : files_) {
        if (info.outputPath == outputFile) {
            info.dependencies.insert(dependencyPath);
        }
    }
}

bool IncrementalBuilder::needsRecompile(const std::string& sourcePath, const std::string& outputPath) const {
    long long srcMtime = getFileMtime(sourcePath);
    long long outMtime = getFileMtime(outputPath);

    // Output doesn't exist
    if (outMtime == 0) return true;

    // Source is newer than output
    if (srcMtime > outMtime) return true;

    // Check dependencies
    for (const auto& [src, info] : files_) {
        if (info.sourcePath == sourcePath) {
            for (const auto& dep : info.dependencies) {
                if (getFileMtime(dep) > outMtime) return true;
            }
        }
    }

    return false;
}

std::vector<std::string> IncrementalBuilder::filesToRecompile() const {
    std::vector<std::string> result;
    for (const auto& [src, info] : files_) {
        if (needsRecompile(info.sourcePath, info.outputPath)) {
            result.push_back(info.sourcePath);
        }
    }
    return result;
}

void IncrementalBuilder::markCompiled(const std::string& sourcePath, const std::string& outputPath) {
    auto it = files_.find(sourcePath);
    if (it != files_.end()) {
        it->second.sourceMtime = getFileMtime(sourcePath);
        it->second.outputMtime = getFileMtime(outputPath);
    }
}

std::vector<std::string> IncrementalBuilder::sourceFiles() const {
    std::vector<std::string> result;
    for (const auto& [src, info] : files_) {
        result.push_back(src);
    }
    return result;
}

bool IncrementalBuilder::loadManifest(const std::string& path) {
    manifestPath_ = path;
    std::ifstream file(path);
    if (!file) return false;

    files_.clear();
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        std::istringstream iss(line);
        std::string src, out;
        iss >> src >> out;
        if (!src.empty() && !out.empty()) {
            addFile(src, out);
        }
    }
    return true;
}

bool IncrementalBuilder::saveManifest(const std::string& path) const {
    std::ofstream file(path);
    if (!file) return false;
    for (const auto& [src, info] : files_) {
        file << info.sourcePath << " " << info.outputPath << "\n";
    }
    return true;
}

} // namespace at
