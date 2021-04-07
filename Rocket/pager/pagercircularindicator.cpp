#include <QPainter>
#include <QDebug>
#include "mainwindow.h"

#include "pager/pagercircularindicator.h"
#include "stylingparams.h"

PagerCircularIndicator::PagerCircularIndicator(QWidget *parent, Pager *pager) : QWidget(parent)
{
    m_pager = pager;
    m_width = pager->getNumberOfElements()*(2*m_radius+m_spacing);
    //setAutoFillBackground(true);
    //setPalette(RocketStyle::WhiteBackground);
    setMaximumSize(m_width,m_height);
    setGeometry(0,0,m_width,m_height);
}

void PagerCircularIndicator::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setBrush(QBrush(RocketStyle::WhiteColour,Qt::BrushStyle::SolidPattern));
    painter.setPen(Qt::transparent);
    //painter.drawRoundedRect(0,0,width,height,5,5);
    for (int i=0;i<m_pager->getNumberOfElements();i++)
    {
        painter.drawEllipse(m_spacing*0.5+i*(m_radius*2+m_spacing),m_height/2-m_radius,m_radius*2,m_radius*2);
    }
}

