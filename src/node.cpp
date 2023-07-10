#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

#include "SymbolTable.hpp"
#include "node.hpp"

Node::Node() : production(""), symbolTable(nullptr) {}

Node::Node(SymbolTable* symbolTable) : production(""), symbolTable(symbolTable) {}

void Node::addChild(Node* child) {
    this->children.push_back(*child);
    child->parent = this;
}

void Node::addChildren(std::vector<Node> children) {
    for (Node child : children) {
        this->addChild(&child);
    }
}

std::vector<Node> Node::getChildren() { return this->children; }

void Node::resetChildren() { this->children = {}; }

void Node::setsvalue(std::string value) { this->svalue = value; }

void Node::setivalue(int value) { this->ivalue = value; }

int Node::getivalue() { return this->ivalue; }

std::string Node::getsvalue() { return this->svalue; }

void Node::preOrderPrint(int level) {
    if (this->production != "" && this->shouldPrint == true) {
        for (int i = 0; i < level; i++)
            std::cout << "  ";
        std::cout << this->production << std::endl;
        level++;
    }
    for (Node node : this->children) {
        node.preOrderPrint(level);
    }
}
