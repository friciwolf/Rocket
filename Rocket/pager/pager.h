#ifndef PAGER_H
#define PAGER_H

#include <pager/pageritem.h>

#include <QObject>
#include <QWidget>
#include <QGridLayout>
#include <QScrollArea>
#include <QPropertyAnimation>
#include <QTimer>
#include "icongrid/kmenuitems.h"
#include "stylingparams.h"

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
    QPoint drag_start_position;
    QPoint drag_0;
    QPoint mouse_pos_scroll_0;
    bool searching = false;
    bool page_turned = false;
    bool scrolled = false;

    explicit Pager(QWidget * parent);
    void constructPager(std::vector<KApplication> kapplications);
    void addItem(PagerItem * page);
    void updatePager(std::vector<KApplication> kapplications);

    int getNumberOfElements() {return pages.size();}


    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent * e);
    void mouseMoveEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void wheelEvent(QWheelEvent *event);

public slots:
    void activateSearch(const QString & query);
    void goToPage(int deltaPage);
    void finishScrolling();

signals:
    void updated(bool indicator_visibility);

private:
    std::vector<KApplication> m_kapplications;
    QTimer * m_scrolltimeouttimer = new QTimer();
};

#endif // PAGER_H
