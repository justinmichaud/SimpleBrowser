#ifndef COMPOSITORWIDGET_H
#define COMPOSITORWIDGET_H

#include "layoutworker.h"

#include <QThread>
#include <QWheelEvent>
#include <QWidget>

class CompositorWidget : public QWidget
{
    Q_OBJECT

    QString text = "Hello World!";

    LayoutWorker *layout_worker;
    QThread worker_thread;

public:
    explicit CompositorWidget(QWidget *parent = nullptr);
    virtual ~CompositorWidget();

signals:
    void urlChanged(QString url);

protected:
    void paintEvent(QPaintEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
    void wheelEvent(QWheelEvent *e) override;
};

#endif // COMPOSITORWIDGET_H
