#ifndef PAGERCIRCULARACTIVEINDICATOR_H
#define PAGERCIRCULARACTIVEINDICATOR_H

#include <QObject>
#include <QWidget>
#include "pagercircularindicator.h"
#include "pager.h"

class PagerCircularActiveIndicator : public QWidget
{
    Q_OBJECT
public:
    explicit PagerCircularActiveIndicator(QWidget *parent, PagerCircularIndicator * indicator);
    void paintEvent(QPaintEvent *event);

public slots:
    void toggleVisibility(const QString & text);

private:
    QPixmap pixmap;
    int radius;
    int spacing;
    int height;
    int width;
    Pager * pager;
    PagerCircularIndicator * indicator;
};

#endif // PAGERCIRCULARACTIVEINDICATOR_H
