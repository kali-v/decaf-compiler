#include "utils.hpp"
#include <iostream>

std::string sanitizeProd(std::string prod) {
    std::string res = "";
    bool copy = false;

    for (char c : prod) {
        if (c == '<') {
            copy = false;
            continue;
        } else if (c == '>') {
            copy = true;
            continue;
        } else if (copy && c != ' ' && c != '-') {
            res += c;
        }
    }

    return res;
}

std::string parseType(Node* node) {
    std::string res = "";
    for (Node child : node->children) {
        res += parseType(&child);
        res += sanitizeProd(child.production);
    }

    return res;
}

std::vector<STObject> processParamList(Node node, std::vector<STObject> params) {
    if (node.children.size() == 0) {
        return params;
    }

    std::string type = parseType(&node.children[0]);
    std::string id = sanitizeProd(node.children[1].production);
    STObject sto = STObject(type, id);

    if (node.children.size() > 2) {
        params = processParamList(node.children[2], params);
    }
    params.push_back(sto);

    return params;
}

bool hasStrLastChild(Node* node, std::string str) {
    // return true if node contains str production
    Node curr = *node;
    while (true) {
        for (Node child : curr.children) {
            if (std::string(child.production).find(str) != std::string::npos) {
                return true;
            }
        }
        if (curr.children.size() > 0) {
            // because str must be always at the end of production we
            // can explore only the rightmost branch
            Node tmp = curr.children[curr.children.size() - 1];
            curr = tmp;
        } else {
            return false;
        }
    }
}

bool containsEmptyProd(Node* node) {
    if (node->children.size() == 1) {
        if (node->children[0].production == "empty") return true;
    }
    return false;
}

std::string getRightSideOfProd(std::string prod) {
    std::string c = "";
    bool copy = false;
    for (int i = 1; i < prod.size(); i++) {
        if (copy && (c.size() > 0 || prod[i] != ' ')) c += prod[i];
        if (prod[i - 1] == '-' && prod[i] == '>') copy = true;
    }
    return c;
}

std::string getLeftSideOfProd(std::string prod) {
    std::string c = "";
    for (int i = 0; i < prod.size() - 1; i++) {
        if (prod[i] == ' ' && prod[i + 1] == '-') return c;
        c += prod[i];
    }
    return c;
}

bool endsWith(std::string str, std::string sub) {
    for (int i = str.size() - 1; sub.size() > 0; i--) {
        char subc = sub[sub.size() - 1];
        sub.pop_back();
        if (str[i] != subc) return false;
    }
    return true;
}

bool startsWith(std::string str, std::string sub) {
    for (int i = 0; i < sub.size(); i++) {
        if (str[i] != sub[i]) return false;
    }
    return true;
}

bool strContains(std::string str, std::string sub) { return (str.find(sub) != std::string::npos); }

std::string getNewList(Node* newnode, std::string res) {
    for (Node child : newnode->children) {
        if (child.production.size() > 0) {
            std::string lprod = getLeftSideOfProd(child.production);
            if (strContains(lprod, "newexps")) res += "[]";
            else if (strContains(lprod, "<newbracks>")) res += "[]";
        }
        res = getNewList(&child, res);
    }

    return res;
}

std::string getType(Node* node, SymbolTable* symbolTable) {
    std::string prod = getRightSideOfProd(node->production);

    if (strContains(prod, "<name>")) {
        if (strContains(prod, ".")) return "int"; // TODO: get last
        else return getNameType(node->children[0], symbolTable);
    } else if (strContains(node->production, "ID")) {
        return getNameType(*node, symbolTable);
    } else if (endsWith(prod, "<exp>") || strContains(prod, "read") || strContains(node->production, "NUMBER") ||
               prod == "int") {
        return "int";
    } else if (prod == "<newexp>") {
        std::string ntype = getType(&node->children[0].children[0], symbolTable);
        std::string arglist = "";
        return ntype + getNewList(node, arglist);
    }

    return prod;
}

std::string getNameType(Node name, SymbolTable* symbolTable) {
    std::string id = name.production;
    if (endsWith(id, "ID")) id = name.children[name.children.size() - 1].production;
    id = getRightSideOfProd(id);
    if (id == "this") return parseThis(symbolTable);

    STObject* obj = symbolTable->lookup(id);
    return obj->type == "<func_type>" ? ((FunctionSTObject*)obj)->ret : obj->type;
}

std::string parseThis(SymbolTable* symbolTable) {
    SymbolTable* tb = symbolTable->parent;
    while (tb != nullptr) {
        std::vector<STObject*> objs = tb->objects;
        if (objs[objs.size() - 1]->type == "<class_type>") return objs[objs.size() - 1]->name;
        tb = tb->parent;
    }
    return "this";
}

void handleFuncParams(FunctionSTObject* sto, SymbolTable* symbolTable) {
    if (sto->params.size() == 0) return;
    std::vector<STObject*> pobjects = symbolTable->objects;
    int it = symbolTable->children.size() - 1;
    for (int i = 0; i < pobjects.size(); i++) {
        if (std::string(pobjects[i]->type).find("type") != std::string::npos) {
            if (sto->str() == pobjects[i]->str()) break;
            it--;
        }
    }

    SymbolTable* fscope = symbolTable->children[it];
    for (int i = sto->params.size() - 1; i >= 0; i--) {
        STObject* psto = new STObject(sto->params[i]);
        psto->flag = "<param>";
        fscope->insert(psto);
    }
}

FunctionSTObject* createFunctionSTObject(Node* param, std::string type, std::string name, std::string ret) {
    std::vector<STObject> params = {};
    if (param->children.size() > 0) {
        params = processParamList(param->children[0], {});
        params.insert(params.begin(), params.back());
        params.pop_back();
    }

    return new FunctionSTObject(type, name, ret, params);
}

std::string getCurrentReturnType(SymbolTable* symbolTable) {
    SymbolTable* parent = symbolTable->parent;
    while (parent != nullptr) {
        for (int i = parent->objects.size() - 1; i >= 0; i--) {
            if (parent->objects[i]->type == "<const_type>") return "<const_type>";
            if (parent->objects[i]->type == "<func_type>") return ((FunctionSTObject*)parent->objects[i])->ret;
        }
        parent = parent->parent;
    }

    return "";
}

std::vector<Node> getArgListParams(Node* arglist, std::vector<Node> res, SymbolTable* symbolTable) {
    for (Node child : arglist->children) {
        res = getArgListParams(&child, res, symbolTable);
        if (child.production.size() > 0) res.push_back(child);
    }

    return res;
}

std::string extractOperation(std::string prod) {
    std::string op = "";
    bool is_op = false;
    for (char c : prod) {
        if (c == ' ') {
            if (is_op) return op;
            is_op = true;
        } else if (is_op) op += c;
    }
    return op;
}