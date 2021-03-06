#include <QPainter>
#include <QDebug>

#include "horizontalpager.h"
#include "mainwindow.h"
#include "pager/pagercircularindicator.h"

#include "../RocketLibrary/tools/rocketconfigmanager.h"

PagerCircularIndicator::PagerCircularIndicator(QWidget *parent, HorizontalPager *pager) : QWidget(parent)
{
    m_pager = pager;
    m_elements = pager->getNumberOfElements();
    setPalette(ConfigManager.getBaseColourBackgroundPalette());
    positioning();
}

PagerCircularIndicator::PagerCircularIndicator(QWidget *parent, VerticalPager *pager) : QWidget(parent)
{
    m_verticalpager = pager;
    m_elements = pager->getNumberOfElements();
    setPalette(ConfigManager.getBaseColourBackgroundPalette());
    positioning();
}

void PagerCircularIndicator::positioning()
{
    m_parent_geometry = parentWidget()->geometry();
    m_radius = m_parent_geometry.size().height()*0.01;
    m_spacing = m_radius;
    if (m_pager==nullptr)
    {
        m_elements = m_verticalpager->getNumberOfElements();
        setFixedSize(m_parent_geometry.size().width()*0.05,m_elements*(2*m_radius+m_spacing));
        setGeometry((m_parent_geometry.width()-width()),(m_parent_geometry.height()-height())*0.5,width(),height());
    }
    else
    {
        m_elements = m_pager->getNumberOfElements();
        setFixedSize(m_elements*(2*m_radius+m_spacing),m_parent_geometry.size().height()*0.1);
        setGeometry((m_parent_geometry.width()-width())*0.5,(m_parent_geometry.height()-height()),width(),height());
    }
}

void PagerCircularIndicator::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if (ConfigManager.getBoxSetting())
            painter.setBrush(QBrush(ConfigManager.getBaseColour(),Qt::BrushStyle::SolidPattern));
    else
            painter.setBrush(QBrush(ConfigManager.getSelectionColour(),Qt::BrushStyle::SolidPattern));
    painter.setPen(Qt::transparent);
    //painter.drawRoundedRect(0,0,width,height,5,5);
    if (m_pager==nullptr)
    {
        for (int i=0;i<m_verticalpager->getNumberOfElements();i++)
        {
            painter.drawEllipse(width()/2-m_radius,m_spacing*0.5+i*(m_radius*2+m_spacing),m_radius*2,m_radius*2);
        }
    }
    else
    {
        for (int i=0;i<m_pager->getNumberOfElements();i++)
        {
            painter.drawEllipse(m_spacing*0.5+i*(m_radius*2+m_spacing),height()/2-m_radius,m_radius*2,m_radius*2);
        }
    }
}

void PagerCircularIndicator::resizeEvent(QResizeEvent *event)
{
    positioning();
}
