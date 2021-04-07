#include <QPainter>
#include <QDebug>
#include "mainwindow.h"

#include "pager/pagercircularindicator.h"
#include "stylingparams.h"

PagerCircularIndicator::PagerCircularIndicator(QWidget *parent, Pager *pager) : QWidget(parent)
{
    m_pager = pager;
    width = pager->getNumberOfElements()*(2*radius+spacing);
    setAutoFillBackground(true);
    setPalette(RocketStyle::WhiteBackground);
    setMaximumSize(width,height);
    setGeometry(0,0,width,height);
}

void PagerCircularIndicator::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setBrush(QBrush(RocketStyle::WhiteColour,Qt::BrushStyle::SolidPattern));
    for (int i=0;i<m_pager->getNumberOfElements();i++)
    {
        painter.drawEllipse(spacing*0.5+i*(radius*2+spacing),height/2-radius,radius*2,radius*2);
    }
}

void PagerCircularIndicator::repaint()
{
    update();
}

