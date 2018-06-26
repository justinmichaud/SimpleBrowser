#ifndef BLOCKNODE_H
#define BLOCKNODE_H

#include "domnode.h"
#include <memory>
#include <vector>



class BlockNode : public DomNode
{
public:
    BlockNode(std::string tag, std::vector<std::unique_ptr<DomNode>> children): DomNode(tag), children{std::move(children)} {}

    std::vector<std::unique_ptr<DomNode>> children;

    void layout(QSize max_size);
    void render(QPainter &painter);

    void printTree(size_t depth = 0) override {
        for (size_t i=0; i<depth; ++i) std::cout << " ";
        std::cout << "<" << tag << ">" << std::endl;
        for (auto &n : children) n->printTree(depth+1);
        for (size_t i=0; i<depth; ++i) std::cout << " ";
        std::cout << "</" << tag << ">" << std::endl;
    }
};

#endif // BLOCKNODE_H
