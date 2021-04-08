#include <QPainter>
#include <QDebug>
#include "pager.h"
#include "mainwindow.h"
#include "pager/pagercircularindicator.h"
#include "stylingparams.h"

PagerCircularIndicator::PagerCircularIndicator(QWidget *parent, Pager *pager) : QWidget(parent)
{
    m_pager = pager;
    m_height = parentWidget()->size().height()*0.03;
    m_radius = parentWidget()->size().height()*0.01;
    m_spacing = m_radius/2;
    m_elements = pager->getNumberOfElements();
    m_width = m_elements*(2*m_radius+m_spacing);
    //setAutoFillBackground(true);
    setPalette(RocketStyle::WhiteBackground);
    //setMaximumSize(m_width,m_height);
    setGeometry(0,0,m_width,m_height);
    setMinimumSize(m_width,m_height);
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

void PagerCircularIndicator::resizeEvent(QResizeEvent *event)
{
    m_radius = parentWidget()->size().height()*0.01;
    m_spacing = m_radius/2;
    m_height = parentWidget()->size().height()*0.03;
    m_width = m_elements*(2*m_radius+m_spacing);
    setMinimumSize(m_width,m_height);
}
