#include "pager/pager.h"
#include "icongrid/kmenuitems.h"
#include "icongrid/icongrid.h"

#include <QScrollArea>
#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QCursor>
#include <QDebug>

Pager::Pager(QWidget *parent) : QWidget(parent)
{
    // Colouring the background...
    //QPalette palette;
    //setAutoFillBackground(true);
    //palette.setColor(QPalette::ColorRole::Background,Qt::gray);
    //setPalette(palette);

    // Setting the sizes
    setGeometry(parent->geometry());
    m_width = parent->geometry().width();
    m_height = parent->geometry().height();

    // Getting the entries
    KMenuItems menuitems;
    menuitems.scanElements();

    std::vector<KApplication> kapplications = menuitems.getApplications();


    IconGrid * grid = new IconGrid(this);
    int page_max_icons = grid->getMaxNumberOfRows()*grid->getMaxNumberOfColumns();
    std::vector<KApplication> applications;

    m_number_of_elements = kapplications.size()/page_max_icons +1;

    // Filling up the grid...
    for (int i=0;i<menuitems.getApplications().size();i++)
    {
        if ((i%page_max_icons)!=0 || i==0)
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

void Pager::addItem(PagerItem * page, QGridLayout * layout)
{
    pages.push_back(page);
    pagelayouts.push_back(layout);
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
    if (dragging)
    {
        if (current_element==0 && dx0>50 || current_element==pages.size()-1 && dx0<-50)
        {
            qDebug()<< "stuck at " << current_element;
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
    int dx0 = (QCursor::pos()-drag_0).x();
    if (dx0>swipe_decision_threshold && current_element!=0) {
        new_element = current_element-1;
    }
    if (dx0<-swipe_decision_threshold && current_element!=pages.size()-1){
        new_element = current_element+1;
    }

    qDebug() << "going to element " << new_element;
    for (int i=0;i<pages.size(); i++)
    {
        QPropertyAnimation * animation = new QPropertyAnimation(pages[i],"pos");
        animation->setStartValue(pages[i]->pos());
        animation->setEndValue(QPoint((i-new_element)*m_width,pages[i]->pos().y()));
        animation->setDuration(100);
        animation->start();
    }

    current_element = new_element;
    dragging = false;
    event->accept();
}
