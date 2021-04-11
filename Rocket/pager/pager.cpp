#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QCursor>
#include <QDebug>
#include <QDir>
#include <QApplication>

#include "pager/pager.h"
#include "icongrid/kmenuitems.h"
#include "icongrid/icongrid.h"
#include "stylingparams.h"
#include "tools/searchingapps.h"
#include "tools/rocketconfigmanager.h"

Pager::Pager(QWidget *parent) : QWidget(parent)
{    
    // Colouring the background...
    //QPalette palette;
    setAutoFillBackground(true);
    //palette.setColor(QPalette::ColorRole::Background,QColor(255,255,100,100));
    //setPalette(palette);

    // Setting the sizes
    setFixedSize(parent->size());

    // Getting the entries
    //KMenuItems m_menuitems;
    //m_menuitems.scanElements();

    //m_kapplications = m_menuitems.getApplications();
    m_kapplications = ConfigManager.getApplications();
    constructPager(m_kapplications);
    setMouseTracking(true);
}

void Pager::constructPager(std::vector<KApplication> kapplications)
{
    std::vector<KApplication> applications;
    int maxicons = ConfigManager.getRowNumber()*ConfigManager.getColumnNumber();

    // Filling up the grid...
    for (int i=0;i<kapplications.size();i++)
    {
        if ((i%maxicons)!=0 || i==0)
        {
            applications.push_back(kapplications[i]);
        }
        else
        {
            PagerItem * page = new PagerItem(this,applications);
            connect(page->getIconGrid(),&IconGrid::goToPage,this,&Pager::goToPage);
            addItem(page);
            applications.clear();

            applications.push_back(kapplications[i]);
        }
    }

    // Adding the rest..
    PagerItem * page = new PagerItem(this,applications);
    connect(page->getIconGrid(),&IconGrid::goToPage,this,&Pager::goToPage);
    addItem(page);
    if (kapplications.size()==1) // only 1 item found after searching
    {
        page->getIconGrid()->highlight(0);
    }

    QPixmap bkgnd(QDir::homePath()+"/.config/rocket/wallpaper.jpeg");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    setPalette(palette);
}

void Pager::updatePager(std::vector<KApplication> kapplications)
{
    /*
     * Updates the whole pager, including its children (the icongrids), whenever needed (e.g. while searching).
     *
     * */
    for (PagerItem * i: pages)
    {
        delete i;
    }
    pages.clear();

    if (kapplications == m_kapplications)
    {
        current_element = element_before_searching;
    }
    else
    {
        current_element = 0;
    }
    constructPager(kapplications);

    for (int i=0;i<pages.size();i++)
    {
        pages[i]->setGeometry(QRect((i-current_element)*width(),0,width(),height()));
        pages[i]->setEnabled(true);
        pages[i]->setVisible(true);
    }
}

void Pager::addItem(PagerItem * page)
{
    pages.push_back(page);
}

void Pager::goToPage(int deltaPage)
{
    if (current_element+deltaPage==getNumberOfElements() || current_element+deltaPage==-1) return;
    if (searching) return;
    int newpage = current_element+deltaPage;
    for (int i=0;i<pages.size(); i++)
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
    element_before_searching = current_element;
    page_turned = true;
}

void Pager::resizeEvent(QResizeEvent *event)
{
    updatePager(m_kapplications);
    for (int i=0;i<pages.size();i++)
    {
        pages[i]->resizeEvent(event);
    }
}

void Pager::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        pages[current_element]->getIconGrid()->resetHighlightAndActiveElement();
        drag_start_position = QCursor::pos();
        drag_0 = QCursor::pos();
        dragging = true;
    }
    e->accept();
}

void Pager::mouseMoveEvent(QMouseEvent * event)
{
    if (scrolled) // pages have been scrolled
    {
        if (mouse_pos_scroll_0!=QCursor::pos())
        {
            finishScrolling();
        }
    }
    else // pages have been dragged manually
    {
        int dx0 = (QCursor::pos()-drag_0).x();
        if (dragging && !searching)
        {
            if ((current_element==0 && dx0>RocketStyle::pager_deadzone_threshold) || (current_element==pages.size()-1 && dx0<-RocketStyle::pager_deadzone_threshold))
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
}

void Pager::mouseReleaseEvent(QMouseEvent * event)
{
    // Checking whether outside area is clicked; if it's the case, close the app
    int dx0 = (QCursor::pos()-drag_0).x();
    int dy0 = (QCursor::pos()-drag_0).y();
    if (!searching && dx0*dx0+dy0*dy0<RocketStyle::click_tolerance && event->button()==Qt::LeftButton)
    {
        bool itemclicked = false;
        for (PagerItem * i : pages)
        {
            if (i->getIconGrid()->geometry().contains(event->pos()))
            {
                itemclicked = true;
                break;
            }
        }
        if (!itemclicked)
        {
            event->accept();
            qApp->exit();
        }
    }
    // Don't allow scrolling while searching
    if (searching)
    {
        event->ignore();
        return;
    }
    // Scroll if neccessary and update
    if (dx0>swipe_decision_threshold && current_element!=0) {
        new_element = current_element-1;
    }
    if (dx0<-swipe_decision_threshold && current_element!=pages.size()-1){
        new_element = current_element+1;
    }

    for (int i=0;i<pages.size(); i++)
    {
        QPropertyAnimation * animation = new QPropertyAnimation(pages[i],"pos");
        animation->setStartValue(pages[i]->pos());
        animation->setEndValue(QPoint((i-new_element)*width(),pages[i]->pos().y()));
        animation->setDuration(100);
        animation->start();
    }

    pages[new_element]->getIconGrid()->resetHighlightAndActiveElement();
    current_element = new_element;
    element_before_searching = current_element;
    dragging = false;
    event->accept();
}

void Pager::wheelEvent(QWheelEvent *event)
{
    pages[current_element]->getIconGrid()->resetHighlightAndActiveElement();
    if (!searching)
    {
        if (event->angleDelta().y() % 120 == 0 && event->angleDelta().y()!=0) //scrolling with a mouse
        {
            if (event->angleDelta().y()<0) goToPage(1);
            else goToPage(-1);
            pages[current_element]->getIconGrid()->resetHighlightAndActiveElement();
            event->accept();
        }
        else // scrolling with a touchpad
        {
            int delta = (event->angleDelta().y()==0 ? -(event->angleDelta().x()) : event->angleDelta().y() );
            if ((pages[0]->pos().x()<=0 && delta<0) || (pages[0]->pos().x()>=-pages[0]->width()*(getNumberOfElements()-1)&& delta>0))
            {
                if (pages[0]->pos().x()-delta<=0 && pages[0]->pos().x()-delta>=-pages[0]->width()*(getNumberOfElements()-1))
                {
                    m_scrolltimeouttimer->stop();
                    event->ignore();
                    scrolled = true;
                    mouse_pos_scroll_0 = QCursor::pos();
                    for (PagerItem *page_i : pages)
                    {
                        page_i->move(page_i->pos().x()-delta,page_i->pos().y());
                    }
                    m_scrolltimeouttimer = new QTimer();
                    connect(m_scrolltimeouttimer,&QTimer::timeout,this,&Pager::finishScrolling);
                    m_scrolltimeouttimer->start(500);
                    m_scrolltimeouttimer->setSingleShot(true);
                }
                event->accept();
            }
        }
    }
}

void Pager::finishScrolling()
{
    current_element = (-pages[0]->pos().x()+pages[0]->width()/2)/pages[0]->width();
    element_before_searching = current_element;
    new_element = current_element;
    scrolled = false;
    for (int i=0;i<pages.size(); i++)
    {
        QPropertyAnimation * animation = new QPropertyAnimation(pages[i],"pos");
        animation->setStartValue(pages[i]->pos());
        animation->setEndValue(QPoint((i-new_element)*width(),pages[i]->pos().y()));
        animation->setDuration(100);
        animation->start();
    }
}


void Pager::activateSearch(const QString &query)
{
    if (scrolled) // pages have been scrolled
    {
        m_scrolltimeouttimer->stop();
        if (mouse_pos_scroll_0!=QCursor::pos())
        {
            current_element = (-pages[0]->pos().x()+pages[0]->width()/2)/pages[0]->width();
            element_before_searching = current_element;
            new_element = current_element;
            scrolled = false;
        }
    }
    searching = (query!="");
    std::vector<KApplication> found_apps = searchApplication(m_kapplications,query);
    updatePager(found_apps);
    updated(searching);
}
