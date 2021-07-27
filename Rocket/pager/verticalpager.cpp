#include <QPropertyAnimation>
#include <QMouseEvent>
#include <QCursor>
#include <QDebug>
#include <QDir>
#include <QApplication>
#include <QParallelAnimationGroup>
#include <QDropEvent>
#include <QMimeData>
#include <QDrag>
#include <QGraphicsBlurEffect>

#include <KConfig>
#include <KConfigGroup>

#include "pager/verticalpager.h"
#include "tools/searchingapps.h"
#include "icongrid/icongrid.h"

#include "../RocketLibrary/tools/kmenuitems.h"
#include "../RocketLibrary/tools/rocketconfigmanager.h"

VerticalPager::VerticalPager(QWidget *parent) : QWidget(parent)
{    
    // Colouring the background...
    //QPalette palette;
    setAutoFillBackground(true);
    //palette.setColor(QPalette::ColorRole::Background,QColor(255,255,100,100));
    //setPalette(palette);

    setMouseTracking(true);

    // Setting the sizes
    setFixedSize(parent->size());

    m_kapplications = ConfigManager.getApplications();
    m_kapplication_tree = ConfigManager.getApplicationTree();
    m_backgroundView = new QGraphicsView(this);
}

void VerticalPager::constructPager(std::vector<KDEApplication> kapplications)
{
    std::vector<KDEApplication> applications;
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
            if (!searching && ConfigManager.getTightLayoutSetting()==false)
            {
                page->getItemLayout()->setRowStretch(1,page->getIconGrid()->getMaxNumberOfRows());
                page->getIconGrid()->getLayout()->setAlignment(Qt::AlignTop | Qt::AlignLeft);
                page->getIconGrid()->setFixedSize(page->getIconGridMaxSize());
            }
            connect(page->getIconGrid(),&IconGrid::goToPage,this,&VerticalPager::goToPage);

            addItem(page);
            applications.clear();

            applications.push_back(kapplications[i]);
        }
    }

    // Adding the rest..
    PagerItem * page = new PagerItem(this,applications);
    if (!searching && ConfigManager.getTightLayoutSetting()==false)
    {
        page->getItemLayout()->setRowStretch(1,page->getIconGrid()->getMaxNumberOfRows());
        page->getIconGrid()->getLayout()->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        page->getIconGrid()->setFixedSize(page->getIconGridMaxSize());
    }
    connect(page->getIconGrid(),&IconGrid::goToPage,this,&VerticalPager::goToPage);
    addItem(page);
    if (kapplications.size()==1) // only 1 item found after searching
    {
        page->getIconGrid()->highlight(0);
    }

    for (PagerItem * i : pages)
    {
        for (IconGridItem * i2 : i->getIconGrid()->getItems())
        {
            connect(i2->getCanvas(),&IconGridItemCanvas::iconDraggingOn,this,&VerticalPager::iconDraggingOn);
            connect(this,&VerticalPager::enableIconDragging,i2->getCanvas(),&IconGridItemCanvas::setDraggable);
        }
    }

    QString backgroundPath = QDir::homePath()+"/.config/rocket/wallpaper.jpeg";
    if (ConfigManager.getUsingSystemWallpaper())
    {
        KConfig config(QDir::homePath()+"/.config/plasma-org.kde.plasma.desktop-appletsrc");
        QStringList wallpaper_candidates;
        QStringList list = config.group("Containments").groupList();
        for (QString l : list)
        {
            if (config.group("Containments").group(l).groupList().contains("Wallpaper"))
            {
                QString variable = config.group("Containments").group(l).group("Wallpaper").group("org.kde.image").group("General").readEntry("Image");
                if (variable.split("file://").size()>1)
                {
                    wallpaper_candidates.push_back(config.group("Containments").group(l).readEntry("lastScreen")+variable.split("file://")[1]);
                }
                else
                {
                    if (variable.at(variable.size()-1)!="/") wallpaper_candidates.push_back(config.group("Containments").group(l).readEntry("lastScreen")+variable);
                    else qDebug() << "Wallpaper packages are not supported, please pick the image file manually.";
                }
            }
        }
        if (wallpaper_candidates.size()==0)
        {
            qDebug() << "No wallpapers found in /.config/plasma-org.kde.plasma.desktop-appletsrc. Falling back the default one ~/.config/rocket/wallpaper.jpeg.";
        }
        else
        {
            if (ConfigManager.getWallpaperScreen()<wallpaper_candidates.size())
            {
                wallpaper_candidates.sort();
                backgroundPath = wallpaper_candidates[ConfigManager.getWallpaperScreen()].right(wallpaper_candidates[ConfigManager.getWallpaperScreen()].size()-1);
            }
            else {
                qDebug() << "No KDE Wallpaper found for screen "<< ConfigManager.getWallpaperScreen() <<". Using the default one ~/.config/rocket/wallpaper.jpeg.";
            }
        }
    }
    m_backgroundView->setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    m_backgroundView->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    m_backgroundView->setEnabled(false);
    m_backgroundView->setFrameShape(QFrame::Shape::NoFrame);
    m_backgroundView->setAutoFillBackground(true);
    m_backgroundView->viewport()->setAutoFillBackground(true);

    //this shift is needed due to scroll animation remnants at the widget's border
    int delta = size().height()*0.1;

    m_backgroundView->setScene(new QGraphicsScene(geometry()));
    m_backgroundView->setFixedSize(size().width(),size().height()+delta);
    m_backgroundView->setAlignment(Qt::AlignCenter);
    QPixmap bkgnd(backgroundPath);
    bkgnd = bkgnd.scaled(size(), Qt::IgnoreAspectRatio);
    m_backgroundView->setBackgroundBrush(QBrush(bkgnd));
    if (!ConfigManager.getBoxSetting()) m_backgroundView->setForegroundBrush(QBrush(ConfigManager.getBaseColour()));

    QGraphicsBlurEffect* p_blur = new QGraphicsBlurEffect;
    p_blur->setBlurRadius(ConfigManager.getBlurRadius());
    p_blur->setBlurHints(QGraphicsBlurEffect::BlurHint::QualityHint);
    m_backgroundView->setGraphicsEffect(p_blur);
    m_backgroundView->move(0,-delta/2);
}

void VerticalPager::updatePager(std::vector<KDEApplication> kapplications)
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

    if (kapplications == m_kapplication_tree)
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
        pages[i]->setGeometry(QRect(0,(i-current_element)*height(),width(),height()));
        pages[i]->setEnabled(true);
        pages[i]->setVisible(true);
    }
}

void VerticalPager::addItem(PagerItem * page)
{
    pages.push_back(page);
}


void VerticalPager::goToPage(int deltaPage)
{
    if (current_element+deltaPage==getNumberOfElements() || current_element+deltaPage==-1) return;
    if (searching) return;
    int newpage = current_element+deltaPage;
    for (int i=0;i<pages.size(); i++)
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
    element_before_searching = current_element;
    page_turned = true;
}

void VerticalPager::resizeEvent(QResizeEvent *event)
{
    // if not in fullscreen, don't bother constructing the pager...
    if (parentWidget()->size().width()<17)
    {
        event->accept();
        return;
    }
    updatePager(m_kapplication_tree);
    for (int i=0;i<pages.size();i++)
    {
        pages[i]->resizeEvent(event);
    }
}

void VerticalPager::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        pages[current_element]->getIconGrid()->resetHighlightAndActiveElement();
        drag_start_position = QCursor::pos();
        drag_0 = QCursor::pos();
        dragging = true;
    }
    e->accept();
    //qDebug() << "pager: mouse press!";
}

void VerticalPager::mouseMoveEvent(QMouseEvent * event)
{
    //qDebug() << "Pagermousemove with dragging:" << dragging;
    if (m_icon_dragging_on)
    {
        dragging = false;
        qDebug() << "    icondragging on, thus dragging:" << dragging;
        event->accept();
        return;
    }
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
            if ((current_element==0 && dy0>RocketStyle::pager_deadzone_threshold) || (current_element==pages.size()-1 && dy0<-RocketStyle::pager_deadzone_threshold))
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
}

void VerticalPager::mouseReleaseEvent(QMouseEvent * event)
{
    if (m_icon_dragging_on)
    {
        event->ignore();
        return;
    }
    // Checking whether outside area is clicked; if it's the case, close the app
    int dx0 = (QCursor::pos()-drag_0).x();
    int dy0 = (QCursor::pos()-drag_0).y();
    if (dx0*dx0+dy0*dy0<RocketStyle::click_tolerance && event->button()==Qt::LeftButton)
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
    if (dy0>swipe_decision_threshold && current_element!=0) {
        new_element = current_element-1;
    }
    if (dy0<-swipe_decision_threshold && current_element!=pages.size()-1){
        new_element = current_element+1;
    }

    QParallelAnimationGroup * animationgroup = new QParallelAnimationGroup;
    for (int i=0;i<pages.size(); i++)
    {
        QPropertyAnimation * animation = new QPropertyAnimation(pages[i],"pos");
        animation->setStartValue(pages[i]->pos());
        animation->setEndValue(QPoint(pages[i]->pos().x(),(i-new_element)*height()));
        animation->setDuration(100);
        animationgroup->addAnimation(animation);
    }
    animationgroup->start();

    pages[new_element]->getIconGrid()->resetHighlightAndActiveElement();
    current_element = new_element;
    element_before_searching = current_element;
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
            int delta = (event->angleDelta().y()==0 ? -(event->angleDelta().x())*ConfigManager.getInvertedScrollFactorXfromSettings() : event->angleDelta().y()*ConfigManager.getInvertedScrollFactorYfromSettings() );
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
                element_before_searching = current_element;
                new_element = current_element;
                for (int i=0;i<pages.size(); i++)
                {
                    pages[i]->move(QPoint(pages[i]->pos().x(),(i-new_element)*height()));
                }
            }
        }
    }
    event->accept();
}

void VerticalPager::finishScrolling()
{
    current_element = (-pages[0]->pos().y()+height()/2)/height();
    element_before_searching = current_element;
    new_element = current_element;
    scrolled = false;
    touchpad = false;
    if (pages[0]->pos().y()!=0 || pages[0]->pos().y()!=-height()*(getNumberOfElements()-1))
    {
        QParallelAnimationGroup * animationgroup = new QParallelAnimationGroup;
        for (int i=0;i<pages.size(); i++)
        {
            QPropertyAnimation * animation = new QPropertyAnimation(pages[i],"pos");
            animation->setStartValue(pages[i]->pos());
            animation->setEndValue(QPoint(pages[i]->pos().x(),(i-new_element)*height()));
            animation->setDuration(100);
            animationgroup->addAnimation(animation);
        }
        animationgroup->start();
    }
}


void VerticalPager::activateSearch(const QString &query)
{
    if (scrolled) // pages have been scrolled
    {
        m_scrolltimeouttimer->stop();
        if (mouse_pos_scroll_0!=QCursor::pos())
        {
            current_element = (-pages[0]->pos().y()+height()/2)/height();
            element_before_searching = current_element;
            new_element = current_element;
            scrolled = false;
        }
    }
    searching = (query!="");
    std::vector<KDEApplication> found_apps;
    if (searching)
       found_apps = searchApplication(m_kapplications,query);
    else found_apps = m_kapplication_tree;
    updatePager(found_apps);
    updated(searching);
    enableIconDragging(!searching);
}
