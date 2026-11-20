#include "codegen.h"
#include "interpreter.h"
#include "parser.h"
#include "semantic.h"
#include "sig_bridge.h"
#include "incremental_builder.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <functional>
#include <vector>

namespace {
void dump(const at::AstPtr& node, int indent = 0) {
    if (!node) return;
    for (int i = 0; i < indent; ++i) std::cout << "  ";
    std::cout << node->kind;
    if (!node->text.empty()) std::cout << " [" << node->text << "]";
    std::cout << "\n";
    for (const auto& c : node->children) dump(c, indent + 1);
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
x : `0` = 42;
y : `0` = x + 1;
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
                    // Strip 'main' symbol from imported module;
                    // initialization is handled by module_init constructor
                    std::string stripCmd = "objcopy --strip-symbol main " + importedObj;
                    std::system(stripCmd.c_str());
                    // Rename module_init to avoid symbol collisions between modules
                    std::string renameCmd = "objcopy --redefine-sym module_init=" + modName + "_module_init " + importedObj;
                    std::system(renameCmd.c_str());
                    importedObjFiles.push_back(importedObj);
                    importedInitFuncs.push_back(modName + "_module_init");
                    std::cout << "Imported module (cached): " << modName << " from " << path << "\n";
                    return;
                }

                std::string importCmd = "./main --emit-obj --no-main -o " + importedObj + " " + path;
                int ret = std::system(importCmd.c_str());
                if (ret == 0) {
                    // Strip 'main' symbol from imported module;
                    // initialization is handled by module_init constructor
                    std::string stripCmd = "objcopy --strip-symbol main " + importedObj;
                    std::system(stripCmd.c_str());
                    // Rename module_init to avoid symbol collisions between modules
                    std::string renameCmd = "objcopy --redefine-sym module_init=" + modName + "_module_init " + importedObj;
                    std::system(renameCmd.c_str());
                    builder.markCompiled(path, importedObj);
                    importedObjFiles.push_back(importedObj);
                    importedInitFuncs.push_back(modName + "_module_init");
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
                std::string linkCmd = "cc -shared " + objFile + " -o " + outFile;
                std::cout << "linking: " << linkCmd << "\n";
                int ret = std::system(linkCmd.c_str());
                if (ret != 0) {
                    std::cerr << "linking failed (exit " << ret << ")\n";
                    return 1;
                }
                std::cout << "linked shared library to " << outFile << "\n";
            } else {
                // Link imported modules first (their constructors run before main),
                // then main module, then runtime support
                std::string linkCmd = "cc -no-pie ";
                for (const auto& o : importedObjFiles) {
                    linkCmd += " " + o;
                }
                linkCmd += " " + objFile;
                linkCmd += " src/at_runtime.cpp";
                linkCmd += " -o " + outFile;
                std::cout << "linking: " << linkCmd << "\n";
                int ret = std::system(linkCmd.c_str());
                if (ret != 0) {
                    std::cerr << "linking failed (exit " << ret << ")\n";
                    return 1;
                }
                std::cout << "linked to " << outFile << "\n";
            }
        }
    } else {
        at::Interpreter interp;
        at::Value result = interp.run(ast);
        std::cout << "Interpreter result: ";
        if (result.kind == at::Value::Kind::Int) std::cout << "int:" << result.intValue.toString();
        else if (result.kind == at::Value::Kind::Decimal) std::cout << "dec:" << result.decimalValue;
        else if (result.kind == at::Value::Kind::String) std::cout << "str:" << result.stringValue;
        else if (result.kind == at::Value::Kind::Bool) std::cout << "bool:" << (result.boolValue ? "true" : "false");
        else if (result.kind == at::Value::Kind::Object) std::cout << "obj:" << result.objectValue->size();
        else if (result.kind == at::Value::Kind::Function) std::cout << "fn";
        else std::cout << "null/void";
        std::cout << "\n";
    }
    return 0;
}
