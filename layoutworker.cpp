#include "layoutworker.h"
#include <iostream>

namespace {

QNetworkRequest makeRequest(QUrl url) {
    QNetworkRequest req(url);
    req.setRawHeader("User-Agent", "ELinks/0.9.3 (textmode; Linux 2.6.9-kanotix-8 i686; 127x41)");
    return req;
}

}

LayoutWorker::LayoutWorker()
{}

LayoutWorker::~LayoutWorker()
{
    if (network) network->deleteLater();
}

void LayoutWorker::resize(QSize size)
{
    this->size = size;
    QMetaObject::invokeMethod(this, &LayoutWorker::redraw);
}

void LayoutWorker::copyImage(QPainter &dst)
{
    if (resultLock.tryLock(10)) {
        dst.drawImage(QPoint{0,0}, result);
        resultLock.unlock();
    }
}

void LayoutWorker::gotoUrl(QString url)
{
    if (!network) {
        network = new QNetworkAccessManager();
        connect(network, &QNetworkAccessManager::finished, this, &LayoutWorker::networkFinished);
    }
    network->get(makeRequest(QUrl(url)));
}

void LayoutWorker::networkFinished(QNetworkReply *reply)
{
    reply->deleteLater();
    if (reply->error()) {
        std::cout << "Network error " << reply->errorString().toStdString() << std::endl;
        return;
    }

    QUrl redirectTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    if(!redirectTarget.isEmpty()) {
        network->get(makeRequest(redirectTarget));
        return;
    }

    html = reply->readAll();
    redraw();
}

void LayoutWorker::redraw()
{
    QMutexLocker lock(&resultLock);

    if (result.size() != size) result = QImage(size, QImage::Format_ARGB32);
    result.fill(0);

    QBrush brush(Qt::black);
    QPen pen(Qt::blue, 1);
    QRect rect(0,0,0,0);

    QPainter painter(&result);

    painter.setPen(pen);
    painter.setBrush(brush);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawText(rect, Qt::AlignLeft, html, &rect);
    painter.drawText(rect, Qt::AlignLeft, html, &rect);

    emit pageRendered();
}
