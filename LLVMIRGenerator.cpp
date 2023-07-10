#include "LLVMIRGenerator.hpp"

#include "llvm/IR/Function.h"
#include <llvm/IR/Constants.h>

#include "utils.hpp"
#include <functional>
#include <iostream>
#include <map>
#include <ostream>
#include <unordered_map>

LLVMIRGenerator::LLVMIRGenerator(Node* astRoot, SymbolTable* symbolTable)
    : builder(context), module(new llvm::Module("module", context)),
      intFormater(builder.CreateGlobalStringPtr("%d\n", "d_format", 0U, module)),
      intType(llvm::Type::getInt32Ty(context)), astRoot(astRoot), symbolTable(symbolTable) {}

llvm::Module* LLVMIRGenerator::generateCode() {
    generateCodeForNode(*astRoot);
    builder.CreateRetVoid();
    return module;
}

void LLVMIRGenerator::generateCodeForNode(Node& node) {
    if (node.production.length() > 0) {
        std::string type = getLeftSideOfProd(node.production);

        if (type == "<methdec>") {
            genFuncDec(node);
        } else if (type == "<statement>") {
            genStatement(node);
        } else if (type == "<vardec>" || type == "<localvardec>") {
            genVarDec(node);
        } else {
            ;
        }
    }

    for (Node& child : node.children) {
        generateCodeForNode(child);
    }
}

void LLVMIRGenerator::genStatement(Node& node) {
    std::string rprod = getRightSideOfProd(node.production);
    std::string lprod = getLeftSideOfProd(node.production);

    if (strContains(node.production, "print")) {
        std::vector<llvm::Type*> printArgs = {builder.getInt8Ty()->getPointerTo()};
        llvm::FunctionType* printType = llvm::FunctionType::get(builder.getVoidTy(), printArgs, false);
        llvm::FunctionCallee printFunc = module->getOrInsertFunction("printf", printType);
        std::vector<llvm::Value*> args;
        args.push_back(intFormater);
        args.push_back(genExp(node.children[0].children[0]));
        builder.CreateCall(printFunc, args);
    } else if (rprod == "<name> = <exp>") {
        std::string nid = getRightSideOfProd(node.children[0].children[0].production);
        llvm::Value* rvalue = genExp(node.children[1]);
        STObject* lobject = node.symbolTable->lookup(nid);
        if (lobject->flag == "<param>") {
            throw NotImplemented("code gen for func_args not implemented yet");
        }
        if (rvalue) builder.CreateStore(rvalue, lobject->value);
    } else if (startsWith(node.production, "<statement> --> <name> (")) {
        genFunc(getRightSideOfProd(node.children[0].children[0].production));
    } else if (startsWith(rprod, "return")) {
        builder.CreateRet(genExp(node.children[0]));
        createFuncRet = 0;
    } else {
        throw NotImplemented("functionality" + node.production + "not implemented yet");
    }
}

llvm::Value* LLVMIRGenerator::genFunc(std::string prod) {
    llvm::Function* func = module->getFunction(prod);
    builder.SetInsertPoint(builder.GetInsertBlock(), builder.GetInsertPoint());
    return builder.CreateCall(func);
}

llvm::Value* LLVMIRGenerator::genExp(Node& exp) {
    if (getRightSideOfProd(exp.production) == "NUMBER") {
        int irvalue = stoi(getRightSideOfProd(exp.children[0].production));
        return (llvm::Value*)llvm::ConstantInt::get(intType, irvalue);
    } else if (getRightSideOfProd(exp.production) == "<name>") {
        llvm::Value* rvalue =
            exp.symbolTable->lookup(getRightSideOfProd(exp.children[0].children[0].production))->value;
        return builder.CreateLoad(intType, rvalue);
    } else if (startsWith(getRightSideOfProd(exp.production), "<name> (")) {
        return genFunc(getRightSideOfProd(exp.children[0].children[0].production));
    } else {
        std::unordered_map<std::string, std::function<llvm::Value*(llvm::Value*, llvm::Value*)>> opMap = {
            {"+", [this](llvm::Value* lhs, llvm::Value* rhs) { return builder.CreateAdd(lhs, rhs); }},
            {"-", [this](llvm::Value* lhs, llvm::Value* rhs) { return builder.CreateSub(lhs, rhs); }},
            {"*", [this](llvm::Value* lhs, llvm::Value* rhs) { return builder.CreateMul(lhs, rhs); }},
            {"/", [this](llvm::Value* lhs, llvm::Value* rhs) { return builder.CreateFDiv(lhs, rhs); }},
            {"==", [this](llvm::Value* lhs, llvm::Value* rhs) { return builder.CreateICmpEQ(lhs, rhs); }},
            {"!=", [this](llvm::Value* lhs, llvm::Value* rhs) { return builder.CreateICmpNE(lhs, rhs); }},
            {"<=", [this](llvm::Value* lhs, llvm::Value* rhs) { return builder.CreateICmpSLE(lhs, rhs); }},
            {">=", [this](llvm::Value* lhs, llvm::Value* rhs) { return builder.CreateICmpSGE(lhs, rhs); }},
            {"<", [this](llvm::Value* lhs, llvm::Value* rhs) { return builder.CreateICmpSLT(lhs, rhs); }},
            {">", [this](llvm::Value* lhs, llvm::Value* rhs) { return builder.CreateICmpSGT(lhs, rhs); }},
            {"||", [this](llvm::Value* lhs, llvm::Value* rhs) { return builder.CreateOr(lhs, rhs); }},
            {"%", [this](llvm::Value* lhs, llvm::Value* rhs) { return builder.CreateSRem(lhs, rhs); }},
            {"&&", [this](llvm::Value* lhs, llvm::Value* rhs) { return builder.CreateAnd(lhs, rhs); }},
        };

        auto it = opMap.find(extractOperation(getRightSideOfProd(exp.production)));
        if (it != opMap.end())
            return it->second(genExp(exp.children[0]), genExp(exp.children[1]));
        else
            throw NotImplemented("functionality for: " + exp.production + " not implemented yet");
    }

    return nullptr;
}

void LLVMIRGenerator::genVarDec(Node& node) {
    int pos = startsWith(getRightSideOfProd(node.production), "INT") ? 0 : 1;
    std::string name = getRightSideOfProd(node.children[pos].production);
    STObject* object = node.symbolTable->lookup(name);
    llvm::Constant* init = llvm::ConstantInt::get(intType, 0);
    llvm::GlobalVariable* gvar =
        new llvm::GlobalVariable(*module, intType, false, llvm::GlobalValue::ExternalLinkage, nullptr, name);
    gvar->setInitializer(init);
    object->value = gvar;
}

void LLVMIRGenerator::genFuncDec(Node& node) {
    if (createFuncRet) builder.CreateRetVoid();

    int pos = startsWith(getRightSideOfProd(node.production), "<type>") ? 1 : 0;
    std::string name = getRightSideOfProd(node.children[pos].production);

    FunctionSTObject* fobj = (FunctionSTObject*)node.symbolTable->lookup(name);
    llvm::Type* fType = fobj->ret == "int" ? llvm::Type::getInt32Ty(context) : llvm::Type::getVoidTy(context);
    llvm::Function* func =
        llvm::Function::Create(llvm::FunctionType::get(fType, false), llvm::Function::ExternalLinkage, name, module);
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(context, name, func);
    builder.SetInsertPoint(entry);
    createFuncRet = 1;
}
