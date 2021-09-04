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

#include "pager/pager.h"
#include "pager/pagerfolderview.h"
#include "icongrid/icongrid.h"
#include "tools/searchingapps.h"

#include "../RocketLibrary/tools/kmenuitems.h"
#include "../RocketLibrary/tools/rocketconfigmanager.h"

Pager::Pager(QWidget *parent, std::vector<KDEApplication> appTree, bool withBackgound) : QWidget(parent)
{    
    // Colouring the background...
    //QPalette palette;
    setAutoFillBackground(true);
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
    m_timer_hovering_above_elements->setInterval(250);

    m_timer_drag_switch->setSingleShot(true);
    connect(m_timer_drag_switch,&QTimer::timeout,this,[=]{
        m_timer_drag_mouse_pos = QCursor::pos();

        current_element = current_element + m_timer_drag_delta;
        element_before_entering_submenu = (!in_subfolder ? current_element : element_before_entering_submenu);
        new_element = current_element;
        scrolled = false;
        touchpad = false;
        QParallelAnimationGroup * animationgroup = new QParallelAnimationGroup;
        for (int i=0;i<pages.size(); i++)
        {
            QPropertyAnimation * animation = new QPropertyAnimation(pages[i],"pos");
            animation->setStartValue(pages[i]->pos());
            animation->setEndValue(QPoint((i-new_element)*width(),pages[i]->pos().y()));
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

void Pager::constructPager(std::vector<KDEApplication> kapplications)
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
                page->getIconGrid()->setGeometry(page->getIconGrid()->x(),page->getIconGrid()->y(),page->getIconGridMaxSize().width(),page->getIconGridMaxSize().height());
            }
            connect(page->getIconGrid(),&IconGrid::goToPage,this,&Pager::goToPage);

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
        page->getIconGrid()->setGeometry(page->getIconGrid()->x(),page->getIconGrid()->y(),page->getIconGridMaxSize().width(),page->getIconGridMaxSize().height());
    }
    connect(page->getIconGrid(),&IconGrid::goToPage,this,&Pager::goToPage);
    addItem(page);
    if (kapplications.size()==1) // only 1 item found after searching
    {
        page->getIconGrid()->highlight(0);
    }

    for (IconGridItem * i : getAllIconGridItems())
    {
        connect(i->getCanvas(),&IconGridItemCanvas::makeFolder,this,&Pager::makeFolder);
        connect(i->getCanvas(),&IconGridItemCanvas::folderClickEvent,this,&Pager::folderClickEvent);
        connect(i->getCanvas(),&IconGridItemCanvas::enterIconDraggingMode,this,&Pager::enterIconDraggingMode);
        connect(this,&Pager::enableIconDragging,i->getCanvas(),&IconGridItemCanvas::setDraggable);
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
        int delta = size().width()*0.1;

        m_backgroundView->setScene(new QGraphicsScene(geometry()));
        m_backgroundView->setFixedSize(size().width()+delta,size().height()+delta);
        m_backgroundView->setAlignment(Qt::AlignCenter);
        QPixmap bkgnd(backgroundPath);
        bkgnd = bkgnd.scaled(size(), Qt::IgnoreAspectRatio);
        m_backgroundView->setBackgroundBrush(QBrush(bkgnd));;
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

IconGridItem * Pager::findGridItemOfMinimumDistance(QPoint referencePoint)
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
    element_before_entering_submenu = (!in_subfolder ? current_element : element_before_entering_submenu);
    page_turned = true;
}

std::vector<IconGridItem*> Pager::getAllIconGridItems()
{
    std::vector<IconGridItem*> res;
    for (PagerItem * page : pages)
        for (IconGridItem * i : page->getIconGrid()->getItems())
            res.push_back(i);
    return res;
}

void Pager::startBlurringEffect()
{
    QGraphicsBlurEffect* p_blur = new QGraphicsBlurEffect;
    p_blur->setBlurRadius(50);
    p_blur->setBlurHints(QGraphicsBlurEffect::BlurHint::QualityHint);
    pages[current_element]->setGraphicsEffect(p_blur);

    QGraphicsBlurEffect* p_blur2 = new QGraphicsBlurEffect;
    p_blur2->setBlurRadius(50);
    p_blur2->setBlurHints(QGraphicsBlurEffect::BlurHint::QualityHint);
    m_backgroundView->setGraphicsEffect(p_blur2);

    /* Fancy blurring animation with performace issues
    QParallelAnimationGroup * paranim = new QParallelAnimationGroup;

    QPropertyAnimation * panim2 = new QPropertyAnimation;
    panim2->setTargetObject(pages[current_element]->graphicsEffect());
    panim2->setPropertyName("blurRadius");
    panim2->setStartValue(0);
    panim2->setEndValue(50);
    panim2->setDuration(100);
    panim2->setEasingCurve(QEasingCurve::InQuart);
    paranim->addAnimation(panim2);

    QPropertyAnimation * panim1 = new QPropertyAnimation;
    panim1->setTargetObject(m_backgroundView->graphicsEffect());
    panim1->setPropertyName("blurRadius");
    panim1->setStartValue(0);
    panim1->setEndValue(50);
    panim1->setDuration(100);
    panim1->setEasingCurve(QEasingCurve::InQuart);
    paranim->addAnimation(panim1);

    paranim->start();

    QTimer * timer = new QTimer();
    timer->setInterval(10);
    connect(timer,&QTimer::timeout,pages[current_element],[=]{
       pages[current_element]->update();
        if (paranim->state()==QPropertyAnimation::State::Stopped)
        {
            timer->stop();
            delete timer;
            delete panim1;
            delete panim2;
            delete paranim;
        }
    });
    timer->start();
    */
}

void Pager::resizeEvent(QResizeEvent *event)
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

void Pager::mouseMoveEvent(QMouseEvent * event)
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
        // If the user clicks outside of the icongrid, and not in a subfolder, close the app
        if (!pages[current_element]->getIconGrid()->geometry().contains(event->pos()))
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
    // Scroll if neccessary and update
    new_element = current_element;
    if (dx0>swipe_decision_threshold && current_element!=0) {
        new_element = current_element-1;
    }
    if (dx0<-swipe_decision_threshold && current_element!=pages.size()-1){
        new_element = current_element+1;
    }

    QParallelAnimationGroup * animationgroup = new QParallelAnimationGroup;
    for (int i=0;i<pages.size(); i++)
    {
        QPropertyAnimation * animation = new QPropertyAnimation(pages[i],"pos");
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

void Pager::wheelEvent(QWheelEvent *event)
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
                connect(m_scrolltimeouttimer,&QTimer::timeout,this,&Pager::finishScrolling);
                m_scrolltimeouttimer->setSingleShot(true);
                m_scrolltimeouttimer->start(250);
            }
            else {
                // we've reached the last/first page, don't do anything anymore
                current_element = (-pages[0]->pos().x()+width()/2)/width();
                element_before_entering_submenu = (!in_subfolder ? current_element : element_before_entering_submenu);
                new_element = current_element;
                for (int i=0;i<pages.size(); i++)
                {
                    pages[i]->move(QPoint((i-new_element)*width(),pages[i]->pos().y()));
                }
            }
        }
    }
    event->accept();
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
    else
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
            else
            {
                if (in_subfolder)
                {
                    //qDebug() << "closing folder" << in_subfolder_app.name();
                }
            }
        }
    }
    event->accept();
}

void Pager::dropEvent(QDropEvent *event)
{
    enterIconDraggingMode(false);
    if (!in_subfolder)
    {
        ConfigManager.generateAppGridConfigFile(ConfigManager.getAppGridConfig(),m_kapplication_tree);
    }
    else
    {
        QStringList indices_string = event->mimeData()->text().split(";");
        std::vector<int> indices({indices_string[0].toInt(),indices_string[1].toInt()});
        std::vector<KDEApplication> newtree = ConfigManager.getApplicationTree();
        newtree[indices[0]].setChildren(m_kapplication_tree);
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

void Pager::finishScrolling()
{
    current_element = (-pages[0]->pos().x()+width()/2)/width();
    element_before_entering_submenu = (!in_subfolder ? current_element : element_before_entering_submenu);
    new_element = current_element;
    scrolled = false;
    touchpad = false;
    if (pages[0]->pos().x()!=0 || pages[0]->pos().x()!=-width()*(getNumberOfElements()-1))
    {
        QParallelAnimationGroup * animationgroup = new QParallelAnimationGroup;
        for (int i=0;i<pages.size(); i++)
        {
            QPropertyAnimation * animation = new QPropertyAnimation(pages[i],"pos");
            animation->setStartValue(pages[i]->pos());
            animation->setEndValue(QPoint((i-new_element)*width(),pages[i]->pos().y()));
            animation->setDuration(100);
            animationgroup->addAnimation(animation);
        }
        animationgroup->start();
    }
    m_horizontal_scrolling = 0;
}

void Pager::activateSearch(const QString &query)
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
    searching = (query!="");
    std::vector<KDEApplication> found_apps;
    if (searching)
        found_apps = searchApplication(m_kapplications,query);
    else found_apps = m_kapplication_tree;
    updatePager(found_apps);
    updated(searching);
    enableIconDragging(!searching);
}

void Pager::enterIconDraggingMode(bool on, IconGridItemCanvas * canvas)
{
    m_icon_dragging_on=on;
    dragging=false;
    if (on && m_drag_animation->currentTime()==m_drag_animation->duration())
    {
        m_item_dragged = canvas;
    }
    if (on==false && canvas==nullptr && !in_subfolder)
    {
        // an item has been dropped on a canvas (folder creation), or the mouse has moved a lot during a long a click
        ConfigManager.generateAppGridConfigFile(ConfigManager.getAppGridConfig(),m_kapplication_tree);
    }
}

void Pager::folderClickEvent(KDEApplication folder)
{
    element_before_entering_submenu = current_element;
    in_subfolder = true;
    in_subfolder_app = folder;
    dragging=false;
    updated(true);
    setSearchbarVisibility(false);
    startBlurringEffect();

    PagerFolderView * folderview = new PagerFolderView(this,folder.getChildren());
    connect(folderview,&PagerFolderView::leavingPagerFolderView,this,[=]{
        folderview->hide();
        this->updated(false);
        this->enableIconDragging(true);
        this->setSearchbarVisibility(true);
        this->in_subfolder = false;
        this->dragging = false;
        // setting the new tree
        this->m_kapplication_tree = ConfigManager.getApplicationTree();
        this->updatePager(this->m_kapplication_tree);
    });
    folderview->show();
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
                QPropertyAnimation * animation = new QPropertyAnimation(this->pages[i],"pos");
                animation->setStartValue(this->pages[i]->pos());
                animation->setEndValue(QPoint((i-this->new_element)*width(),this->pages[i]->pos().y()));
                animation->setDuration(100);
                animationgroup->addAnimation(animation);
            }
            animationgroup->start();
        }
    });
    paranim->start();
    enterIconDraggingMode(false);
}
