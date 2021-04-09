#include <QPainter>
#include <QDebug>
#include <QColor>

#include "pager/pagercircularactiveindicator.h"
#include "stylingparams.h"

PagerCircularActiveIndicator::PagerCircularActiveIndicator(QWidget *parent, PagerCircularIndicator * indicator) : QWidget(parent)
{
    pager = indicator->getPager();
    this->indicator = indicator;
    positioning();
}

void PagerCircularActiveIndicator::positioning()
{
    radius = indicator->getRadius();
    spacing = indicator->getSpacing();
    setGeometry(indicator->geometry());
}

void PagerCircularActiveIndicator::paintEvent(QPaintEvent *event)
{
    int position = -(float)pager->pages[0]->pos().x()/pager->width() * (2*radius+spacing);
    QPainter painter(this);
    painter.setBrush(RocketStyle::active_indicator_brush);
    painter.setPen(Qt::transparent);
    //painter.drawEllipse(spacing*0.5+current_item*(radius*2+spacing)+correction,height/2-radius,radius*2,radius*2);
    painter.drawEllipse(spacing*0.5+position,height()/2-radius,radius*2,radius*2);
}

void PagerCircularActiveIndicator::resizeEvent(QResizeEvent *event)
{
    positioning();
}
