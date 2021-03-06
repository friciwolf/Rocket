#ifndef PAGER_H
#define PAGER_H

#include <QTimer>
#include <QObject>
#include <QWidget>
#include <QMainWindow>
#include <QGridLayout>
#include <QScrollArea>
#include <QGraphicsView>
#include <QParallelAnimationGroup>

#include "pager/pageritem.h"
#include "../RocketLibrary/tools/kmenuitems.h"

class Pager : public QWidget
{
    Q_OBJECT
public:
    ~Pager() {}
    // Number of pages on which applications are displayed
    std::vector<PagerItem*> pages;

    // indicates the currently selected element, and the new one, if the user swipes
    int current_element = 0;
    int new_element = 0;
    int element_before_entering_submenu = 0; // submenu = searching or subfolder

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
    bool in_subfolder = false; //true if we are in a subfolder
    KDEApplication in_subfolder_app; //stores in which subfolder we are currently
    bool page_turned = false;
    bool scrolled = false;
    bool isIconDraggingOn(){return m_icon_dragging_on;}

    explicit Pager(QWidget * parent, std::vector<KDEApplication> appTree, bool withBackgound);
    virtual void constructPager(std::vector<KDEApplication> kapplications);
    virtual void updatePager(std::vector<KDEApplication> kapplications);
    IconGridItem * findGridItemOfMinimumDistance(QPoint referencePoint);

    int getNumberOfElements() {return pages.size();}
    void setApplicationList(std::vector<KDEApplication> newlist){m_kapplications = newlist;}
    std::vector<KDEApplication> getApplicationTree() {return m_kapplication_tree;}
    void setApplicationTree(std::vector<KDEApplication> tree){m_kapplication_tree = tree;}
    std::vector<IconGridItem*> getAllIconGridItems();

    void resizeEvent(QResizeEvent *event);
    void leaveEvent(QEvent * event);
    void mousePressEvent(QMouseEvent * e);
    virtual void mouseReleaseEvent(QMouseEvent * event);
    virtual void mouseMoveEvent(QMouseEvent * event) = 0;
    virtual void wheelEvent(QWheelEvent *event) = 0;
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);

public slots:
    void folderClickEvent(KDEApplication folder);
    void makeFolder(KDEApplication app_dropped_on, KDEApplication app_dragged);
    virtual void enterIconDraggingMode(bool on, IconGridItemCanvas * canvas = nullptr);
    virtual void activateSearch(const QString & query);
    virtual void goToPage(int deltaPage) = 0;
    virtual void finishScrolling() = 0;
    virtual void updatePagerAndPlayAnimationIfOnePageLess() = 0;

signals:
    void hideCircularIndicator(bool indicatorVisibility);
    void enableIconDragging(bool enable);
    void setSearchbarVisibility(bool visibility);

private:
    virtual void setTimerDragDelta(QDragMoveEvent *event) = 0;

protected:
    bool m_icon_dragging_on = false; //true: icons are dragged
    QTimer * m_timer_drag_switch = new QTimer(); //if running, the user is moving elements bw. pages
    QTimer * m_timer_hovering_above_elements = new QTimer(); // delays rearangement animation
    int m_timer_drag_delta = 0; // +1 or -1 if paging is needed
    QPoint m_timer_drag_mouse_pos;
    IconGridItemCanvas * m_item_dragged;
    QParallelAnimationGroup * m_drag_animation = new QParallelAnimationGroup;
    QMainWindow * m_folder_view_window;

    std::vector<KDEApplication> m_kapplications;
    std::vector<KDEApplication> m_kapplication_tree;
    QTimer * m_scrolltimeouttimer = new QTimer();
    QGraphicsView * m_backgroundView;
    bool m_withbackground; // true: draw the background; false for folders
};

#endif // PAGER_H
