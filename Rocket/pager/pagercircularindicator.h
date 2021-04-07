#ifndef PAGERCIRCULARINDICATOR_H
#define PAGERCIRCULARINDICATOR_H

#include "pager.h"

#include <QObject>
#include <QWidget>

class PagerCircularIndicator : public QWidget
{
    Q_OBJECT
public:
    explicit PagerCircularIndicator(QWidget *parent, Pager * pager);
    void paintEvent(QPaintEvent *event);

    int getSpacing() {return spacing;}
    int getRadius() {return radius;}
    Pager * getPager() {return m_pager;}

    // The height of the object
    int height = 30;
    int width;

public slots:
    void repaint();

private:
    QPixmap pixmap;
    Pager * m_pager;
    /*
     * Space between the circles and their radius.
    */
    int spacing = 5;
    int radius = 10;
};

#endif // PAGERCIRCULARINDICATOR_H
