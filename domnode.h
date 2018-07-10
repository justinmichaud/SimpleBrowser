#ifndef DOMNODE_H
#define DOMNODE_H

#include <QPainter>
#include <iostream>

class DomNode
{
public:
    DomNode(std::string tag): tag{tag} {}
    virtual ~DomNode() {}

    std::string tag;
    QRect bounding_box; // Local coordinaes

    virtual void layout(QSize max_size) = 0;
    virtual void render(QPainter &painter) = 0;

    virtual void printTree(size_t depth = 0) = 0;
};

#endif // DOMNODE_H
