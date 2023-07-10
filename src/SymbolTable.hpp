#ifndef ST_HPP
#define ST_HPP

#include "llvm/IR/Value.h"
#include <stdio.h>
#include <string>
#include <vector>

class STObject {
  public:
    std::string name;
    std::string type;
    std::string flag = "";
    llvm::Value* value;

    STObject(std::string type, std::string name);

    virtual std::string str();
    virtual void print();
};

class FunctionSTObject : public STObject {
  public:
    std::vector<STObject> params;
    std::string desc;
    std::string ret;

    FunctionSTObject(std::string type, std::string name, std::string ret, std::vector<STObject> params);

    virtual std::string str();
    virtual void print();
};

class SymbolTable {
  public:
    std::vector<STObject*> objects;
    std::vector<SymbolTable*> children;
    SymbolTable* parent;

    SymbolTable(SymbolTable* parent);

    SymbolTable* getSuperParent();
    STObject* lookup(std::string name);
    STObject* locLookup(std::string name);
    void insert(STObject* object);
    SymbolTable* addScope();

    void print(int level);
};

#endif