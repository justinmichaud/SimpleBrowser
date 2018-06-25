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
};

#endif // BLOCKNODE_H
