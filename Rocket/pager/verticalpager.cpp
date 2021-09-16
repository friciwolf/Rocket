#include <QDir>
#include <QDrag>
#include <QDebug>
#include <QCursor>
#include <QMimeData>
#include <QDropEvent>
#include <QMouseEvent>
#include <QApplication>
#include <QPropertyAnimation>
#include <QGraphicsBlurEffect>
#include <QParallelAnimationGroup>

#include <KConfig>
#include <KConfigGroup>
#include <KWindowEffects>

#include "icongrid/icongrid.h"
#include "tools/searchingapps.h"
#include "pager/verticalpager.h"
#include "pager/pagerfolderview.h"

#include "../RocketLibrary/tools/kmenuitems.h"
#include "../RocketLibrary/tools/rocketconfigmanager.h"

VerticalPager::VerticalPager(QWidget *parent, std::vector<KDEApplication> appTree, bool withBackgound) : Pager(parent, appTree, withBackgound)
{
    connect(m_timer_drag_switch,&QTimer::timeout,this,[=]{
        m_timer_drag_mouse_pos = QCursor::pos();

        current_element = current_element + m_timer_drag_delta;
        element_before_entering_submenu = (!in_subfolder ? current_element : element_before_entering_submenu);
        new_element = current_element;
        scrolled = false;
        touchpad = false;
        QParallelAnimationGroup * animationgroup = new QParallelAnimationGroup;
        for (int i=0;i<(int)pages.size(); i++)
        {
            QPropertyAnimation * animation = new QPropertyAnimation(pages[i],"pos");
            animation->setTargetObject(pages[i]);
            animation->setStartValue(pages[i]->pos());
            animation->setEndValue(QPoint(pages[i]->pos().x(),(i-new_element)*height()));
            animation->setDuration(100);
            animationgroup->addAnimation(animation);
        }
        connect(animationgroup,&QParallelAnimationGroup::finished,this,[=]{
            int dx0 = (QCursor::pos()-m_timer_drag_mouse_pos).x();
            int dy0 = (QCursor::pos()-m_timer_drag_mouse_pos).y();
            if (dx0*dx0+dy0*dy0<RocketStyle::click_tolerance && current_element+m_timer_drag_delta>=0 && current_element+m_timer_drag_delta<this->getNumberOfElements())
            {
                m_timer_drag_switch->start(750);
            }
            else {
                m_timer_drag_delta = 0;
            }
        });
        animationgroup->start();
    });
}

void VerticalPager::constructPager(std::vector<KDEApplication> kapplications)
{
    Pager::constructPager(kapplications);
    for (PagerItem * page : pages)
    {
        connect(page->getIconGrid(),&IconGrid::goToPage,this,&VerticalPager::goToPage);
    }

    for (IconGridItem * i : getAllIconGridItems())
    {
        connect(i->getCanvas(),&IconGridItemCanvas::makeFolder,this,&VerticalPager::makeFolder);
        connect(i->getCanvas(),&IconGridItemCanvas::folderClickEvent,this,&VerticalPager::folderClickEvent);
        connect(i->getCanvas(),&IconGridItemCanvas::enterIconDraggingMode,this,&VerticalPager::enterIconDraggingMode);
        connect(this,&VerticalPager::enableIconDragging,i->getCanvas(),&IconGridItemCanvas::setDraggable);
    }
}

void VerticalPager::updatePager(std::vector<KDEApplication> kapplications)
{
    Pager::updatePager(kapplications);
    VerticalPager::constructPager(kapplications);
    for (int i=0;i<(int)pages.size();i++)
    {
        pages[i]->setGeometry(QRect(0,(i-current_element)*height(),width(),height()));
        pages[i]->setEnabled(true);
        pages[i]->setVisible(true);
    }
}

// ***********************************************
//                    EVENTS
// ***********************************************

void VerticalPager::mouseMoveEvent(QMouseEvent * event)
{
    if (scrolled) // pages have been scrolled
    {
        if (mouse_pos_scroll_0!=QCursor::pos())
        {
            finishScrolling(); // update the state
        }
    }
    else // pages have been dragged manually
    {
        int dy0 = (QCursor::pos()-drag_0).y();
        if (dragging && !searching)
        {
            if ((current_element==0 && dy0>RocketStyle::pager_deadzone_threshold) || (current_element==(int)pages.size()-1 && dy0<-RocketStyle::pager_deadzone_threshold))
            {
                event->accept();
            }
            else
            {
                QPoint delta = QCursor::pos()-drag_start_position;
                for (PagerItem *page_i : pages)
                {
                    page_i->move(page_i->pos().x(),(page_i->pos()+delta).y());
                }
                drag_start_position = QCursor::pos();
            }
        }
    }
    event->accept();
    //qDebug() << "pager: mouse move!";
}

void VerticalPager::mouseReleaseEvent(QMouseEvent * event)
{
    Pager::mouseReleaseEvent(event);

    // Scroll if neccessary and update
    new_element = current_element;
    if ((QCursor::pos()-drag_0).y()>swipe_decision_threshold && current_element!=0) {
        new_element = current_element-1;
    }
    if ((QCursor::pos()-drag_0).y()<-swipe_decision_threshold && current_element!=(int)pages.size()-1){
        new_element = current_element+1;
    }

    QParallelAnimationGroup * animationgroup = new QParallelAnimationGroup;
    for (int i=0;i<(int)pages.size(); i++)
    {
        QPropertyAnimation * animation = new QPropertyAnimation(pages[i],"pos");
        animation->setTargetObject(pages[i]);
        animation->setStartValue(pages[i]->pos());
        animation->setEndValue(QPoint(pages[i]->pos().x(),(i-new_element)*height()));
        animation->setDuration(100);
        animationgroup->addAnimation(animation);
    }
    animationgroup->start();

    pages[new_element]->getIconGrid()->resetHighlightAndActiveElement();
    current_element = new_element;
    element_before_entering_submenu = (!in_subfolder ? current_element : element_before_entering_submenu);
    dragging = false;
    event->accept();
    //qDebug() << "pager: mouse up!";
}

void VerticalPager::wheelEvent(QWheelEvent *event)
{
    pages[current_element]->getIconGrid()->resetHighlightAndActiveElement();
    if (!searching)
    {
        if (event->angleDelta().y() % 120 == 0 && event->angleDelta().y()!=0 && !touchpad) //scrolling with a mouse
        {
            if (event->angleDelta().y()<0) goToPage(1);
            else goToPage(-1);
            pages[current_element]->getIconGrid()->resetHighlightAndActiveElement();
        }
        else // scrolling with a touchpad
        {
            touchpad = true; // touchpads need to build up some momentum, thus this switch
            bool horizontal_now = (std::abs(event->angleDelta().x())>std::abs(event->angleDelta().y()));
            if (m_horizontal_scrolling==0)
                    m_horizontal_scrolling = (horizontal_now ? 1 : -1);
            int delta = (event->angleDelta().y()==0 ? -(event->angleDelta().x())*ConfigManager.getInvertedScrollFactorXfromSettings() : event->angleDelta().y()*ConfigManager.getInvertedScrollFactorYfromSettings() );
            bool orig_horizontal = (m_horizontal_scrolling==1 ? true : false);
            if (orig_horizontal!=horizontal_now) return;
            if (pages[0]->pos().y()-delta<=0 && pages[0]->pos().y()-delta>=-height()*(getNumberOfElements()-1))
            {
                m_scrolltimeouttimer->stop();
                scrolled = true;
                mouse_pos_scroll_0 = QCursor::pos();
                for (PagerItem *page_i : pages)
                {
                    page_i->move(page_i->pos().x(),page_i->pos().y()-delta);
                }
                m_scrolltimeouttimer = new QTimer();
                connect(m_scrolltimeouttimer,&QTimer::timeout,this,&VerticalPager::finishScrolling);
                m_scrolltimeouttimer->setSingleShot(true);
                m_scrolltimeouttimer->start(250);
            }
            else {
                // we've reached the last/first page, don't do anything anymore
                current_element = (-pages[0]->pos().y()+height()/2)/height();
                element_before_entering_submenu = (!in_subfolder ? current_element : element_before_entering_submenu);
                new_element = current_element;
                for (int i=0;i<(int)pages.size(); i++)
                {
                    pages[i]->move(QPoint(pages[i]->pos().x(),(i-new_element)*height()));
                }
            }
        }
    }
    event->accept();
}

// ***********************************************
//                    SLOTS
// ***********************************************

void VerticalPager::activateSearch(const QString &query)
{
    if (scrolled) // pages have been scrolled
    {
        m_scrolltimeouttimer->stop();
        if (mouse_pos_scroll_0!=QCursor::pos())
        {
            current_element = (-pages[0]->pos().y()+height()/2)/height();
            element_before_entering_submenu = (!in_subfolder ? current_element : element_before_entering_submenu);
            new_element = current_element;
            scrolled = false;
        }
    }
    Pager::activateSearch(query);
}

void VerticalPager::goToPage(int deltaPage)
{
    if (current_element+deltaPage==getNumberOfElements() || current_element+deltaPage==-1) return;
    if (searching) return;
    int newpage = current_element+deltaPage;
    for (int i=0;i<(int)pages.size(); i++)
    {
        pages[i]->move(QPoint(pages[i]->pos().x(),(i-newpage)*height()));
    }
    if (deltaPage==1)
    {
        IconGrid * oldGrid = pages[current_element]->getIconGrid();
        int oldElement = oldGrid->getActiveElement(); // 0 ... N_elements
        int oldElementColumn = (oldElement) % oldGrid->getMaxNumberOfColumns()+1; // 1...N_cols
        IconGrid * newGrid = pages[current_element+1]->getIconGrid();
        if (oldElementColumn > newGrid->getCurrentNumberOfColumns())
        {
            newGrid->highlight(newGrid->getCurrentNumberOfColumns()-1);
        }
        else
        {
            newGrid->highlight((oldElementColumn-1==-1 ? 0 : oldElementColumn-1));
        }
    }
    if (deltaPage==-1)
    {
        IconGrid * oldGrid = pages[current_element]->getIconGrid();
        int oldElement = oldGrid->getActiveElement(); // 0 ... N_elements
        int oldElementColumn = (oldElement) % oldGrid->getMaxNumberOfColumns()+1; // 1...N_cols
        IconGrid * newGrid = pages[current_element-1]->getIconGrid();
        newGrid->highlight(newGrid->getNumberOfItems()-(newGrid->getMaxNumberOfColumns()-oldElementColumn)-1);







    }


    current_element = newpage;
    new_element = current_element;
    element_before_entering_submenu = (!in_subfolder ? current_element : element_before_entering_submenu);
    page_turned = true;
}

void VerticalPager::finishScrolling()
{
    current_element = (-pages[0]->pos().y()+height()/2)/height();
    element_before_entering_submenu = (!in_subfolder ? current_element : element_before_entering_submenu);
    new_element = current_element;
    scrolled = false;
    touchpad = false;
    if (pages[0]->pos().y()!=0 || pages[0]->pos().y()!=-height()*(getNumberOfElements()-1))
    {
        QParallelAnimationGroup * animationgroup = new QParallelAnimationGroup;
        for (int i=0;i<(int)pages.size(); i++)
        {
            QPropertyAnimation * animation = new QPropertyAnimation(pages[i],"pos");
            animation->setTargetObject(pages[i]);
            animation->setStartValue(pages[i]->pos());
            animation->setEndValue(QPoint(pages[i]->pos().x(),(i-new_element)*height()));
            animation->setDuration(100);
            animationgroup->addAnimation(animation);
        }
        animationgroup->start();
    }
    m_horizontal_scrolling = 0;
}

void VerticalPager::updatePagerAndPlayAnimationIfOnePageLess()
{
    int numberofelementsbefore = getNumberOfElements();
    updatePager(m_kapplication_tree);
    // move pager to the right position if there is one page less, and we are on the last page
    if (getNumberOfElements() != numberofelementsbefore && element_before_entering_submenu==numberofelementsbefore-1)
    {
        current_element = getNumberOfElements()-1;
        element_before_entering_submenu = current_element;
        new_element = current_element;
        QParallelAnimationGroup * animationgroup = new QParallelAnimationGroup;
        for (int i=0;i<(int)pages.size(); i++)
        {
            QPropertyAnimation * animation = new QPropertyAnimation(pages[i],"pos");
            animation->setTargetObject(pages[i]);
            animation->setStartValue(pages[i]->pos());
            animation->setEndValue(QPoint(pages[i]->pos().x(),(i-new_element)*height()));
            animation->setDuration(100);
            animationgroup->addAnimation(animation);
        }
        animationgroup->start();
    }
}

void VerticalPager::setTimerDragDelta(QDragMoveEvent *event)
{
    if (!m_timer_drag_switch->isActive())
    {
        if (event->pos().y()>pages[current_element]->getIconGrid()->geometry().bottom()
                && current_element+1<getNumberOfElements())
        {
            m_timer_drag_delta = 1;
        }
        else if (event->pos().y()<pages[current_element]->getIconGrid()->geometry().top()
                             && current_element-1>=0)
        {
             m_timer_drag_delta = -1;
        }
        if (m_timer_drag_delta!=0)
        {
            m_timer_drag_mouse_pos = QCursor::pos();
            m_timer_drag_switch->start(750);
        }
    }
}
