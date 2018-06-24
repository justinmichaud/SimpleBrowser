#include "compositorwidget.h"

#include <QPainter>
#include <QPen>

CompositorWidget::CompositorWidget(QWidget *parent) : QWidget(parent)
{

}

void CompositorWidget::render(QString url)
{
    text = url;
    repaint();
}

void CompositorWidget::paintEvent(QPaintEvent *)
{
    QBrush brush(Qt::black);
    QPen pen(Qt::blue, 1);
    QRect rect(0,0,0,0);

    QPainter painter(this);

    painter.setPen(pen);
    painter.setBrush(brush);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawText(rect, Qt::AlignLeft, text, &rect);
    painter.drawText(rect, Qt::AlignLeft, text, &rect);
}
