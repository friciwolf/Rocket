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

#include "pager/pager.h"
#include "icongrid/icongrid.h"
#include "tools/searchingapps.h"
#include "pager/pagerfolderview.h"

#include "../RocketLibrary/tools/kmenuitems.h"
#include "../RocketLibrary/tools/rocketconfigmanager.h"

Pager::Pager(QWidget *parent, std::vector<KDEApplication> appTree, bool withBackgound) : QWidget(parent)
{
    // Colouring the background...
    //QPalette palette;
    //setAutoFillBackground(true);
    //palette.setColor(QPalette::ColorRole::Background,QColor(255,255,100,100));
    //setPalette(palette);

    setMouseTracking(true);
    setAcceptDrops(true);

    // Setting the sizes
    setGeometry(x(),y(),parent->width(),parent->height());

    m_kapplications = ConfigManager.getApplications();
    m_kapplication_tree = appTree;
    m_withbackground = withBackgound;
    if (withBackgound)
        m_backgroundView = new QGraphicsView(this);

    m_timer_hovering_above_elements->setSingleShot(true);
    m_timer_hovering_above_elements->setInterval(500);

    m_timer_drag_switch->setSingleShot(true);
}

void Pager::constructPager(std::vector<KDEApplication> kapplications)
{
    std::vector<KDEApplication> applications;
    int maxicons = ConfigManager.getRowNumber()*ConfigManager.getColumnNumber();

    // Filling up the grid...
    for (int i=0;i<(int)kapplications.size();i++)
    {
        if ((i%maxicons)!=0 || i==0)
        {
            applications.push_back(kapplications[i]);
        }
        else
        {
            PagerItem * page = new PagerItem(this,applications);
            if (!searching && ConfigManager.getTightLayoutSetting()==false && !in_subfolder)
            {
                page->getItemLayout()->setRowStretch(1,page->getIconGrid()->getMaxNumberOfRows());
                page->getIconGrid()->getLayout()->setAlignment(Qt::AlignTop | Qt::AlignLeft);
                page->getIconGrid()->setGeometry(page->getIconGrid()->x(),page->getIconGrid()->y(),page->getIconGridMaxSize().width(),page->getIconGridMaxSize().height());
            }

            pages.push_back(page);
            applications.clear();

            applications.push_back(kapplications[i]);
        }
    }

    // Adding the rest..
    PagerItem * page = new PagerItem(this,applications);
    if (!searching && ConfigManager.getTightLayoutSetting()==false && !in_subfolder)
    {
        page->getItemLayout()->setRowStretch(1,page->getIconGrid()->getMaxNumberOfRows());
        page->getIconGrid()->getLayout()->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        page->getIconGrid()->setFixedSize(page->getIconGridMaxSize());
    }
    pages.push_back(page);
    if (kapplications.size()==1) // only 1 item found after searching
    {
        page->getIconGrid()->highlight(0);
    }

    if (m_withbackground)
    {
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
        m_backgroundView->setFixedSize(size().width()+delta,size().height()+delta);
        m_backgroundView->setAlignment(Qt::AlignCenter);
        QPixmap bkgnd(backgroundPath);
        bkgnd = bkgnd.scaled(size(), Qt::IgnoreAspectRatio);
        m_backgroundView->setBackgroundBrush(QBrush(bkgnd));
        if (!ConfigManager.getBoxSetting()) m_backgroundView->setForegroundBrush(QBrush(ConfigManager.getBaseColour()));

        QGraphicsBlurEffect* p_blur = new QGraphicsBlurEffect;
        p_blur->setBlurRadius(ConfigManager.getBlurRadius());
        p_blur->setBlurHints(QGraphicsBlurEffect::BlurHint::QualityHint);
        m_backgroundView->setGraphicsEffect(p_blur);
        m_backgroundView->move(-delta/2,-delta/2);
    }
}

void Pager::updatePager(std::vector<KDEApplication> kapplications)
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
}

IconGridItem * Pager::findGridItemOfMinimumDistance(QPoint referencePoint)
{
    IconGridItem * item = nullptr;
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

std::vector<IconGridItem*> Pager::getAllIconGridItems()
{
    std::vector<IconGridItem*> res;
    for (PagerItem * page : pages)
        for (IconGridItem * i : page->getIconGrid()->getItems())
            res.push_back(i);
    return res;
}

// ***********************************************
//                    EVENTS
// ***********************************************

void Pager::resizeEvent(QResizeEvent *event)
{
    // if not in fullscreen, don't bother constructing the pager...
    if (parentWidget()->size().width()<17)
    {
        event->accept();
        return;
    }
    updatePager(m_kapplication_tree);
    for (int i=0;i<(int)pages.size();i++)
    {
        pages[i]->resizeEvent(event);
    }
}

void Pager::leaveEvent(QEvent *event)
{
    if (QCursor::pos().x()<mapToGlobal(geometry().topLeft()).x() || QCursor::pos().x()>mapToGlobal(geometry().topRight()).x() ||
            QCursor::pos().y()>mapToGlobal(geometry().bottomLeft()).y() || QCursor::pos().y()<mapToGlobal(geometry().topRight()).y())
    {
        qDebug() << geometry() << mapToGlobal(geometry().topLeft()) << pages[0]->getIconGrid()->getItems()[0]->getApplication().name();
        qDebug() << "PagerLeaveEvent: exiting...";
        qApp->exit();
    }
    event->accept();
}

void Pager::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton)
    {
        pages[current_element]->getIconGrid()->resetHighlightAndActiveElement();
        drag_start_position = QCursor::pos();
        drag_0 = QCursor::pos();
        if (!searching) dragging = true;
    }
    e->accept();
    //qDebug() << "pager: mouse press!";
}

void Pager::mouseReleaseEvent(QMouseEvent *event)
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
        // If the user clicks outside of the icongrid or
        // inside of the grid but not above an element,
        // and is not in a subfolder, close the app
        if (!pages[current_element]->getIconGrid()->geometry().contains(event->pos()))
        {
            event->accept();
            if (!in_subfolder)
                qApp->exit();
        }
        bool clickedAboveIconGridItem = false;
        for (IconGridItem * i : pages[current_element]->getIconGrid()->getItems())
            if ((i->geometry().translated(pages[current_element]->getIconGrid()->geometry().topLeft())).contains(event->pos()))
            {
                clickedAboveIconGridItem = true;
                break;
            }
        if (!clickedAboveIconGridItem)
        {
            event->accept();
            if (!in_subfolder)
                qApp->exit();
        }
    }
    // Don't allow scrolling while searching
    if (searching)
    {
        event->ignore();
        return;
    }
}

void Pager::dragEnterEvent(QDragEnterEvent *event)
{
    m_timer_hovering_above_elements->start();
    event->accept();
}

void Pager::dragMoveEvent(QDragMoveEvent *event)
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
        bool leftOfMinimumDistance = (mapToGlobal(event->pos()).x()<pages[current_element]->getIconGrid()->mapToGlobal(minimum_distance->geometry().center()).x());
        bool itemFound = false;

        for (PagerItem * page : pages)
        {
            // this one fixes the size of the grids, so they don't "collapse" while being rearranged
            page->getIconGrid()->setFixedSize(page->getIconGrid()->size());
            for (IconGridItem * i : page->getIconGrid()->getItems())
            {
                // first, save each item's old positions for the animation...
                item_positions.push_back(i->pos());

                // then, construct the new application tree
                if (i->getCanvas()->getApplication()==m_item_dragged->getApplication())
                {
                    itemFound = true;
                    continue;
                }
                if (i->getApplication()==minimum_distance->getApplication())
                {
                    // finding the right place for the dragged icon...
                    bool condition = leftOfMinimumDistance;
                    switch (item_positions.size()%ConfigManager.getColumnNumber()) {
                        case 0: // left side
                        {
                            if(!itemFound && !leftOfMinimumDistance)
                                condition=!leftOfMinimumDistance;
                            break;
                        }
                        case 1: // right side
                        {
                            if(itemFound && leftOfMinimumDistance) condition=!leftOfMinimumDistance;
                        }
                        default:
                            break;
                    }
                    if (!condition)
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

        std::vector<IconGridItem*> items = getAllIconGridItems();
        std::vector<IconGridItem*> newitems_in_row;

        QPropertyAnimation * panim;

        for (int i=0;i<(int)newtree.size();i++)
        {
            // Look for the IconGridItems in the newtree to create an animation
            for (IconGridItem * item : items)
            {
                if (newtree[i] == item->getApplication())
                {
                    newitems_in_row.push_back(item);

                    item->setParent(pages[i/maxicons]->getIconGrid());

                    panim = new QPropertyAnimation(item,"pos");
                    panim->setTargetObject(item);
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
            // If an element is being pulled out from a folder, add it to the tree, and to the grid
            if (event->mimeData()->text().split(";")[1].toInt()!=-1 && newtree[i]==m_item_dragged->getApplication() && (int)newitems_in_row.size()-1!=i)
            {
                IconGridItem * item = new IconGridItem(pages[i/maxicons]->getIconGrid(),m_item_dragged->getApplication(),pages[i/maxicons]->getIconGridItemSize());
                connect(item->getCanvas(),&IconGridItemCanvas::enterIconDraggingMode,this,&Pager::enterIconDraggingMode);

                item->show();
                newitems_in_row.push_back(item);

                panim = new QPropertyAnimation(item,"pos");
                panim->setTargetObject(item);
                panim->setStartValue(getAllIconGridItems()[event->mimeData()->text().split(";")[0].toInt()]->pos());
                panim->setEndValue((i<(int)getAllIconGridItems().size() ? getAllIconGridItems()[i]->pos() : getAllIconGridItems()[i-1]->pos()));
                panim->setDuration(200);
                connect(panim,&QPropertyAnimation::finished,item,[=]{
                    pages[i/maxicons]->getIconGrid()->getLayout()->addWidget(item,i/ConfigManager.getColumnNumber(),i%ConfigManager.getColumnNumber());
                });
                m_drag_animation->addAnimation(panim);
            }
        }

        m_kapplication_tree = newtree;

        int i=0;
        for (PagerItem * page : pages)
        {
            std::vector<IconGridItem*> vector;
            if ((i+1)*maxicons<(int)newitems_in_row.size())
            {
                vector = std::vector<IconGridItem*>(newitems_in_row.begin()+i*maxicons,newitems_in_row.begin()+(i+1)*maxicons);
                page->getIconGrid()->setItems(vector);
            }
            else
            {
                vector = std::vector<IconGridItem*>(newitems_in_row.begin()+i*maxicons,newitems_in_row.end());
                page->getIconGrid()->setItems(vector);
            }
            i++;
        }

        m_drag_animation->start();
    }
    else
        setTimerDragDelta(event);
    event->accept();
}

void Pager::dropEvent(QDropEvent *event)
{
    QStringList i = event->mimeData()->text().split(";");
    if (!in_subfolder)
    {
        // Moving an item from a folder to the frontpage
        if (i[1].toInt()!=-1)
        {
            std::vector<KDEApplication> children;
            for (KDEApplication child : ConfigManager.getApplicationTree()[i[0].toInt()].getChildren())
                if (!(child==m_item_dragged->getApplication()))
                    children.push_back(child);
            std::vector<int> i2 = searchApplicationTree(m_kapplication_tree,ConfigManager.getApplicationTree()[i[0].toInt()]);
            m_kapplication_tree[i2[0]].setChildren(children);
            m_folder_view_window->close();
        }
        updatePager(m_kapplication_tree);
        enterIconDraggingMode(false);
    }
    else
    {
        // An item has been dropped on the pager within a folder
        std::vector<KDEApplication> newtree = ConfigManager.getApplicationTree();
        newtree[i[0].toInt()].setChildren(m_kapplication_tree);
        updatePager(m_kapplication_tree);
        ConfigManager.generateAppGridConfigFile(ConfigManager.getAppGridConfig(),newtree);
    }
    event->accept();
}

void Pager::dragLeaveEvent(QDragLeaveEvent *event)
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

// ***********************************************
//                    SLOTS
// ***********************************************

void Pager::folderClickEvent(KDEApplication folder)
{
    element_before_entering_submenu = current_element;
    in_subfolder = true;
    in_subfolder_app = folder;
    dragging=false;
    hideCircularIndicator(true);
    setSearchbarVisibility(false);

    m_folder_view_window = new QMainWindow(this);
    m_folder_view_window->setAttribute(Qt::WA_NoSystemBackground);
    m_folder_view_window->setAttribute(Qt::WA_TranslucentBackground);
    m_folder_view_window->setWindowModality(Qt::WindowModality::WindowModal);

    KWindowEffects::enableBlurBehind(m_folder_view_window->winId());
    QPoint p = mapToGlobal(QPoint(x(),y()));
    m_folder_view_window->setGeometry(QRect(p.x(),p.y(),width(),height()));

    PagerFolderView * folderview = new PagerFolderView(m_folder_view_window,folder.getChildren());
    connect(folderview,&PagerFolderView::leavingPagerFolderView,this,[=]{
        this->hideCircularIndicator(false);
        this->enableIconDragging(true);
        this->setSearchbarVisibility(true);
        this->in_subfolder = false;
        this->dragging = false;
        if (folderview->getCloseFolderTimer()->remainingTimeAsDuration()==std::chrono::milliseconds::zero() || folderview->getCloseFolderTimer()->remainingTime()==-1)
        {
            // setting the new tree, unless the user is dragging an element
            // outside of a folder
            this->m_kapplication_tree = ConfigManager.getApplicationTree();
            this->updatePager(this->m_kapplication_tree);
        }
    });

    m_folder_view_window->setCentralWidget(folderview);
    m_folder_view_window->showFullScreen();

    QRect start;
    std::vector<IconGridItem*> array;
    array = getAllIconGridItems();

    for (IconGridItem * i : array)
        if (i->getApplication().getChildren()==folder.getChildren())
        {
            QPoint point = m_folder_view_window->mapFromGlobal(i->mapToGlobal(i->getCanvas()->geometry().center()));
            start=QRect(point.x(),point.y(),0,0);
            break;
        }

    QParallelAnimationGroup * paranim = new QParallelAnimationGroup(folderview);
    QPropertyAnimation * panim = new QPropertyAnimation(folderview);
    panim->setTargetObject(folderview);
    panim->setPropertyName("geometry");
    panim->setStartValue(start);
    panim->setEndValue(QRect(x(),y(),width(),height()));
    panim->setDuration(300);
    panim->setEasingCurve(QEasingCurve::InQuart);
    paranim->addAnimation(panim);

    QPropertyAnimation * panim2 = new QPropertyAnimation(folderview->getNameField());
    panim2->setTargetObject(folderview->getNameField());
    panim2->setPropertyName("geometry");
    panim2->setStartValue(start);
    panim2->setEndValue(folderview->getNameField()->geometry());
    panim2->setDuration(300);
    panim2->setEasingCurve(QEasingCurve::InQuart);
    paranim->addAnimation(panim2);

    QPropertyAnimation * panim3 = new QPropertyAnimation(m_folder_view_window);
    panim3->setTargetObject(m_folder_view_window);
    panim3->setPropertyName("windowOpacity");
    panim3->setStartValue(0);
    panim3->setEndValue(1);
    panim3->setDuration(300);
    panim3->setEasingCurve(QEasingCurve::InQuart);
    paranim->addAnimation(panim3);

    connect(paranim,&QPropertyAnimation::finished,folderview,[=] {
        if (QCursor::pos().x()<mapToGlobal(geometry().topLeft()).x() || QCursor::pos().x()>mapToGlobal(geometry().topRight()).x() ||
                QCursor::pos().y()>mapToGlobal(geometry().bottomLeft()).y() || QCursor::pos().y()<mapToGlobal(geometry().topRight()).y())
        {
            qDebug() << "folderviewanimationfinished: exiting...";
            qApp->exit();
        }
    });

    folderview->setWindowAnimation(paranim);
    folderview->getWindowAnimation()->start();
}

void Pager::makeFolder(KDEApplication app_dropped_on, KDEApplication app_dragged)
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
            if(app_dropped_on.isFolder())
            {
                std::vector<KDEApplication> children = app_dropped_on.getChildren();
                children.push_back(app_dragged);
                newfolder.setChildren(children);
                newtree.push_back(newfolder);
                continue;
            }
            else
            {
                newfolder.setChildren(std::vector<KDEApplication>({app_dropped_on,app_dragged}));
                newfolder.setToFolder(true);
                newtree.push_back(newfolder);
                continue;
            }
        }
        newtree.push_back(i->getApplication());
    }

    std::vector<IconGridItem*> newitems_in_row;
    QPropertyAnimation * panim;

    // Adding new positions to the animation group
    for (int i=0;i<(int)newtree.size();i++)
    {
        for (IconGridItem * item : items)
        {
            if (newtree[i] == item->getApplication() || (newtree[i] == newfolder && item->getApplication() == app_dropped_on))
            {
                newitems_in_row.push_back(item);

                item->setParent(pages[i/maxicons]->getIconGrid());

                panim = new QPropertyAnimation(item,"pos");
                panim->setTargetObject(item);
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

    connect(paranim,&QParallelAnimationGroup::finished,this,&Pager::updatePagerAndPlayAnimationIfOnePageLess);
    paranim->start();
    enterIconDraggingMode(false);
}

void Pager::enterIconDraggingMode(bool on, IconGridItemCanvas * canvas)
{
    m_icon_dragging_on=on;
    dragging=false;
    if (on && m_drag_animation->currentTime()==m_drag_animation->duration())
    {
        m_item_dragged = canvas;
    }
    if (on==false && canvas!=nullptr && !in_subfolder)
    {
        // an item has been moved outside of a folder
        // and has been dropped on itself (and not on the pager)
        std::vector<KDEApplication> children;
        std::vector<int> i = searchApplicationTree(ConfigManager.getApplicationTree(),canvas->getApplication());
        for (KDEApplication child : ConfigManager.getApplicationTree()[i[0]].getChildren())
            if (!(child==m_item_dragged->getApplication()))
                children.push_back(child);
        std::vector<int> i2 = searchApplicationTree(m_kapplication_tree,ConfigManager.getApplicationTree()[i[0]]);
        m_kapplication_tree[i2[0]].setChildren(children);
        m_folder_view_window->close();
        updatePager(m_kapplication_tree);
        ConfigManager.generateAppGridConfigFile(ConfigManager.getAppGridConfig(),m_kapplication_tree);
    }
    if (on==false && canvas==nullptr && !in_subfolder)
    {
        // an item has been dropped on a canvas (folder creation),
        // or an item has been droppen on the pager
        // or the mouse has moved a lot during a long a click (abort moving item)
        ConfigManager.generateAppGridConfigFile(ConfigManager.getAppGridConfig(),m_kapplication_tree);
    }
    if (on==false && canvas!=nullptr && in_subfolder)
    {
        // an item has been dropped on another canvas in a folder
        std::vector<KDEApplication> newtree = ConfigManager.getApplicationTree();
        std::vector<int> i = searchApplicationTree(ConfigManager.getApplicationTree(),canvas->getApplication());
        newtree[i[0]].setChildren(m_kapplication_tree);
        updatePager(newtree[i[0]].getChildren());
        ConfigManager.generateAppGridConfigFile(ConfigManager.getAppGridConfig(),newtree);
    }
}

void Pager::activateSearch(const QString &query)
{
    searching = (query!="");
    std::vector<KDEApplication> found_apps;
    if (searching)
       found_apps = searchApplication(m_kapplications,query);
    else found_apps = m_kapplication_tree;
    updatePager(found_apps);
    hideCircularIndicator(searching);
    enableIconDragging(!searching);
}
