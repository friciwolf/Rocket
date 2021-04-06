#ifndef PAGER_H
#define PAGER_H

#include <pager/pageritem.h>

#include <QObject>
#include <QWidget>
#include <QGridLayout>
#include <QScrollArea>
#include <QPropertyAnimation>

class Pager : public QWidget
{
    Q_OBJECT
public:
    QGridLayout * scrollgrid = new QGridLayout();
    QScrollArea * scrollarea;
    std::vector<PagerItem*> pages;
    std::vector<QGridLayout*> pagelayouts;

    int m_width;
    int m_height;

    int m_number_of_elements = 15;

    // indicates the currently selected element, and the new one, if the user swipes
    int current_element = 0;
    int new_element = 0;

    // below this threshold, the user cannot swipe to the next page
    int swipe_decision_threshold = 200;

    // indicates whether the user is currently dragging
    bool dragging = false;
    QPoint drag_start_position;
    QPoint drag_0;

    explicit Pager(QWidget * parent);
    void addItem(PagerItem * page, QGridLayout * layout=nullptr);

    int getNumberOfElements() {return m_number_of_elements;}

    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent * e);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
};

#endif // PAGER_H
