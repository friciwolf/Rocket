#include <QPainter>
#include <QDebug>
#include <QColor>

#include "pager/pagercircularactiveindicator.h"

#include "../RocketLibrary/tools/rocketconfigmanager.h"

PagerCircularActiveIndicator::PagerCircularActiveIndicator(QWidget *parent, PagerCircularIndicator * indicator) : QWidget(parent)
{
    if (ConfigManager.getVerticalModeSetting()) verticalpager = indicator->getVerticalPager();
    else pager = indicator->getPager();
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
    QPainter painter(this);
    painter.setBrush(QBrush(ConfigManager.getSecondaryColour(),Qt::BrushStyle::SolidPattern));
    painter.setPen(Qt::transparent);
    //painter.drawEllipse(spacing*0.5+current_item*(radius*2+spacing)+correction,height/2-radius,radius*2,radius*2);
    if (ConfigManager.getVerticalModeSetting())
    {
        int position = -(float)verticalpager->pages[0]->pos().y()/verticalpager->height() * (2*radius+spacing);
        painter.drawEllipse(width()/2-radius,spacing*0.5+position,radius*2,radius*2);
    }
    else
    {
        int position = -(float)pager->pages[0]->pos().x()/pager->width() * (2*radius+spacing);
        painter.drawEllipse(spacing*0.5+position,height()/2-radius,radius*2,radius*2);
    }
}

void PagerCircularActiveIndicator::resizeEvent(QResizeEvent *event)
{
    positioning();
}
