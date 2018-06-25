#ifndef TEXTNODE_H
#define TEXTNODE_H

#include "domnode.h"



class TextNode : public DomNode
{
public:
    std::string text;

    TextNode(std::string tag, std::string text): DomNode(tag), text{text} { this->text.insert(0, tag); }

    void layout(QSize max_size);
    void render(QPainter &painter);
};

#endif // TEXTNODE_H
