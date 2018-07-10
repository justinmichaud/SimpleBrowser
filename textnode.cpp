#include "textnode.h"

void TextNode::layout(QSize max_size)
{
    QFontMetrics fm {QFont{"Helvetica", size}};
    bounding_box = fm.boundingRect(QRect(QPoint{0,0}, max_size), Qt::AlignLeft|Qt::TextWordWrap, QString::fromStdString(text));
}

void TextNode::render(QPainter &painter)
{
    painter.save();
    if (link) painter.setPen(QPen(Qt::blue));
    else painter.setPen(QPen(Qt::black));
    painter.setFont(QFont{"Helvetica", size});
    painter.drawText(bounding_box, Qt::AlignLeft|Qt::TextWordWrap, QString::fromStdString(text));
    painter.drawRect(bounding_box);
    painter.restore();
}
