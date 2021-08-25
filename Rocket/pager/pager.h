#ifndef PAGER_H
#define PAGER_H

#include "pager/pageritem.h"

#include <QObject>
#include <QWidget>
#include <QGridLayout>
#include <QScrollArea>
#include <QParallelAnimationGroup>
#include <QTimer>
#include <QGraphicsView>

#include "../RocketLibrary/tools/kmenuitems.h"

class Pager : public QWidget
{
    Q_OBJECT
public:
    // Number of pages on which applications are displayed
    std::vector<PagerItem*> pages;

    // indicates the currently selected element, and the new one, if the user swipes
    int current_element = 0;
    int new_element = 0;
    int element_before_searching = 0;

    // below this threshold, the user cannot swipe to the next page
    int swipe_decision_threshold = RocketStyle::pager_swpipe_threshold;

    // indicates whether the user is currently dragging
    bool dragging = false;
    // Mouse position previously to compare new mouse movements to and to adjust the layout during dragging
    QPoint drag_start_position;
    // Mouse position at the time of the start of dragging procedure with the mouse
    QPoint drag_0;
    // Mouse position for touchpad scrolling
    QPoint mouse_pos_scroll_0;
    bool touchpad = false;
    int m_horizontal_scrolling = 0;
    bool searching = false;
    bool page_turned = false;
    bool scrolled = false;
    bool isIconDraggingOn(){return m_icon_dragging_on;}

    explicit Pager(QWidget * parent);
    void constructPager(std::vector<KDEApplication> kapplications);
    void addItem(PagerItem * page);
    void updatePager(std::vector<KDEApplication> kapplications);
    IconGridItem * findGridItemOfMinimumDistance(QPoint referencePoint);

    int getNumberOfElements() {return pages.size();}
    void setApplicationList(std::vector<KDEApplication> newlist){m_kapplications = newlist;}
    std::vector<KDEApplication> getApplicationTree() {return m_kapplication_tree;}
    void setApplicationTree(std::vector<KDEApplication> tree){m_kapplication_tree = tree;}

    std::vector<IconGridItem*> getAllIconGridItems();

    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent * e);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void wheelEvent(QWheelEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);

public slots:
    void activateSearch(const QString & query);
    void goToPage(int deltaPage);
    void finishScrolling();
    void enterIconDraggingMode(bool on, IconGridItemCanvas * canvas = nullptr);

signals:
    void updated(bool indicator_visibility);
    void enableIconDragging(bool enable);

private:
    bool m_icon_dragging_on = false; //true: icons are dragged
    QTimer * m_timer_drag_switch = new QTimer(); //if running, the user is moving elements bw. pages
    QTimer * m_timer_hovering_above_elements = new QTimer(); // delays rearangement animation
    int m_timer_drag_delta = 0; // +1 or -1 if paging is needed
    QPoint m_timer_drag_mouse_pos;
    IconGridItemCanvas * m_item_dragged;
    QParallelAnimationGroup * m_drag_animation = new QParallelAnimationGroup;

    std::vector<KDEApplication> m_kapplications;
    std::vector<KDEApplication> m_kapplication_tree;
    QTimer * m_scrolltimeouttimer = new QTimer();
    QGraphicsView * m_backgroundView;
};

#endif // PAGER_H
