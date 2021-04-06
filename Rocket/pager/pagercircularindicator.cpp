#include <QPainter>
#include <QDebug>

#include "pager/pagercircularindicator.h"

PagerCircularIndicator::PagerCircularIndicator(QWidget *parent, Pager *pager) : QWidget(parent)
{
    m_pager = pager;
    width = pager->getNumberOfElements()*(2*radius+spacing);
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::ColorRole::Background,QColor(255,255,255,150));
    setPalette(palette);
    setMaximumSize(width,height);
    setGeometry(0,0,width,height);
}

void PagerCircularIndicator::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setBrush(QBrush(QColor(255,255,255,150),Qt::BrushStyle::SolidPattern));
    for (int i=0;i<m_pager->getNumberOfElements();i++)
    {
        painter.drawEllipse(spacing*0.5+i*(radius*2+spacing),height/2-radius,radius*2,radius*2);
    }
}

