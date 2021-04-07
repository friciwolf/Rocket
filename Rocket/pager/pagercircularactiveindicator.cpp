#include <QPainter>
#include <QDebug>
#include <QColor>

#include "pager/pagercircularactiveindicator.h"

PagerCircularActiveIndicator::PagerCircularActiveIndicator(QWidget *parent, PagerCircularIndicator * indicator) : QWidget(parent)
{
    radius = indicator->getRadius();
    spacing = indicator->getSpacing();
    height = indicator->height;
    width = indicator->width;
    pager = indicator->getPager();
    this->indicator = indicator;
    setGeometry(0,0,width,height);
}

void PagerCircularActiveIndicator::paintEvent(QPaintEvent *event)
{
    int position = -(float)pager->pages[0]->pos().x()/pager->width() * (2*radius+spacing);
    QPainter painter(this);
    painter.setBrush(QBrush(QColor(0,0,0,100),Qt::BrushStyle::SolidPattern));
    //painter.drawEllipse(spacing*0.5+current_item*(radius*2+spacing)+correction,height/2-radius,radius*2,radius*2);
    painter.drawEllipse(spacing*0.5+position,height/2-radius,radius*2,radius*2);
}

void PagerCircularActiveIndicator::toggleVisibility(const QString & text)
{
    /*
    bool searching = (text!="");
    indicator->setVisible(!searching);
    setVisible(!searching);
    std::vector<PagerItem*> pages = indicator->getPager()->pages;
    for (PagerItem* i : pages)
    {
        i->setVisible(!searching);
    }
    indicator->getPager()->setEnabled(!searching);
    */
}
