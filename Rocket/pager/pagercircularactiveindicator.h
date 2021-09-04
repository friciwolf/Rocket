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
    void setPager(Pager* newPager) {pager=newPager;}
    void setVerticalPager(VerticalPager* newPager) {verticalpager=newPager;}

private:
    QPixmap pixmap;
    int radius;
    int spacing;
    Pager * pager = nullptr;
    VerticalPager * verticalpager = nullptr;
    PagerCircularIndicator * indicator;
};

#endif // PAGERCIRCULARACTIVEINDICATOR_H
