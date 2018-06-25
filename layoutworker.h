#ifndef LAYOUWORKER_H
#define LAYOUWORKER_H

#include <memory>
#include <QImage>
#include <QMutex>
#include <QPainter>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <myhtml/api.h>

#include "domnode.h"

class LayoutWorker: public QObject
{
    Q_OBJECT

private:
    QNetworkAccessManager *network = nullptr;

    QImage result;
    QMutex resultLock;
    QSize size;

    std::string html;
    std::unique_ptr<DomNode> root {nullptr};

    void parseHtml();

public:
    LayoutWorker();
    virtual ~LayoutWorker();

    // These can be called from either thread
    void resize(QSize size);
    void copyImage(QPainter &dst);

public slots:
    // Must be called on owning thread to avoid blocking
    void gotoUrl(QString url);
    void redraw();

private slots:
    void networkFinished(QNetworkReply *reply);

signals:
    void pageRendered();
};

#endif // LAYOUTTHREAD_H
