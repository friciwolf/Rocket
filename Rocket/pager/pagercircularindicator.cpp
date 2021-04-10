#include <QPainter>
#include <QDebug>

#include "pager.h"
#include "mainwindow.h"
#include "pager/pagercircularindicator.h"
#include "stylingparams.h"
#include "tools/rocketconfigmanager.h"

PagerCircularIndicator::PagerCircularIndicator(QWidget *parent, Pager *pager) : QWidget(parent)
{
    m_pager = pager;
    m_elements = pager->getNumberOfElements();
    setPalette(ConfigManager.getBaseColourBackgroundPalette());
    positioning();
}

void PagerCircularIndicator::positioning()
{
    m_parent_geometry = parentWidget()->geometry();
    m_radius = m_parent_geometry.size().height()*0.01;
    m_spacing = m_radius;
    setFixedSize(m_elements*(2*m_radius+m_spacing),m_parent_geometry.size().height()*0.1);
    setGeometry((m_parent_geometry.width()-width())*0.5,(m_parent_geometry.height()-height()),width(),height());
}

void PagerCircularIndicator::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setBrush(QBrush(ConfigManager.getBaseColour(),Qt::BrushStyle::SolidPattern));
    painter.setPen(Qt::transparent);
    //painter.drawRoundedRect(0,0,width,height,5,5);
    for (int i=0;i<m_pager->getNumberOfElements();i++)
    {
        painter.drawEllipse(m_spacing*0.5+i*(m_radius*2+m_spacing),height()/2-m_radius,m_radius*2,m_radius*2);
    }
}

void PagerCircularIndicator::resizeEvent(QResizeEvent *event)
{
    positioning();
}
