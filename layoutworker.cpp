#include "layoutworker.h"
#include <iostream>

LayoutWorker::LayoutWorker()
{}

LayoutWorker::~LayoutWorker()
{
    if (network) network->deleteLater();
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

void LayoutWorker::gotoUrl(QString url)
{
    if (!network) {
        network = new QNetworkAccessManager();
        connect(network, &QNetworkAccessManager::finished, this, &LayoutWorker::networkFinished);
    }
    std::cout << "Getting url" << std::endl;
    network->get(QNetworkRequest(QUrl(url)));
}

void LayoutWorker::networkFinished(QNetworkReply *reply)
{
    reply->deleteLater();
    if (reply->error()) {
        std::cout << "Network error " << reply->errorString().toStdString() << std::endl;
        return;
    }

    html = reply->readAll();
    std::cout << "success" << std::endl;

    draw();
}

void LayoutWorker::draw()
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

    emit layoutComplete();
}
