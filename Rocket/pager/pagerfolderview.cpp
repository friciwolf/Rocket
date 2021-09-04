#include <QDebug>
#include <QGraphicsBlurEffect>
#include <QLabel>
#include <QPalette>
#include <QGraphicsView>
#include <QPropertyAnimation>
#include <QLineEdit>

#include "pagerfolderview.h"
#include "tools/searchingapps.h"
#include "pagercircularindicator.h"
#include "pagercircularactiveindicator.h"

PagerFolderView::PagerFolderView(QWidget *parent, std::vector<KDEApplication> appTree, bool withBackgound) : Pager(parent, appTree, withBackgound)
{
    m_appTree = appTree;
    in_subfolder = true;
    if (ConfigManager.getVerticalModeSetting())
        in_subfolder_app = ((VerticalPager*)parent)->in_subfolder_app;
    else
        in_subfolder_app = ((Pager*)parent)->in_subfolder_app;
    updatePager(appTree);

    QPalette palette;
    setAutoFillBackground(true);
    palette.setColor(QPalette::ColorRole::Background,Qt::transparent);
    setPalette(palette);

    m_folderNameField = new QLineEdit(parentWidget());
    QFont fieldfont = m_folderNameField->font();
    fieldfont.setPointSize(ConfigManager.getFontSize2());
    m_folderNameField->setFont(fieldfont);
    m_folderNameField->setGeometry(m_folderNameField->x(),m_folderNameField->y(),pages[current_element]->getIconGrid()->width()-60,50);
    QString rgba = QString::number(ConfigManager.getBaseColour().red())+","+QString::number(ConfigManager.getBaseColour().green())+","+QString::number(ConfigManager.getBaseColour().blue())+","+QString::number(ConfigManager.getBaseColour().alpha());
    m_folderNameField->setStyleSheet(
                "QLineEdit {margin-top:10; padding-left: 30; background: rgba("+rgba+"); border: 0; border-radius: 10;}"
    );
    m_folderNameField->setAlignment(Qt::AlignCenter);


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

    m_folderNameField->setFixedHeight(50);
    m_folderNameField->show();

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

    QRect start;
    std::vector<IconGridItem*> array;
    if (ConfigManager.getVerticalModeSetting())
        array = ((VerticalPager*)parentWidget())->getAllIconGridItems();
    else
        array = ((Pager*)parentWidget())->getAllIconGridItems();

    for (IconGridItem * i : array)
        if (i->getApplication().getChildren()==appTree)
        {
            QPoint point = i->mapToGlobal(i->getCanvas()->geometry().topLeft());
            start=QRect(point.x(),point.y(),i->getCanvas()->width(),i->getCanvas()->height());
            break;
        }

    QParallelAnimationGroup * paranim = new QParallelAnimationGroup(this);
    QPropertyAnimation * panim = new QPropertyAnimation(this);
    panim->setTargetObject(this);
    panim->setPropertyName("geometry");
    panim->setStartValue(start);
    panim->setEndValue(QRect(0,0,parentWidget()->width(),parentWidget()->height()));
    panim->setDuration(300);
    panim->setEasingCurve(QEasingCurve::InQuart);
    paranim->addAnimation(panim);

    QPropertyAnimation * panim2 = new QPropertyAnimation(this);
    panim2->setTargetObject(m_folderNameField);
    panim2->setPropertyName("geometry");
    panim2->setStartValue(start);
    panim2->setEndValue(QRect(pages[0]->getIconGrid()->geometry().center().x()-m_folderNameField->width()*0.5,pages[0]->getIconGrid()->pos().y()-m_folderNameField->height()-5,m_folderNameField->width(),m_folderNameField->height()));
    panim2->setDuration(300);
    panim2->setEasingCurve(QEasingCurve::InQuart);
    paranim->addAnimation(panim2);

    paranim->start();

    connect(paranim,&QParallelAnimationGroup::finished,m_folderNameField,[=]{
        m_folderNameField->setText(in_subfolder_app.name());
    });
    connect(paranim,&QParallelAnimationGroup::finished,indicator,&PagerCircularIndicator::positioning);
    connect(paranim,&QParallelAnimationGroup::finished,activeIndicator,&PagerCircularActiveIndicator::positioning);
}


void PagerFolderView::mouseReleaseEvent(QMouseEvent *event)
{
    // Checking whether outside area is clicked; if it's the case, close the folder
    int dx0 = (QCursor::pos()-drag_0).x();
    int dy0 = (QCursor::pos()-drag_0).y();
    if (dx0*dx0+dy0*dy0<RocketStyle::click_tolerance && event->button()==Qt::LeftButton)
    {
        if (!pages[current_element]->getIconGrid()->geometry().contains(event->pos()) && !m_folderNameField->geometry().contains(event->pos()))
        {
            event->accept();

            QRect end;
            for (IconGridItem * i : ((Pager*)parentWidget())->getAllIconGridItems())
                if (i->getApplication().getChildren()==m_appTree)
                {
                    QPoint point = i->mapToGlobal(i->getCanvas()->geometry().topLeft());
                    end=QRect(point.x(),point.y(),i->getCanvas()->width(),i->getCanvas()->height());
                    break;
                }

            QParallelAnimationGroup * paranim = new QParallelAnimationGroup(this);
            QPropertyAnimation * panim = new QPropertyAnimation(this);
            panim->setTargetObject(pages[current_element]);
            panim->setPropertyName("geometry");
            panim->setEndValue(end);
            panim->setStartValue(pages[current_element]->geometry());
            panim->setDuration(300);
            panim->setEasingCurve(QEasingCurve::InQuart);
            paranim->addAnimation(panim);

            QPropertyAnimation * panim2 = new QPropertyAnimation(this);
            panim2->setTargetObject(m_folderNameField);
            panim2->setPropertyName("geometry");
            panim2->setEndValue(end);
            panim2->setStartValue(m_folderNameField->geometry());
            panim2->setDuration(300);
            panim2->setEasingCurve(QEasingCurve::InQuart);
            paranim->addAnimation(panim2);

            paranim->start();
            connect(paranim,&QParallelAnimationGroup::finished,this,[=] {
                leavingPagerFolderView();
                delete m_folderNameField;
                delete this;
            });
            return;
        }
    }
    Pager::mouseReleaseEvent(event);
}

void PagerFolderView::mouseMoveEvent(QMouseEvent * event)
{
    if (pages.size()==1)
        return;
    else
        Pager::mouseMoveEvent(event);
}
