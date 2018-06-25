#ifndef DOMNODE_H
#define DOMNODE_H

#include <iostream>

class DomNode
{
public:
    DomNode() {}

    std::string tag;
    std::string text;
    std::vector<DomNode> children;

    void printTree(size_t depth = 0) {
        for (size_t i=0; i<depth; ++i) std::cout << " ";
        std::cout << "<" << tag << ">" << text << std::endl;
        for (auto &n : children) n.printTree(depth+1);
        for (size_t i=0; i<depth; ++i) std::cout << " ";
        std::cout << "</" << tag << ">" << std::endl;
    }
};

#endif // DOMNODE_H
