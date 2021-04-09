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
    void positioning();
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    QPixmap pixmap;
    int radius;
    int spacing;
    Pager * pager;
    PagerCircularIndicator * indicator;
};

#endif // PAGERCIRCULARACTIVEINDICATOR_H
