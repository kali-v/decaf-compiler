#ifndef UTILS_HPP
#define UTILS_HPP

#include "SymbolTable.hpp"
#include "node.hpp"
#include <string>
#include <vector>

bool containsEmptyProd(Node* node);
bool startsWith(std::string str, std::string sub);
bool endsWith(std::string str, std::string sub);
bool hasStrLastChild(Node* node, std::string str);
bool strContains(std::string str, std::string sub);

void handleFuncParams(FunctionSTObject* sto, SymbolTable* symbolTable);

std::string extractOperation(std::string prod);
std::string getCurrentReturnType(SymbolTable* symbolTable);
std::string getLeftSideOfProd(std::string prod);
std::string getNameType(Node name, SymbolTable* symbolTable);
std::string getNewList(Node* newnode, std::string res);
std::string getRightSideOfProd(std::string prod);
std::string getType(Node* node, SymbolTable* symbolTable);
std::string parseThis(SymbolTable* symbolTable);
std::string parseType(Node* node);
std::string sanitizeProd(std::string prod);

std::vector<Node> getArgListParams(Node* arglist, std::vector<Node> res, SymbolTable* symbolTable);
std::vector<STObject> processParamList(Node node, std::vector<STObject> params);

FunctionSTObject* createFunctionSTObject(Node* param, std::string type, std::string name, std::string ret);

class NotImplemented : public std::logic_error {
  public:
    NotImplemented(std::string message = "Function not yet implemented") : std::logic_error(message){};
};

#endif