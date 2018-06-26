#include "textnode.h"

void TextNode::layout(QSize max_size)
{
    QFontMetrics fm {QFont{}};
    bounding_box = fm.boundingRect(QRect(QPoint{0,0}, max_size), Qt::AlignLeft|Qt::TextWordWrap, QString::fromStdString(text));
}

void TextNode::render(QPainter &painter)
{
    painter.drawText(bounding_box, Qt::AlignLeft|Qt::TextWordWrap, QString::fromStdString(text));
    painter.drawRect(bounding_box);
}
