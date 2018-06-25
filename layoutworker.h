#ifndef LAYOUWORKER_H
#define LAYOUWORKER_H

#include <QImage>
#include <QMutex>
#include <QPainter>
#include <QNetworkAccessManager>
#include <QNetworkReply>


class LayoutWorker: public QObject
{
    Q_OBJECT

private:
    QNetworkAccessManager *network = nullptr;

    QImage result;
    QMutex resultLock;
    QSize size;

    QString html;

    void draw();

public:
    LayoutWorker();
    virtual ~LayoutWorker();

    void resize(QSize size);
    void copyImage(QPainter &dst);

public slots:
    void gotoUrl(QString url);

    void networkFinished(QNetworkReply *reply);

signals:
    void layoutComplete();
};

#endif // LAYOUTTHREAD_H
