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
#include "pager/horizontalpager.h"
#include "pager/pagerfolderview.h"

#include "../RocketLibrary/tools/kmenuitems.h"
#include "../RocketLibrary/tools/rocketconfigmanager.h"

HorizontalPager::HorizontalPager(QWidget *parent, std::vector<KDEApplication> appTree, bool withBackgound) : Pager(parent, appTree, withBackgound)
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
            animation->setEndValue(QPoint((i-new_element)*width(),pages[i]->pos().y()));
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

void HorizontalPager::constructPager(std::vector<KDEApplication> kapplications)
{
    Pager::constructPager(kapplications);
    for (PagerItem * page : pages)
    {
        connect(page->getIconGrid(),&IconGrid::goToPage,this,&HorizontalPager::goToPage);
    }

    for (IconGridItem * i : getAllIconGridItems())
    {
        connect(i->getCanvas(),&IconGridItemCanvas::makeFolder,this,&HorizontalPager::makeFolder);
        connect(i->getCanvas(),&IconGridItemCanvas::folderClickEvent,this,&HorizontalPager::folderClickEvent);
        connect(i->getCanvas(),&IconGridItemCanvas::enterIconDraggingMode,this,&HorizontalPager::enterIconDraggingMode);
        connect(this,&HorizontalPager::enableIconDragging,i->getCanvas(),&IconGridItemCanvas::setDraggable);
    }
}

void HorizontalPager::updatePager(std::vector<KDEApplication> kapplications)
{
    Pager::updatePager(kapplications);
    HorizontalPager::constructPager(kapplications);
    for (int i=0;i<(int)pages.size();i++)
    {
        pages[i]->setGeometry(QRect((i-current_element)*width(),0,width(),height()));
        pages[i]->setEnabled(true);
        pages[i]->setVisible(true);
    }
}

// ***********************************************
//                    EVENTS
// ***********************************************

void HorizontalPager::mouseMoveEvent(QMouseEvent * event)
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
        int dx0 = (QCursor::pos()-drag_0).x();
        if (dragging && !searching)
        {
            if ((current_element==0 && dx0>RocketStyle::pager_deadzone_threshold) || (current_element==(int)pages.size()-1 && dx0<-RocketStyle::pager_deadzone_threshold))
            {
                event->accept();
            }
            else
            {
                QPoint delta = QCursor::pos()-drag_start_position;
                for (PagerItem *page_i : pages)
                {
                    page_i->move((page_i->pos()+delta).x(),page_i->pos().y());
                }
                drag_start_position = QCursor::pos();
            }
        }
    }
    event->accept();
    //qDebug() << "pager: mouse move!";
}

void HorizontalPager::mouseReleaseEvent(QMouseEvent * event)
{
    Pager::mouseReleaseEvent(event);

    // Scroll if neccessary and update
    new_element = current_element;
    if ((QCursor::pos()-drag_0).x()>swipe_decision_threshold && current_element!=0) {
        new_element = current_element-1;
    }
    if ((QCursor::pos()-drag_0).x()<-swipe_decision_threshold && current_element!=(int)pages.size()-1){
        new_element = current_element+1;
    }

    QParallelAnimationGroup * animationgroup = new QParallelAnimationGroup;
    for (int i=0;i<(int)pages.size(); i++)
    {
        QPropertyAnimation * animation = new QPropertyAnimation(pages[i],"pos");
        animation->setTargetObject(pages[i]);
        animation->setStartValue(pages[i]->pos());
        animation->setEndValue(QPoint((i-new_element)*width(),pages[i]->pos().y()));
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

void HorizontalPager::wheelEvent(QWheelEvent *event)
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
            int delta = (horizontal_now ? -(event->angleDelta().x())*ConfigManager.getInvertedScrollFactorXfromSettings() : event->angleDelta().y()*ConfigManager.getInvertedScrollFactorYfromSettings() );
            bool orig_horizontal = (m_horizontal_scrolling==1 ? true : false);
            if (orig_horizontal!=horizontal_now) return;
            if (pages[0]->pos().x()-delta<=0 && pages[0]->pos().x()-delta>=-width()*(getNumberOfElements()-1))
            {
                m_scrolltimeouttimer->stop();
                scrolled = true;
                mouse_pos_scroll_0 = QCursor::pos();
                for (PagerItem *page_i : pages)
                {
                    page_i->move(page_i->pos().x()-delta,page_i->pos().y());
                }
                m_scrolltimeouttimer = new QTimer();
                connect(m_scrolltimeouttimer,&QTimer::timeout,this,&HorizontalPager::finishScrolling);
                m_scrolltimeouttimer->setSingleShot(true);
                m_scrolltimeouttimer->start(250);
            }
            else {
                // we've reached the last/first page, don't do anything anymore
                current_element = (-pages[0]->pos().x()+width()/2)/width();
                element_before_entering_submenu = (!in_subfolder ? current_element : element_before_entering_submenu);
                new_element = current_element;
                for (int i=0;i<(int)pages.size(); i++)
                {
                    pages[i]->move(QPoint((i-new_element)*width(),pages[i]->pos().y()));
                }
            }
        }
    }
    event->accept();
}

// ***********************************************
//                    SLOTS
// ***********************************************

void HorizontalPager::activateSearch(const QString &query)
{
    if (scrolled) // pages have been scrolled
    {
        m_scrolltimeouttimer->stop();
        if (mouse_pos_scroll_0!=QCursor::pos())
        {
            current_element = (-pages[0]->pos().x()+width()/2)/width();
            element_before_entering_submenu = (!in_subfolder ? current_element : element_before_entering_submenu);
            new_element = current_element;
            scrolled = false;
        }
    }
    Pager::activateSearch(query);
}

void HorizontalPager::goToPage(int deltaPage)
{
    if (current_element+deltaPage==getNumberOfElements() || current_element+deltaPage==-1) return;
    if (searching) return;
    int newpage = current_element+deltaPage;
    for (int i=0;i<(int)pages.size(); i++)
    {
        pages[i]->move(QPoint((i-newpage)*width(),pages[i]->pos().y()));
    }
    if (deltaPage==1)
    {
        IconGrid * oldGrid = pages[current_element]->getIconGrid();
        int oldElement = oldGrid->getActiveElement(); // 0 ... N_elements
        int oldElementRow = (int) ((oldElement)/(oldGrid->getCurrentNumberOfColumns()))+1; // 1...N_rows
        IconGrid * newGrid = pages[current_element+1]->getIconGrid();
        if (oldElementRow > newGrid->getCurrentNumberOfRows())
        {
            if (newGrid->getCurrentNumberOfRows()==1)
            {
                newGrid->highlight(0);
            }
            else
            {
                newGrid->highlight(newGrid->getCurrentNumberOfColumns()*(newGrid->getCurrentNumberOfRows()-1));
            }
        }
        else
        {
            newGrid->highlight((oldElementRow-1)*newGrid->getCurrentNumberOfColumns());
        }
    }
    if (deltaPage==-1)
    {
        IconGrid * oldGrid = pages[current_element]->getIconGrid();
        int oldElement = oldGrid->getActiveElement(); // 0 ... N_elements
        int oldElementRow = (int) ((oldElement)/(oldGrid->getCurrentNumberOfColumns())); // 0...N_rows
        IconGrid * newGrid = pages[current_element-1]->getIconGrid();
        newGrid->highlight((oldElementRow+1)*newGrid->getCurrentNumberOfColumns()-1);
    }


    current_element = newpage;
    new_element = current_element;
    element_before_entering_submenu = (!in_subfolder ? current_element : element_before_entering_submenu);
    page_turned = true;
}

void HorizontalPager::finishScrolling()
{
    current_element = (-pages[0]->pos().x()+width()/2)/width();
    element_before_entering_submenu = (!in_subfolder ? current_element : element_before_entering_submenu);
    new_element = current_element;
    scrolled = false;
    touchpad = false;
    if (pages[0]->pos().x()!=0 || pages[0]->pos().x()!=-width()*(getNumberOfElements()-1))
    {
        QParallelAnimationGroup * animationgroup = new QParallelAnimationGroup;
        for (int i=0;i<(int)pages.size(); i++)
        {
            QPropertyAnimation * animation = new QPropertyAnimation(pages[i],"pos");
            animation->setTargetObject(pages[i]);
            animation->setStartValue(pages[i]->pos());
            animation->setEndValue(QPoint((i-new_element)*width(),pages[i]->pos().y()));
            animation->setDuration(100);
            animationgroup->addAnimation(animation);
        }
        animationgroup->start();
    }
    m_horizontal_scrolling = 0;
}

void HorizontalPager::updatePagerAndPlayAnimationIfOnePageLess()
{
    int numberofelementsbefore = this->getNumberOfElements();
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
            animation->setEndValue(QPoint((i-new_element)*width(),pages[i]->pos().y()));
            animation->setDuration(100);
            animationgroup->addAnimation(animation);
        }
        animationgroup->start();
    }
}

void HorizontalPager::setTimerDragDelta(QDragMoveEvent *event)
{
    if (!m_timer_drag_switch->isActive())
    {
        if (event->pos().x()>pages[current_element]->getIconGrid()->geometry().right()
                && current_element+1<getNumberOfElements())
        {
            m_timer_drag_delta = 1;
        }
        else if (event->pos().x()<pages[current_element]->getIconGrid()->geometry().left()
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
