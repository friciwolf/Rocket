#ifndef VERTICALPAGER_H
#define VERTICALPAGER_H

#include <QTimer>
#include <QObject>
#include <QWidget>
#include <QMainWindow>
#include <QGridLayout>
#include <QScrollArea>
#include <QGraphicsView>
#include <QParallelAnimationGroup>

#include "pager/pager.h"
#include "pager/pageritem.h"
#include "../RocketLibrary/tools/kmenuitems.h"

class VerticalPager : public Pager
{
    Q_OBJECT
public:
    ~VerticalPager() {}

    explicit VerticalPager(QWidget * parent, std::vector<KDEApplication> appTree, bool withBackgound);
    void constructPager(std::vector<KDEApplication> kapplications) override;
    void updatePager(std::vector<KDEApplication> kapplications) override;

    void mouseReleaseEvent(QMouseEvent * event) override;
    void mouseMoveEvent(QMouseEvent * event) override;
    void wheelEvent(QWheelEvent *event) override;

public slots:
    void activateSearch(const QString & query) override;
    void goToPage(int deltaPage) override;
    void finishScrolling() override;
    void updatePagerAndPlayAnimationIfOnePageLess() override;
    virtual void enterIconDraggingMode(bool on, IconGridItemCanvas * canvas = nullptr) override
    {
        Pager::enterIconDraggingMode(on,canvas);
    }

private:
    void setTimerDragDelta(QDragMoveEvent *event) override;
};

#endif // VERTICALPAGER_H
