#include "compositorwidget.h"

#include <QPainter>
#include <QPen>

CompositorWidget::CompositorWidget(QWidget *parent) : QWidget(parent)
{
    layout_worker = new LayoutWorker();
    layout_worker->moveToThread(&worker_thread);
    connect(&worker_thread, &QThread::finished, layout_worker, &QObject::deleteLater);
    connect(layout_worker, &LayoutWorker::pageRendered, this, static_cast<void (CompositorWidget::*)()>(&CompositorWidget::update));
    connect(this, &CompositorWidget::urlChanged, layout_worker, &LayoutWorker::gotoUrl);
    worker_thread.start();
}

CompositorWidget::~CompositorWidget()
{
    worker_thread.quit();
    worker_thread.wait();
}

void CompositorWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    layout_worker->copyImage(painter);
}

void CompositorWidget::resizeEvent(QResizeEvent *)
{
    layout_worker->resize(size());
}

void CompositorWidget::wheelEvent(QWheelEvent *e)
{
    layout_worker->scrollTo(e->delta()/2);
}
