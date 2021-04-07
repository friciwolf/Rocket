#include "pager/pager.h"
#include "icongrid/kmenuitems.h"
#include "icongrid/icongrid.h"
#include "stylingparams.h"
#include "tools/searchingapps.h"

#include <QScrollArea>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QCursor>
#include <QDebug>

Pager::Pager(QWidget *parent) : QWidget(parent)
{
    // Colouring the background...
    //QPalette palette;
    setAutoFillBackground(true);
    //palette.setColor(QPalette::ColorRole::Background,QColor(255,255,100,100));
    //setPalette(palette);

    // Setting the sizes
    setGeometry(parent->geometry());
    m_width = parent->geometry().width();
    m_height = parent->geometry().height();

    // Getting the entries
    KMenuItems m_menuitems;
    m_menuitems.scanElements();

    m_kapplications = m_menuitems.getApplications();
    constructPager(m_kapplications);
    resizeEvent(nullptr);
}

void Pager::constructPager(std::vector<KApplication> kapplications)
{
    std::vector<KApplication> applications;
    int maxicons = RocketStyle::m_rows*RocketStyle::m_cols;

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
            addItem(page);
            applications.clear();

            applications.push_back(kapplications[i]);
        }
    }

    // Adding the rest..
    // TODO: filling up the rest with empty entries
    PagerItem * page = new PagerItem(this,applications);
    addItem(page);
}

void Pager::updatePager(std::vector<KApplication> kapplications)
{
    for (PagerItem * i: pages)
    {
        delete i;
    }
    pages.clear();

    if (kapplications == m_kapplications)
    {
        qDebug() << "matches" << element_before_searching;
        current_element = element_before_searching;
    }
    else
    {
        current_element = 0;
    }
    constructPager(kapplications);

    for (int i=0;i<pages.size();i++)
    {
        // +30: due to the height of the indicators!
        pages[i]->setGeometry(QRect((i-current_element)*m_width,0,m_width,m_height+30*2));
        pages[i]->setEnabled(true);
        pages[i]->setVisible(true);
    }
}

void Pager::addItem(PagerItem * page)
{
    pages.push_back(page);
}

void Pager::resizeEvent(QResizeEvent *event)
{
    m_width = this->width();
    m_height = this->height();
    for (int i=0;i<pages.size();i++)
    {
        // +30: due to the height of the indicators!
        pages[i]->setGeometry(QRect((i-current_element)*m_width,0,m_width,m_height+30*2));
    }
    QPixmap bkgnd("grid.jpeg");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    setPalette(palette);
}

void Pager::mousePressEvent(QMouseEvent *e)
{
    dragging = true;
    drag_start_position = QCursor::pos();
    drag_0 = QCursor::pos();
    e->accept();
}

void Pager::mouseMoveEvent(QMouseEvent * event)
{
    int dx0 = (QCursor::pos()-drag_0).x();
    if (dragging && !searching)
    {
        if ((current_element==0 && dx0>50) || (current_element==pages.size()-1 && dx0<-50))
        {
            return;
        }

        QPoint delta = QCursor::pos()-drag_start_position;
        for (PagerItem *page_i : pages)
        {
            page_i->move((page_i->pos()+delta).x(),page_i->pos().y());
        }
        drag_start_position = QCursor::pos();
    }
    event->accept();
}

void Pager::mouseReleaseEvent(QMouseEvent * event)
{
    if (searching)
    {
        event->ignore();
        return;
    }
    int dx0 = (QCursor::pos()-drag_0).x();
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
        animation->setEndValue(QPoint((i-new_element)*m_width,pages[i]->pos().y()));
        animation->setDuration(100);
        animation->start();
    }

    current_element = new_element;
    element_before_searching = current_element;
    dragging = false;
    event->accept();
}

void Pager::activateSearch(const QString &query)
{
    searching = (query!="");
    std::vector<KApplication> found_apps = searchApplication(m_kapplications,query);
    updatePager(found_apps);
    updated(searching);
}
