#ifndef COMPOSITORWIDGET_H
#define COMPOSITORWIDGET_H

#include <QWidget>

class CompositorWidget : public QWidget
{
    Q_OBJECT

    QString text = "Hello World!";

public:
    explicit CompositorWidget(QWidget *parent = nullptr);
    void render(QString url);

signals:

public slots:

protected:
    void paintEvent(QPaintEvent *e) override;
};

#endif // COMPOSITORWIDGET_H
