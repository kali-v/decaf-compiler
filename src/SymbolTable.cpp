#include "SymbolTable.hpp"
#include "node.hpp"
#include <iostream>

STObject::STObject(std::string type, std::string name) {
    this->name = name;
    this->type = type;
};

void STObject::print() { std::cout << this->str() << std::endl; }

std::string STObject::str() {
    std::string flg = this->flag.empty() ? "" : " " + this->flag;
    return this->name + ": " + this->type + flg;
}

FunctionSTObject::FunctionSTObject(std::string type, std::string name, std::string ret, std::vector<STObject> params)
    : STObject(type, name) {
    this->ret = ret;
    this->params = params;
}

void FunctionSTObject::print() { std::cout << this->str() << std::endl; }

std::string FunctionSTObject::str() {
    std::string pstr = "";
    for (STObject param : params) {
        pstr += param.str() + ", ";
    }
    if (pstr.size() > 0) {
        // remove ", " at the end of param string
        pstr.pop_back();
        pstr.pop_back();
    }

    return this->name + ": " + this->type + " (" + pstr + ") -> " + this->ret;
}

SymbolTable::SymbolTable(SymbolTable* parent) {
    this->objects = std::vector<STObject*>();
    this->children = std::vector<SymbolTable*>();
    this->parent = parent;
}

STObject* SymbolTable::lookup(std::string name) {
    STObject* obj = locLookup(name);
    return (obj == nullptr && this->parent != nullptr) ? this->parent->lookup(name) : obj;
}

STObject* SymbolTable::locLookup(std::string name) {
    for (STObject* obj : objects) {
        if (obj->name == name) return obj;
    }
    return nullptr;
}

SymbolTable* SymbolTable::getSuperParent() {
    SymbolTable* parent = this;
    while (parent->parent) {
        parent = parent->parent;
    }
    return parent;
}

void SymbolTable::insert(STObject* object) { this->objects.push_back(object); };

SymbolTable* SymbolTable::addScope() {
    SymbolTable* st = new SymbolTable(this);
    this->children.push_back(st);
    return st;
}

void SymbolTable::print(int level) {
    for (int i = 0; i < this->objects.size(); i++) {
        for (int l = 0; l < level; l++) {
            std::cout << "  ";
        }

        STObject* object = nullptr;
        if (level == 0) {
            this->objects[this->objects.size() - 1 - i]->print();
            object = this->objects[this->objects.size() - 1 - i];
        } else {
            this->objects[i]->print();
            object = this->objects[i];
        }

        if (std::string(object->type).find("_type") != std::string::npos) {
            this->children.front()->print(level + 1);
            this->children.erase(this->children.begin());
        }
    }
}
