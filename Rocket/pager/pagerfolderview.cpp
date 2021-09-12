#include <QDebug>
#include <QPalette>
#include <QLineEdit>
#include <QApplication>
#include <QGraphicsView>

#include "pagerfolderview.h"
#include "tools/searchingapps.h"
#include "pagercircularindicator.h"
#include "pagercircularactiveindicator.h"

PagerFolderView::PagerFolderView(QWidget *parent, std::vector<KDEApplication> appTree, bool withBackgound) : Pager(parent, appTree, withBackgound)
{
    /*
     *  Pager Settings
     */
    m_appTree = appTree;
    in_subfolder = true;
    updatePager(appTree);
    if (ConfigManager.getVerticalModeSetting())
    {
        VerticalPager * verticalpager = ((VerticalPager*)parent->parent());
        in_subfolder_app = verticalpager->in_subfolder_app;
    }
    else
    {
        Pager * pager = ((Pager*)parent->parent());
        in_subfolder_app = pager->in_subfolder_app;
    }

    /*
     *  PagerFolderView Specific Initializers
     */
    m_closeFolder = new QTimer(this);
    m_closeFolder->setSingleShot(true);
    m_closeFolder->setInterval(500);
    connect(m_closeFolder,&QTimer::timeout,this,[this]{
        leavingPagerFolderView();
        parentWidget()->hide();
    });

    /*
     *  Folder Name Textfield
     */
    m_folderNameField = new QLineEdit(parent);
    m_folderNameField->setAcceptDrops(false);
    QFont fieldfont = m_folderNameField->font();
    fieldfont.setPointSize(ConfigManager.getFontSize2());
    m_folderNameField->setFont(fieldfont);
    m_folderNameField->setGeometry(pages[0]->getIconGrid()->geometry().left()+30,pages[0]->getIconGrid()->pos().y()-m_folderNameField->height()*2,pages[0]->getIconGrid()->width()-60,50);
    QString rgba = QString::number(ConfigManager.getBaseColour().red())+","+QString::number(ConfigManager.getBaseColour().green())+","+QString::number(ConfigManager.getBaseColour().blue())+","+QString::number(ConfigManager.getBaseColour().alpha());
    m_folderNameField->setStyleSheet(
                "QLineEdit {margin-top:10; padding-left: 30; background: rgba("+rgba+"); border: 0; border-radius: 10;}"
    );
    m_folderNameField->setAlignment(Qt::AlignCenter);
    m_folderNameField->setText(in_subfolder_app.name());

    std::vector<int> res = searchApplicationTree(ConfigManager.getApplicationTree(),in_subfolder_app);
    connect(m_folderNameField,&QLineEdit::textEdited,this,[=]{
        std::vector<KDEApplication> newtree = ConfigManager.getApplicationTree();
        newtree[res[0]].setName(m_folderNameField->text());
        ConfigManager.generateAppGridConfigFile(ConfigManager.getAppGridConfig(),newtree);
    });
    connect(m_folderNameField,&QLineEdit::returnPressed,this,[=]{
        pages[current_element]->getIconGrid()->highlight(0);
        m_folderNameField->clearFocus();
    });
    m_folderNameField->show();

    /*
     *  The Circular Indicators
     */
    PagerCircularIndicator * indicator = new PagerCircularIndicator(this,this);
    indicator->setVerticalPager(nullptr);
    indicator->setPager(this);

    PagerCircularActiveIndicator * activeIndicator = new PagerCircularActiveIndicator(this,indicator);
    activeIndicator->setVerticalPager(nullptr);
    activeIndicator->setPager(this);

    if (pages.size()==1)
    {
        indicator->setVisible(false);
        activeIndicator->setVisible(false);
    }
}


void PagerFolderView::mouseReleaseEvent(QMouseEvent *event)
{
    // Checking whether outside area is clicked; if it's the case, close the folder
    int dx0 = (QCursor::pos()-drag_0).x();
    int dy0 = (QCursor::pos()-drag_0).y();
    if (dx0*dx0+dy0*dy0<RocketStyle::click_tolerance && event->button()==Qt::LeftButton)
    {
        bool clickedAboveIconGridItem = false;
        for (IconGridItem * i : pages[current_element]->getIconGrid()->getItems())
        {
            if ((i->geometry().translated(pages[current_element]->getIconGrid()->geometry().topLeft())).contains(event->pos()))
            {
                clickedAboveIconGridItem = true;
                break;
            }
        }
        // If the user has not clicked inside of the grid AND not not on the folderNameField
        // OR if the user has clicked on an empty area within the icongrid
        // close the folder
        if ((!pages[current_element]->getIconGrid()->geometry().contains(event->pos()) && !m_folderNameField->geometry().contains(event->pos())) || !clickedAboveIconGridItem)
        {
            event->accept();

            QRect end;
            for (IconGridItem * i : ((Pager*)parentWidget()->parentWidget())->getAllIconGridItems())
                if (i->getApplication().getChildren()==m_appTree)
                {
                    QPoint point = mapFromGlobal(i->mapToGlobal(i->getCanvas()->geometry().topLeft()));
                    end=QRect(point.x(),point.y(),i->getCanvas()->width(),i->getCanvas()->height());
                    break;
                }

            m_windowAnimation = new QParallelAnimationGroup(this);
            QPropertyAnimation * panim = new QPropertyAnimation(this);
            panim->setTargetObject(pages[current_element]);
            panim->setPropertyName("geometry");
            panim->setEndValue(end);
            panim->setStartValue(pages[current_element]->geometry());
            panim->setDuration(300);
            panim->setEasingCurve(QEasingCurve::InQuart);
            m_windowAnimation->addAnimation(panim);

            QPropertyAnimation * panim2 = new QPropertyAnimation(this);
            panim2->setTargetObject(m_folderNameField);
            panim2->setPropertyName("geometry");
            panim2->setEndValue(QRect(end.x(),end.y(),0,0));
            panim2->setStartValue(m_folderNameField->geometry());
            panim2->setDuration(300);
            panim2->setEasingCurve(QEasingCurve::InQuart);
            m_windowAnimation->addAnimation(panim2);

            QPropertyAnimation * panim3 = new QPropertyAnimation(window());
            panim3->setTargetObject(window());
            panim3->setPropertyName("windowOpacity");
            panim3->setStartValue(1);
            panim3->setEndValue(0);
            panim3->setDuration(300);
            panim3->setEasingCurve(QEasingCurve::InQuart);
            m_windowAnimation->addAnimation(panim3);

            m_windowAnimation->start();
            connect(m_windowAnimation,&QParallelAnimationGroup::finished,this,[=] {
                leavingPagerFolderView();
                delete parent();
            });
            return;
        }
    }
    Pager::mouseReleaseEvent(event);
}

/*
 * This function disables the dragging animation for folders if there is a single page only.
 */
void PagerFolderView::mouseMoveEvent(QMouseEvent * event)
{
    if (pages.size()==1)
        return;
    else
        Pager::mouseMoveEvent(event);
}

void PagerFolderView::dragMoveEvent(QDragMoveEvent *event)
{
    if (!pages[current_element]->getIconGrid()->geometry().contains(event->pos()) && !m_closeFolder->isActive())
        m_closeFolder->start();
    if (pages[current_element]->getIconGrid()->geometry().contains(event->pos()) && m_closeFolder->isActive())
        m_closeFolder->stop();
    Pager::dragMoveEvent(event);
}

void PagerFolderView::enterIconDraggingMode(bool on, IconGridItemCanvas * canvas)
{
    if (on==false && canvas!=nullptr)
    {
        // an item has been dropped on another canvas in a folder
        std::vector<KDEApplication> newtree = ConfigManager.getApplicationTree();
        std::vector<int> i = searchApplicationTree(ConfigManager.getApplicationTree(),canvas->getApplication());
        newtree[i[0]].setChildren(getApplicationTree());
        updatePager(newtree[i[0]].getChildren());
        ConfigManager.generateAppGridConfigFile(ConfigManager.getAppGridConfig(),newtree);
        return;
    }
    // Propagating the signal back to the parent widget
    if (ConfigManager.getVerticalModeSetting())
        ((VerticalPager*)parentWidget()->parent())->enterIconDraggingMode(on,canvas);
    else
        ((Pager*)parentWidget()->parent())->enterIconDraggingMode(on,canvas);
    Pager::enterIconDraggingMode(on,canvas);
}

void PagerFolderView::leaveEvent(QEvent *event)
{
    // in case the event occurs while opening the new window, filter it
    // until the animation stops
    if (m_windowAnimation->currentTime()!=m_windowAnimation->duration())
        return;
    qDebug() << window()->geometry() << parentWidget()->parentWidget()->geometry();
    Pager::leaveEvent(event);
}
