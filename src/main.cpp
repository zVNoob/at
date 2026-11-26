#include "codegen.h"
#include "interpreter.h"
#include "parser.h"
#include "semantic.h"
#include "sig_bridge.h"
#include "incremental_builder.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <functional>
#include <vector>
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>

namespace {
void dump(const at::AstPtr& node, int indent = 0) {
    if (!node) return;
    for (int i = 0; i < indent; ++i) std::cout << "  ";
    std::cout << node->kind;
    if (!node->text.empty()) std::cout << " [" << node->text << "]";
    std::cout << "\n";
    for (const auto& c : node->children) dump(c, indent + 1);
}

// Compile an imported .at file in-process (no subprocess, no shell injection).
bool compileImportedModule(const std::string& path, const std::string& outputPath,
                          const std::string& initFuncName) {
    std::ifstream f(path);
    if (!f) {
        std::cerr << "Warning: cannot open imported module: " << path << "\n";
        return false;
    }
    std::string input((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

    at::AstPtr ast = at::parseSource(input);
    if (!ast) {
        std::cerr << "Warning: parse failed for imported module: " << path << "\n";
        return false;
    }

    at::SemanticAnalyzer sem;
    sem.analyze(ast);
    if (!sem.errors().empty()) {
        std::cerr << "Warning: semantic errors in imported module: " << path << "\n";
        for (const auto& e : sem.errors()) {
            std::cerr << "  " << e.message << "\n";
        }
        return false;
    }

    at::CodeGen cg;
    cg.setSignatures(at::declsFromScope(*sem.globalScope()));
    if (!cg.compile(ast, outputPath, false, {}, initFuncName)) {
        std::cerr << "Warning: codegen failed for imported module: " << path << "\n";
        return false;
    }
    return true;
}

// Run cc linker via fork+execvp (no shell injection).
bool linkExecutable(const std::vector<std::string>& objFiles,
                    const std::string& outputPath) {
    pid_t pid = fork();
    if (pid < 0) {
        std::cerr << "linking failed: fork error\n";
        return false;
    }
    if (pid == 0) {
        // Child process
        std::vector<char*> args;
        args.push_back(const_cast<char*>("cc"));
        args.push_back(const_cast<char*>("-no-pie"));
        for (const auto& o : objFiles) {
            args.push_back(const_cast<char*>(o.c_str()));
        }
        args.push_back(const_cast<char*>("src/at_runtime.cpp"));
        args.push_back(const_cast<char*>("-o"));
        args.push_back(const_cast<char*>(outputPath.c_str()));
        args.push_back(nullptr);

        execvp("cc", args.data());
        _exit(127);
    }

    // Parent process
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        return true;
    }
    std::cerr << "linking failed (exit " << WEXITSTATUS(status) << ")\n";
    return false;
}

bool linkSharedLib(const std::string& objFile, const std::string& outputPath) {
    pid_t pid = fork();
    if (pid < 0) {
        std::cerr << "linking failed: fork error\n";
        return false;
    }
    if (pid == 0) {
        std::vector<char*> args;
        args.push_back(const_cast<char*>("cc"));
        args.push_back(const_cast<char*>("-shared"));
        args.push_back(const_cast<char*>(objFile.c_str()));
        args.push_back(const_cast<char*>("-o"));
        args.push_back(const_cast<char*>(outputPath.c_str()));
        args.push_back(nullptr);

        execvp("cc", args.data());
        _exit(127);
    }

    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
        return true;
    }
    std::cerr << "linking failed (exit " << WEXITSTATUS(status) << ")\n";
    return false;
}
}

int main(int argc, char** argv) {
    std::string input;
    std::string inputFile;
    std::string outputFile;
    std::string buildDir;
    bool compileMode = false;
    bool emitObj = false;
    bool emitMain = true;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--compile" || arg == "-c") {
            compileMode = true;
        } else if (arg == "--emit-obj") {
            compileMode = true;
            emitObj = true;
        } else if (arg == "--no-main") {
            emitMain = false;
        } else if (arg == "-B" && i + 1 < argc) {
            buildDir = argv[++i];
        } else if (arg == "-o" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg.substr(0, 3) == "-o=") {
            outputFile = arg.substr(3);
        } else if (inputFile.empty()) {
            inputFile = arg;
        }
    }

    if (!inputFile.empty()) {
        std::ifstream f(inputFile);
        input.assign((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    } else {
        input = R"(
x : \`0\` = 42;
y : \`0\` = x + 1;
)";
    }

    at::AstPtr ast = at::parseSource(input);
    if (!ast) {
        std::cerr << "parse failed\n";
        return 1;
    }

    dump(ast);

    at::SemanticAnalyzer sem;
    sem.analyze(ast);
    std::cout << "Semantic errors: " << sem.errors().size() << "\n";
    for (const auto& e : sem.errors()) {
        std::cout << "  " << e.message << "\n";
    }

    if (!sem.errors().empty() && compileMode) return 1;

    // Create build directory if specified
    if (!buildDir.empty()) {
        std::filesystem::create_directories(buildDir);
    }

    // Scan AST for 'import 'file.at'' statements and compile imported .at files to .o
    std::vector<std::string> importedObjFiles;
    std::vector<std::string> importedInitFuncs;
    std::map<std::string, std::string> importedModules;
    at::IncrementalBuilder builder;
    std::function<void(const at::AstPtr&)> scanImports = [&](const at::AstPtr& node) {
        if (!node) return;
        if (node->kind == "stmt" && node->children.size() >= 3 &&
            node->children[0] && node->children[0]->kind == "expr" &&
            node->children[0]->children.size() >= 1 &&
            node->children[0]->children[0] && node->children[0]->children[0]->kind == "IDENTIFIER" &&
            node->children[1] && node->children[1]->kind == "CHAR(61)" &&
            node->children[2] && node->children[2]->kind == "expr" &&
            node->children[2]->children.size() == 1 &&
            node->children[2]->children[0] && node->children[2]->children[0]->kind == "IMPORT") {
            std::string varName = node->children[0]->children[0]->text;
            std::string path = node->children[2]->children[0]->text;
            if (path.size() >= 2 && (path.front() == '"' || path.front() == '\'')) {
                path = path.substr(1, path.size() - 2);
            }
            if (path.rfind(".at") == path.size() - 3) {
                size_t slash = path.find_last_of("/\\");
                std::string modName = (slash != std::string::npos) ? path.substr(slash + 1) : path;
                size_t dot = modName.find_last_of('.');
                if (dot != std::string::npos) modName = modName.substr(0, dot);
                importedModules[varName] = modName;

                std::string importedObj = buildDir.empty() ? ("/tmp/" + modName + ".o") : (buildDir + "/" + modName + ".o");
                builder.addFile(path, importedObj);
                builder.addDependency(importedObj, path);

                if (!builder.needsRecompile(path, importedObj)) {
                    importedObjFiles.push_back(importedObj);
                    importedInitFuncs.push_back(modName + "_init");
                    std::cout << "Imported module (cached): " << modName << " from " << path << "\n";
                    return;
                }

                if (compileImportedModule(path, importedObj, modName + "_init")) {
                    builder.markCompiled(path, importedObj);
                    importedObjFiles.push_back(importedObj);
                    importedInitFuncs.push_back(modName + "_init");
                    std::cout << "Imported module: " << modName << " from " << path << "\n";
                } else {
                    std::cerr << "Warning: failed to compile imported module: " << path << "\n";
                }
            }
        }
        for (const auto& c : node->children) scanImports(c);
    };
    scanImports(ast);

    if (compileMode) {
        std::string baseName = inputFile;
        size_t slash = baseName.find_last_of("/\\");
        if (slash != std::string::npos) baseName = baseName.substr(slash + 1);
        size_t dot = baseName.find_last_of('.');
        if (dot != std::string::npos) baseName = baseName.substr(0, dot);

        bool isSharedLib = !outputFile.empty() && outputFile.rfind(".so") == outputFile.size() - 3;

        std::string objFile;
        std::string outFile;

        if (emitObj) {
            objFile = outputFile.empty() ? (baseName + ".o") : outputFile;
            if (!buildDir.empty() && outputFile.empty()) {
                objFile = buildDir + "/" + baseName + ".o";
            } else if (!buildDir.empty() && !outputFile.empty()) {
                objFile = buildDir + "/" + outputFile;
            }
            outFile = "";
        } else if (isSharedLib) {
            objFile = baseName + ".o";
            if (!buildDir.empty()) objFile = buildDir + "/" + objFile;
            outFile = outputFile.empty() ? (baseName + ".so") : outputFile;
        } else {
            outFile = outputFile.empty() ? baseName : outputFile;
            objFile = buildDir.empty() ? (outFile + ".o") : (buildDir + "/" + outFile + ".o");
        }

        // Check if main needs recompilation
        if (builder.needsRecompile(inputFile, objFile)) {
            at::CodeGen cg;
            cg.setSignatures(at::declsFromScope(*sem.globalScope()));
            if (!cg.compile(ast, objFile, emitMain, importedInitFuncs)) {
                std::cerr << "codegen failed\n";
                return 1;
            }
            builder.markCompiled(inputFile, objFile);
            std::cout << "compiled to " << objFile << "\n";
        } else {
            std::cout << "cached: " << objFile << "\n";
        }

        if (!emitObj) {
            if (isSharedLib) {
                if (!linkSharedLib(objFile, outFile)) {
                    return 1;
                }
                std::cout << "linked shared library to " << outFile << "\n";
            } else {
                // Link imported modules first (their constructors run before main),
                // then main module, then runtime support
                std::vector<std::string> allObjs = importedObjFiles; allObjs.push_back(objFile); if (!linkExecutable(allObjs, outFile)) {
                    return 1;
                }
                std::cout << "linked to " << outFile << "\n";
            }
        }
    } else {
        at::Interpreter interp;
        if (!interp.jitModule(ast, sem, true)) {
            std::cerr << "interpreter: JIT compilation failed\n";
            return 1;
        }
        int64_t result = interp.callJitSymbol("main");
        at::Value v;
        v.kind = at::Value::Kind::Int;
        v.intValue = result;
        std::cout << "Interpreter result: int:" << result << "\n";
    }
    return 0;
}
