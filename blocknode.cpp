#include "blocknode.h"

void BlockNode::layout(QSize max_size)
{
    for (auto &c : children) c->layout(QSize(max_size.width(), max_size.height()));
    bounding_box = QRect(0,0,0,0);
    for (auto &c : children) {
        bounding_box.setWidth(std::max(bounding_box.width(), c->bounding_box.width()));
        bounding_box.setHeight(bounding_box.height() + c->bounding_box.height());
    }
}

void BlockNode::render(QPainter &painter)
{
    painter.save();
    for (auto &c : children) {
        painter.save();
        painter.setClipRect(c->bounding_box);
        painter.translate(c->bounding_box.x(), c->bounding_box.y());
        c->render(painter);
        painter.restore();
        painter.translate(0, c->bounding_box.height());
    }
    painter.restore();
}
