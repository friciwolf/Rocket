#ifndef PAGERCIRCULARINDICATOR_H
#define PAGERCIRCULARINDICATOR_H

#include "pager.h"
#include "stylingparams.h"

#include <QObject>
#include <QWidget>

class PagerCircularIndicator : public QWidget
{
    Q_OBJECT
public:
    explicit PagerCircularIndicator(QWidget *parent, Pager * pager);
    void paintEvent(QPaintEvent *event);

    int getSpacing() {return m_spacing;}
    int getRadius() {return m_radius;}
    Pager * getPager() {return m_pager;}

private:
    QPixmap pixmap;
    Pager * m_pager;

    // The height of the object
    int m_height = RocketStyle::indicator_height;
    int m_width;

    /*
     * Space between the circles and their radius.
    */
    int m_spacing = RocketStyle::indicator_spacing;
    int m_radius = RocketStyle::indicator_radius;
};

#endif // PAGERCIRCULARINDICATOR_H
