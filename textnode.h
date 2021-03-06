#ifndef TEXTNODE_H
#define TEXTNODE_H

#include "domnode.h"



class TextNode : public DomNode
{
public:
    std::string text;
    int size;
    bool link;

    TextNode(std::string tag, std::string text, int size = 4, bool link = false): DomNode(tag), text{text}, size{size}, link{link} {}

    void layout(QSize max_size);
    void render(QPainter &painter);

    void printTree(size_t depth = 0) override {
        for (size_t i=0; i<depth; ++i) std::cout << " ";
        std::cout << "<" << tag << ">" << text << "</" << tag << ">" << std::endl;
    }
};

#endif // TEXTNODE_H
