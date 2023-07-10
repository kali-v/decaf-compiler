#include <algorithm>
#include <iomanip>
#include <iostream>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>
#include <string>

#include "LLVMIRGenerator.hpp"
#include "SymbolTable.hpp"
#include "decaf.tab.hpp"
#include "node.hpp"
#include <FlexLexer.h>

int lineno;
int colno = 1;
int matchLen;
int type;

std::string token;
std::string value;

yyFlexLexer scanner;
Node* tree = new Node();
SymbolTable* rootST = new SymbolTable(nullptr);
SymbolTable* currentST = rootST->addScope();

void printHelp() {
    std::cerr << "Usage: decaf [option]\n"
              << "Options:\n"
              << " -s            Print symbol table\n"
              << " -t            Print Abstract Syntax Tree (AST)\n"
              << " -p            Print LLVM IR code\n"
              << " -r FILENAME   Run the program\n";
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printHelp();
        return 1;
    }

    yyparse();
    std::string op = std::string(argv[1]);
    if (op == "-s") {
        rootST->children[0]->print(0);
    } else if (op == "-t") {
        tree->preOrderPrint(0);
    } else if (op == "-p" || op == "-r") {
        LLVMIRGenerator codeGenerator(tree, rootST);
        llvm::Module* module = codeGenerator.generateCode();

        if (std::string(argv[1]) == "-p") {
            module->print(llvm::outs(), nullptr);
            return 0;
        }
        if (argc < 3) {
            std::cerr << "No filename specified for -c option.\n";
            return 1;
        }

        std::string filename = std::string(argv[2]);
        std::error_code error;
        llvm::raw_fd_ostream outputFile(filename + ".ll", error);
        if (error) {
            std::cerr << "Error opening file: " << error.message() << "\n";
            return 1;
        }
        module->print(outputFile, nullptr);
        outputFile.close();

        // poor man's JIT; will fix later
        std::string llcCom = "llc -filetype=obj -relocation-model=pic " + filename + ".ll -o " + filename + ".o";
        std::string clangCom = "clang " + filename + ".o -o " + filename;
        std::string exCom = "./" + filename;
        std::system(llcCom.c_str());
        std::system(clangCom.c_str());
        std::system(exCom.c_str());
    } else {
        printHelp();
        return 1;
    }

    return 0;
}
