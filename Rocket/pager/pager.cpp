#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QCursor>
#include <QDebug>
#include <QDir>

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
    KMenuItems m_menuitems;
    m_menuitems.scanElements();

    m_kapplications = m_menuitems.getApplications();
    constructPager(m_kapplications);
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
    pages[current_element]->getIconGrid()->resetHighlightAndActiveElement();
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
        if ((current_element==0 && dx0>RocketStyle::pager_deadzone_threshold) || (current_element==pages.size()-1 && dx0<-RocketStyle::pager_deadzone_threshold))
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

void Pager::activateSearch(const QString &query)
{
    searching = (query!="");
    std::vector<KApplication> found_apps = searchApplication(m_kapplications,query);
    updatePager(found_apps);
    updated(searching);
}
