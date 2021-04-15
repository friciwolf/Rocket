#ifndef PAGERCIRCULARINDICATOR_H
#define PAGERCIRCULARINDICATOR_H

#include "pager.h"
#include "verticalpager.h"
#include "stylingparams.h"

#include <QObject>
#include <QWidget>

class PagerCircularIndicator : public QWidget
{
    Q_OBJECT
public:
    explicit PagerCircularIndicator(QWidget *parent, Pager * pager);
    explicit PagerCircularIndicator(QWidget *parent, VerticalPager * pager);
    void positioning();
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);

    int getSpacing() {return m_spacing;}
    int getRadius() {return m_radius;}
    Pager * getPager() {return m_pager;}
    VerticalPager * getVerticalPager() {return m_verticalpager;}

private:
    QPixmap pixmap;
    Pager * m_pager;
    VerticalPager * m_verticalpager;
    QRect m_parent_geometry;

    int m_elements;

    /*
     * Space between the circles and their radius.
    */
    int m_spacing;
    int m_radius;
};

#endif // PAGERCIRCULARINDICATOR_H
