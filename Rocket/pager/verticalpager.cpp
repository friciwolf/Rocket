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
    setAcceptDrops(true);

    // Setting the sizes
    setFixedSize(parent->size());

    m_kapplications = ConfigManager.getApplications();
    m_kapplication_tree = ConfigManager.getApplicationTree();
    m_backgroundView = new QGraphicsView(this);

    m_timer_hovering_above_elements->setSingleShot(true);
    m_timer_hovering_above_elements->setInterval(250);

    m_timer_drag_switch->setSingleShot(true);
    connect(m_timer_drag_switch,&QTimer::timeout,this,[=]{
        m_timer_drag_mouse_pos = QCursor::pos();

        current_element = current_element + m_timer_drag_delta;
        element_before_entering_submenu = current_element;
        new_element = current_element;
        scrolled = false;
        touchpad = false;
        QParallelAnimationGroup * animationgroup = new QParallelAnimationGroup;
        for (int i=0;i<pages.size(); i++)
        {
            QPropertyAnimation * animation = new QPropertyAnimation(pages[i],"pos");
            animation->setStartValue(pages[i]->pos());
            animation->setEndValue(QPoint(pages[i]->pos().x(),(i-new_element)*height()));
            animation->setDuration(100);
            animationgroup->addAnimation(animation);
        }
        connect(animationgroup,&QParallelAnimationGroup::finished,this,[=]{
            IconGridItem * minimum_distance = findGridItemOfMinimumDistance(QCursor::pos());

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

    for (IconGridItem * i : getAllIconGridItems())
    {
        connect(i->getCanvas(),&IconGridItemCanvas::makeFolder,this,&VerticalPager::makeFolder);
        connect(i->getCanvas(),&IconGridItemCanvas::folderClickEvent,this,&VerticalPager::folderClickEvent);
        connect(i->getCanvas(),&IconGridItemCanvas::enterIconDraggingMode,this,&VerticalPager::enterIconDraggingMode);
        connect(this,&VerticalPager::enableIconDragging,i->getCanvas(),&IconGridItemCanvas::setDraggable);
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
        current_element = element_before_entering_submenu;
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

IconGridItem * VerticalPager::findGridItemOfMinimumDistance(QPoint referencePoint)
{
    IconGridItem * item;
    int dist = -1;
    for (IconGridItem * i : pages[current_element]->getIconGrid()->getItems())
    {
        QPoint delta = i->mapToGlobal(i->getCanvas()->geometry().center())-referencePoint;
        int d = delta.x()*delta.x() + delta.y() * delta.y();
        if (dist == -1 || d<dist)
        {
            dist = d;
            item = i;
        }
    }
    return item;
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
    element_before_entering_submenu = current_element;
    page_turned = true;
}

std::vector<IconGridItem*> VerticalPager::getAllIconGridItems()
{
    std::vector<IconGridItem*> res;
    for (PagerItem * page : pages)
        for (IconGridItem * i : page->getIconGrid()->getItems())
            res.push_back(i);
    return res;
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
    if (isIconDraggingOn()) enterIconDraggingMode(false);
    if (e->button() == Qt::LeftButton)
    {
        pages[current_element]->getIconGrid()->resetHighlightAndActiveElement();
        drag_start_position = QCursor::pos();
        drag_0 = QCursor::pos();
        if (!searching && !in_subfolder) dragging = true;
    }
    e->accept();
    //qDebug() << "pager: mouse press!";
}

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
        if (dragging && !searching && !in_subfolder)
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
        enterIconDraggingMode(false);
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
            if (in_subfolder)
            {
                updatePager(m_kapplication_tree);
                updated(false);
                enableIconDragging(true);
                setSearchbarVisibility(true);
                in_subfolder = false;
            }
            else
                qApp->exit();
        }
    }
    // Don't allow scrolling while searching or while in a subfolder
    if (searching || in_subfolder)
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
    element_before_entering_submenu = current_element;
    dragging = false;
    event->accept();
    //qDebug() << "pager: mouse up!";
}

void VerticalPager::wheelEvent(QWheelEvent *event)
{
    pages[current_element]->getIconGrid()->resetHighlightAndActiveElement();
    if (!searching && !in_subfolder)
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
                element_before_entering_submenu = current_element;
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

void VerticalPager::dragEnterEvent(QDragEnterEvent *event)
{
    m_timer_hovering_above_elements->start();
    event->accept();
}

void VerticalPager::dragMoveEvent(QDragMoveEvent *event)
{
    if (pages[current_element]->getIconGrid()->geometry().contains(event->pos()))
    {
        m_timer_drag_switch->stop();
        m_timer_drag_delta = 0;
    }
    // the item the cursor has minimum distance to
    IconGridItem * minimum_distance = findGridItemOfMinimumDistance(mapToGlobal(event->pos()));

    if(!(minimum_distance->getApplication()==m_item_dragged->getApplication()) && m_drag_animation->currentTime()==m_drag_animation->duration() && !m_timer_hovering_above_elements->isActive())
    {
        m_drag_animation = new QParallelAnimationGroup;

        std::vector<KDEApplication> newtree;
        std::vector<QPoint> item_positions;
        int maxicons = ConfigManager.getRowNumber()*ConfigManager.getColumnNumber();
        for (PagerItem * page : pages)
        {
            // this one fixes the size of the grids, so they don't "collapse" while being rearranged
            page->getIconGrid()->setFixedSize(page->getIconGrid()->size());
            for (IconGridItem * i : page->getIconGrid()->getItems())
            {
                // first, save each item's old positions for the animation...
                item_positions.push_back(i->pos());

                // then, construct the new application tree
                if (i->getCanvas()==m_item_dragged) continue;
                if (i->getApplication()==minimum_distance->getApplication())
                {
                    std::vector<QPoint> subvector(item_positions.begin()+maxicons*current_element,item_positions.end());
                    if (std::find(subvector.begin(),subvector.end(),m_item_dragged->parentWidget()->pos())
                            !=subvector.end())
                    {
                        newtree.push_back(minimum_distance->getApplication());
                        newtree.push_back(m_item_dragged->getApplication());
                        continue;
                    }
                    else
                    {
                        newtree.push_back(m_item_dragged->getApplication());
                        newtree.push_back(minimum_distance->getApplication());
                        continue;
                    }
                }
                newtree.push_back(i->getApplication());
            }
        }
        if (newtree!=m_kapplication_tree)
        {
            std::vector<IconGridItem*> items = getAllIconGridItems();
            std::vector<IconGridItem*> newitems_in_row;

            QPropertyAnimation * panim;

            for (int i=0;i<newtree.size();i++)
            {
                for (IconGridItem * item : items)
                {
                    if (newtree[i] == item->getApplication())
                    {
                        newitems_in_row.push_back(item);

                        item->setParent(pages[i/maxicons]->getIconGrid());

                        panim = new QPropertyAnimation(item,"pos");
                        panim->setStartValue(item->pos());
                        panim->setEndValue(item_positions[i]);
                        panim->setDuration(200);
                        connect(panim,&QPropertyAnimation::finished,item,[=]{
                            pages[i/maxicons]->getIconGrid()->getLayout()->addWidget(item,i/ConfigManager.getColumnNumber(),i%ConfigManager.getColumnNumber());
                        });
                        m_drag_animation->addAnimation(panim);
                        break;
                    }
                }
            }

            m_kapplication_tree = newtree;

            int i=0;
            for (PagerItem * page : pages)
            {
                std::vector<IconGridItem*> vector;
                if ((i+1)*maxicons<newitems_in_row.size())
                {
                    vector = std::vector<IconGridItem*>(newitems_in_row.begin()+i*maxicons,newitems_in_row.begin()+(i+1)*maxicons);
                    page->getIconGrid()->setItems(vector);
                }
                else {
                    vector = std::vector<IconGridItem*>(newitems_in_row.begin()+i*maxicons,newitems_in_row.end());
                    page->getIconGrid()->setItems(vector);
                }
                i++;
            }

            m_drag_animation->start();
        }
    }
    else
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
    event->accept();
}

void VerticalPager::dropEvent(QDropEvent *event)
{
    enterIconDraggingMode(false);
    bool applistChanged = false;
    if (m_kapplication_tree.size()!=ConfigManager.getApplicationTree().size()) applistChanged = true;
    else {
        for (int i = 0;i<m_kapplication_tree.size();i++)
        {
            if (!(m_kapplication_tree[i] == ConfigManager.getApplicationTree()[i]))
            {
                applistChanged = true;
                break;
            }
        }
    }
    if (applistChanged)
    {
        ConfigManager.generateAppGridConfigFile(ConfigManager.getAppGridConfig(),m_kapplication_tree);
    }
    event->accept();
}

void VerticalPager::dragLeaveEvent(QDragLeaveEvent *event)
{
    if (m_timer_drag_switch->isActive())
    {
        m_timer_drag_switch->stop();
        m_timer_drag_delta = 0;
    }
    if (QCursor::pos().x()<mapToGlobal(geometry().topLeft()).x() || QCursor::pos().x()>mapToGlobal(geometry().topRight()).x() ||
            QCursor::pos().y()>mapToGlobal(geometry().bottomLeft()).y() || QCursor::pos().y()<mapToGlobal(geometry().topRight()).y())
    {
        qApp->exit();
    }
    m_timer_hovering_above_elements->stop();
    event->accept();
}

void VerticalPager::finishScrolling()
{
    current_element = (-pages[0]->pos().y()+height()/2)/height();
    element_before_entering_submenu = current_element;
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
    m_horizontal_scrolling = 0;
}

void VerticalPager::activateSearch(const QString &query)
{
    if (scrolled) // pages have been scrolled
    {
        m_scrolltimeouttimer->stop();
        if (mouse_pos_scroll_0!=QCursor::pos())
        {
            current_element = (-pages[0]->pos().y()+height()/2)/height();
            element_before_entering_submenu = current_element;
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

void VerticalPager::enterIconDraggingMode(bool on, IconGridItemCanvas * canvas)
{
    m_icon_dragging_on=on;
    dragging=false;
    if (on && m_drag_animation->currentTime()==m_drag_animation->duration())
    {
        m_item_dragged = canvas;
    }
    if (on==false && canvas==nullptr)
    {
        // an item has been dropped on canvas, or the mouse has moved a lot during a long a click
        ConfigManager.generateAppGridConfigFile(ConfigManager.getAppGridConfig(),m_kapplication_tree);
    }
}

void VerticalPager::folderClickEvent(KDEApplication folder)
{
    element_before_entering_submenu = current_element;
    in_subfolder = true;
    in_subfolder_app = folder;
    dragging=false;
    updatePager(folder.getChildren());
    updated(true);
    enableIconDragging(false);
    setSearchbarVisibility(false);
}

void VerticalPager::makeFolder(KDEApplication app_dropped_on, KDEApplication app_dragged)
{
    QParallelAnimationGroup * paranim = new QParallelAnimationGroup;

    KDEApplication newfolder = app_dropped_on;
    std::vector<KDEApplication> newtree;
    std::vector<QPoint> item_positions;
    std::vector<IconGridItem*> items = getAllIconGridItems();
    int maxicons = ConfigManager.getRowNumber()*ConfigManager.getColumnNumber();
    for (IconGridItem * i : items)
    {
        // first, save each item's old positions for the animation...
        item_positions.push_back(i->pos());

        // then, construct the new application tree
        if (i->getCanvas()->getApplication()==app_dragged)
        {
            i->setVisible(false);
            continue;
        }
        if (i->getApplication()==app_dropped_on)
        {
            newfolder.setChildren(std::vector<KDEApplication>({app_dropped_on,app_dragged}));
            newfolder.setToFolder(true);
            newtree.push_back(newfolder);
            continue;
        }
        newtree.push_back(i->getApplication());
    }

    std::vector<IconGridItem*> newitems_in_row;
    QPropertyAnimation * panim;

    // Adding new positions to the animation group
    for (int i=0;i<newtree.size();i++)
    {
        for (IconGridItem * item : items)
        {
            if (newtree[i] == item->getApplication() || (newtree[i] == newfolder && item->getApplication() == app_dropped_on))
            {
                newitems_in_row.push_back(item);

                item->setParent(pages[i/maxicons]->getIconGrid());

                panim = new QPropertyAnimation(item,"pos");
                panim->setStartValue(item->pos());
                panim->setEndValue(item_positions[i]);
                panim->setDuration(200);
                connect(panim,&QPropertyAnimation::finished,item,[=]{
                    pages[i/maxicons]->getIconGrid()->getLayout()->addWidget(item,i/ConfigManager.getColumnNumber(),i%ConfigManager.getColumnNumber());
                });
                paranim->addAnimation(panim);
                break;
            }
        }
    }

    // updating the app tree
    m_kapplication_tree = newtree;

    connect(paranim,&QParallelAnimationGroup::finished,this,[=]{
        int numberofelementsbefore = this->getNumberOfElements();
        this->updatePager(m_kapplication_tree);
        // move pager to the right position if there is one page less, and we are on the last page
        if (this->getNumberOfElements() != numberofelementsbefore && this->element_before_entering_submenu==numberofelementsbefore-1)
        {
            this->current_element = this->getNumberOfElements()-1;
            this->element_before_entering_submenu = this->current_element;
            this->new_element = this->current_element;
            QParallelAnimationGroup * animationgroup = new QParallelAnimationGroup;
            for (int i=0;i<this->pages.size(); i++)
            {
                QPropertyAnimation * animation = new QPropertyAnimation(pages[i],"pos");
                animation->setStartValue(pages[i]->pos());
                animation->setEndValue(QPoint(pages[i]->pos().x(),(i-new_element)*height()));
                animation->setDuration(100);
                animationgroup->addAnimation(animation);
            }
            animationgroup->start();
        }
    });
    paranim->start();
    enterIconDraggingMode(false);
}
