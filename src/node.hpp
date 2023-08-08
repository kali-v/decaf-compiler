#ifndef NODE_HPP
#define NODE_HPP

#include "SymbolTable.hpp"

#include <stdio.h>
#include <string>
#include <vector>

class Node {
  public:
    std::string production;
    std::string svalue;
    int ivalue;
    bool shouldPrint = true;

    Node* parent;
    SymbolTable* symbolTable;

    std::vector<Node> children = {};

    Node();
    Node(SymbolTable* symbolTable);

    void addChild(Node* children);
    void addChildren(std::vector<Node> children);
    std::vector<Node> getChildren();
    void resetChildren();

    void setsvalue(std::string value);
    std::string getsvalue();
    void setivalue(int value);
    int getivalue();

    void preOrderPrint(int level);
};

#endif