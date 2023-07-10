#ifndef LLVMIRGENERATOR_HPP
#define LLVMIRGENERATOR_HPP

#include "SymbolTable.hpp"
#include "node.hpp"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

class LLVMIRGenerator {
  private:
    bool createFuncRet = false;
    llvm::LLVMContext context;
    llvm::IRBuilder<> builder;
    llvm::Module* module;
    llvm::Constant* intFormater;
    llvm::Type* intType;
    Node* astRoot;
    SymbolTable* symbolTable;

    void generateCodeForNode(Node& node);
    void genVarDec(Node& node);
    void genFuncDec(Node& node);
    void genStatement(Node& node);

    llvm::Value* genFunc(std::string prod);
    llvm::Value* genExp(Node& exp);

    void getTypeForNode(Node& node);

  public:
    LLVMIRGenerator(Node* astRoot, SymbolTable* symbolTable);

    llvm::Module* generateCode();
};

#endif
