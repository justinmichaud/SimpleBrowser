#include "blocknode.h"
#include "layoutworker.h"
#include "textnode.h"
#include <cassert>

namespace {

QNetworkRequest makeRequest(QUrl url) {
    QNetworkRequest req(url);
    req.setRawHeader("User-Agent", "ELinks/0.9.3 (textmode; Linux 2.6.9-kanotix-8 i686; 127x41)");
    return req;
}

std::vector<std::unique_ptr<DomNode>> buildTree(myhtml_tree_t* tree, myhtml_tree_node_t *node, int size = 8, bool link = false, bool forceInline = false) {
    std::vector<std::unique_ptr<DomNode>> children;
    while (node) {
        if (myhtml_node_tag_id(node) == MyHTML_TAG_SCRIPT ||myhtml_node_tag_id(node) == MyHTML_TAG_HEAD ||myhtml_node_tag_id(node) == MyHTML_TAG_COMMENT) {
            node = myhtml_node_next(node);
            continue;
        }
        const char *tag_name = myhtml_tag_name_by_id(tree, myhtml_node_tag_id(node), NULL);
        assert(tag_name);

        if (myhtml_node_tag_id(node) == MyHTML_TAG__TEXT) {
            const char* node_text = myhtml_node_text(node, NULL);
            assert(node_text && strlen(node_text) > 0);

            std::string text_str = QString(node_text).trimmed().toStdString();
            if (!text_str.empty())
                children.emplace_back(new TextNode(std::string{tag_name}, text_str, size, link));

            assert(!myhtml_node_child(node));
        } else if (myhtml_node_tag_id(node) == MyHTML_TAG_P || myhtml_node_tag_id(node) == MyHTML_TAG_A || myhtml_node_tag_id(node) == MyHTML_TAG_H1 ||
            myhtml_node_tag_id(node) == MyHTML_TAG_H2 || myhtml_node_tag_id(node) == MyHTML_TAG_H3 || myhtml_node_tag_id(node) == MyHTML_TAG_H4) {

            int size = 0;
            bool link = false;
            if (myhtml_node_tag_id(node) == MyHTML_TAG_P) size = 12;
            else if (myhtml_node_tag_id(node) == MyHTML_TAG_A) { size = 12; link = true; }
            else if (myhtml_node_tag_id(node) == MyHTML_TAG_H1) size = 48;
            else if (myhtml_node_tag_id(node) == MyHTML_TAG_H2) size = 36;
            else if (myhtml_node_tag_id(node) == MyHTML_TAG_H3) size = 28;
            else if (myhtml_node_tag_id(node) == MyHTML_TAG_H4) size = 24;
            else assert(false);

            children.emplace_back(new BlockNode(std::string{tag_name}, buildTree(tree, myhtml_node_child(node), size, link, true), true));

        } else {
            children.emplace_back(new BlockNode(std::string{tag_name}, buildTree(tree, myhtml_node_child(node), size, link, forceInline), forceInline));
        }

        node = myhtml_node_next(node);
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
    QMetaObject::invokeMethod(this, &LayoutWorker::layout);
    QMetaObject::invokeMethod(this, &LayoutWorker::redraw);
}

void LayoutWorker::scrollTo(int dy)
{
    scroll_pos -= dy;
    if (scroll_pos < 0) scroll_pos = 0;
    if (scroll_pos > root->bounding_box.height()) scroll_pos = root->bounding_box.height();
    QMetaObject::invokeMethod(this, &LayoutWorker::redraw);
}

void LayoutWorker::copyImage(QPainter &dst)
{
    if (resultLock.tryLock(100)) {
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
    layout();
    redraw();
}

void LayoutWorker::parseHtml()
{
    QMutexLocker lock(&resultLock);
    myhtml_t* myhtml = myhtml_create();
    myhtml_init(myhtml, MyHTML_OPTIONS_DEFAULT, 1, 0);

    // first tree init
    myhtml_tree_t* tree = myhtml_tree_create();
    myhtml_tree_init(tree, myhtml);

    // parse html
    myhtml_parse(tree, MyENCODING_UTF_8, html.c_str(), strlen(html.c_str()));

    myhtml_tree_node_t *node = myhtml_tree_get_document(tree);
    root.reset(new BlockNode("html", buildTree(tree, myhtml_node_child(node)), false));

    root->printTree();

    // release resources
    myhtml_tree_destroy(tree);
    myhtml_destroy(myhtml);
}

void LayoutWorker::layout()
{
    QMutexLocker lock(&resultLock);
    if (result.size() != size) result = QImage(size, QImage::Format_ARGB32);
    if (root) root->layout(QSize(result.width(), INT_MAX));
}

void LayoutWorker::redraw()
{
    QMutexLocker lock(&resultLock);

    result.fill(0);

    QBrush brush(Qt::transparent);
    QPen pen(Qt::green, 1);

    QPainter painter(&result);
    painter.translate(0, -scroll_pos);

    painter.setPen(pen);
    painter.setBrush(brush);
    painter.setRenderHint(QPainter::Antialiasing, true);

    if (root) root->render(painter);

    emit pageRendered();
}
