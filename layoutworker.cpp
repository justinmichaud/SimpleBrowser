#include "layoutworker.h"

LayoutWorker::LayoutWorker()
{
}

void LayoutWorker::resize(QSize size)
{
    this->size = size;
}

void LayoutWorker::copyImage(QPainter &dst)
{
    QMutexLocker lock(&resultLock);
    dst.drawImage(QPoint{0,0}, result);
}

void LayoutWorker::layout(QString text)
{
    QMutexLocker lock(&resultLock);

    if (result.size() != size) result = QImage(size, QImage::Format_ARGB32);
    result.fill(0);

    QBrush brush(Qt::black);
    QPen pen(Qt::blue, 1);
    QRect rect(0,0,0,0);

    QPainter painter(&result);

    for (size_t i=0; i<1000000000; ++i) {}

    painter.setPen(pen);
    painter.setBrush(brush);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawText(rect, Qt::AlignLeft, text, &rect);
    painter.drawText(rect, Qt::AlignLeft, text, &rect);

    emit layoutComplete();
}
