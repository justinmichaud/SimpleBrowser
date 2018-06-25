#include "layoutworker.h"
#include <iostream>
#include <cassert>

namespace {

QNetworkRequest makeRequest(QUrl url) {
    QNetworkRequest req(url);
    req.setRawHeader("User-Agent", "ELinks/0.9.3 (textmode; Linux 2.6.9-kanotix-8 i686; 127x41)");
    return req;
}

std::vector<DomNode> buildTree(myhtml_tree_t* tree, myhtml_tree_node_t *node) {
    std::vector<DomNode> children;
    while (node) {
        DomNode n;
        const char *tag_name = myhtml_tag_name_by_id(tree, myhtml_node_tag_id(node), NULL);
        assert(tag_name);
        n.tag = std::string{tag_name};

        const char* node_text = myhtml_node_text(node, NULL);
        if (node_text) {
            n.text = std::string{node_text};
        }

        n.children = buildTree(tree, myhtml_node_child(node));
        node = myhtml_node_next(node);
        children.push_back(n);
    }
    return children;
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

    html = reply->readAll().toStdString();
    parseHtml();
    redraw();
}

void LayoutWorker::parseHtml()
{
    myhtml_t* myhtml = myhtml_create();
    myhtml_init(myhtml, MyHTML_OPTIONS_DEFAULT, 1, 0);

    // first tree init
    myhtml_tree_t* tree = myhtml_tree_create();
    myhtml_tree_init(tree, myhtml);

    // parse html
    myhtml_parse(tree, MyENCODING_UTF_8, html.c_str(), strlen(html.c_str()));

    myhtml_tree_node_t *node = myhtml_tree_get_document(tree);
    root.text = std::string{};
    root.tag = "html";
    root.children = buildTree(tree, myhtml_node_child(node));
    root.printTree();

    // release resources
    myhtml_tree_destroy(tree);
    myhtml_destroy(myhtml);
}

void LayoutWorker::redraw()
{
    QMutexLocker lock(&resultLock);

    if (result.size() != size) result = QImage(size, QImage::Format_ARGB32);
    result.fill(0);

    QBrush brush(Qt::black);
    QPen pen(Qt::blue, 1);

    QPainter painter(&result);

    painter.setPen(pen);
    painter.setBrush(brush);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QRect rect(0,0,0,0);
    painter.drawText(rect, Qt::AlignLeft, QString::fromStdString(html), &rect);
    painter.drawText(rect, Qt::AlignLeft, QString::fromStdString(html), &rect);

    emit pageRendered();
}
