#include "incremental_builder.h"

#include <cassert>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

int main() {
    // Create a source file
    {
        std::ofstream f("/tmp/inc_test.at");
        f << "x : `0` = 42;\n";
        f.close();
    }

    at::IncrementalBuilder builder;
    builder.addFile("/tmp/inc_test.at", "/tmp/inc_test.o");

    // Output doesn't exist yet -> needs recompile
    std::remove("/tmp/inc_test.o");
    assert(builder.needsRecompile("/tmp/inc_test.at", "/tmp/inc_test.o"));

    // Create output file
    {
        std::ofstream f("/tmp/inc_test.o");
        f << "dummy";
        f.close();
    }
    // Now source is older than output -> no recompile needed
    // (but mtime might be same second, so touch source to be sure)
    std::this_thread::sleep_for(std::chrono::seconds(1));
    {
        std::ofstream f("/tmp/inc_test.at", std::ios::app);
        f << " // modified\n";
        f.close();
    }
    assert(builder.needsRecompile("/tmp/inc_test.at", "/tmp/inc_test.o"));

    // filesToRecompile should include it
    auto files = builder.filesToRecompile();
    assert(!files.empty());

    // Manifest save/load
    builder.saveManifest("/tmp/inc_manifest.txt");
    at::IncrementalBuilder builder2;
    builder2.loadManifest("/tmp/inc_manifest.txt");
    assert(!builder2.sourceFiles().empty());

    std::cout << "incremental_test: PASS\n";
    return 0;
}