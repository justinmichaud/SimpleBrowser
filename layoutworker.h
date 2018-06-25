#ifndef LAYOUWORKER_H
#define LAYOUWORKER_H

#include <QImage>
#include <QMutex>
#include <QPainter>



class LayoutWorker: public QObject
{
    Q_OBJECT

private:
    QImage result;
    QMutex resultLock;
    QSize size;

public:
    LayoutWorker();

    void resize(QSize size);
    void copyImage(QPainter &dst);

public slots:
    void layout(QString text);

signals:
    void layoutComplete();
};

#endif // LAYOUTTHREAD_H
