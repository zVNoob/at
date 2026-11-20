#pragma once

#include "symbols.h"

#include <map>
#include <set>
#include <string>
#include <vector>

namespace at {

// Tracks file dependencies and timestamps for incremental compilation.
class IncrementalBuilder {
public:
    IncrementalBuilder();

    // Register a source file and its output file.
    void addFile(const std::string& sourcePath, const std::string& outputPath);

    // Register a dependency: outputFile depends on dependencyPath.
    void addDependency(const std::string& outputFile, const std::string& dependencyPath);

    // Check if a file needs recompilation.
    // Returns true if source is newer than output, or output doesn't exist.
    bool needsRecompile(const std::string& sourcePath, const std::string& outputPath) const;

    // Get the list of files that need recompilation.
    std::vector<std::string> filesToRecompile() const;

    // Mark a file as compiled (update timestamp tracking).
    void markCompiled(const std::string& sourcePath, const std::string& outputPath);

    // Get all registered source files.
    std::vector<std::string> sourceFiles() const;

    // Load a build manifest from disk.
    bool loadManifest(const std::string& path);

    // Save a build manifest to disk.
    bool saveManifest(const std::string& path) const;

private:
    struct FileInfo {
        std::string sourcePath;
        std::string outputPath;
        std::set<std::string> dependencies;
        long long sourceMtime = 0;
        long long outputMtime = 0;
    };

    std::map<std::string, FileInfo> files_;
    std::string manifestPath_;

    long long getFileMtime(const std::string& path) const;
};

} // namespace at
