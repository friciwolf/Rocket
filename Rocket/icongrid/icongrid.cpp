#include <QGridLayout>
#include <QMouseEvent>
#include <QDebug>
#include <QPainter>

#include "icongrid.h"

#include "../RocketLibrary/tools/rocketconfigmanager.h"

IconGrid::IconGrid(QWidget * parent) : QWidget (parent)
{
    m_layout = new QGridLayout();
    m_layout->setAlignment(Qt::AlignCenter);
    m_layout->setMargin(10);
    m_layout->setSpacing(0);
    setLayout(m_layout);
    setMouseTracking(true);
}

void IconGrid::addItem(IconGridItem * item)
{
    int column_position = getNumberOfItems()%getMaxNumberOfColumns();
    int row_position = (int)getNumberOfItems()/getMaxNumberOfColumns();
    m_layout->addWidget(item,row_position,column_position,Qt::AlignCenter);
    m_items.push_back(item);
    connect(item->getCanvas(),&IconGridItemCanvas::drawSeparator,this,&IconGrid::drawSeparator);
    connect(item->getCanvas(),&IconGridItemCanvas::eraseSeparator,this,&IconGrid::eraseSeparator);
}


void IconGrid::setActiveElement(int element)
{
    int delta = element-m_active_element;
    // Vertical Mode Behaviour
    if (ConfigManager.getVerticalModeSetting())
    {
        if (delta == m_cols) //Key_Down
        {
            if ((int)(m_active_element/m_cols) == m_rows-1) //Last Row, Vertical Mode
            {
                goToPage(1);
                return;
            }
        }
        if (delta == -m_cols) //Key_Up
        {
            if ((int)(m_active_element/m_cols) == 0) //First Column, Vertical Mode
            {
                goToPage(-1);
                return;
            }
        }
    }
    // Horizontal Mode Behaviour
    if (!ConfigManager.getVerticalModeSetting())
    {
        if (delta == 1) //Key_Right
        {
            if (m_active_element % m_cols == m_cols-1) //Last Column, Horizontal Mode
            {
                goToPage(1);
                return;
            }
        }
        if (delta == -1) //Key_Left
        {
            if (m_active_element % m_cols == 0) //First Column, Horizontal Mode
            {
                goToPage(-1);
                return;
            }
        }
    }

    if (element>=0 && element<=getItems().size()-1)
    {
        m_active_element = element;
    }


}


void IconGrid::unhighlightAll()
{
    for (IconGridItem * i : getItems())
    {
        i->setHighlighted(false);
        i->update();
    }
}

void IconGrid::highlight(int element)
{
    m_active_element = element;
    getItems()[element]->setHighlighted(true);
    getItems()[element]->update();
}

void IconGrid::resetHighlightAndActiveElement()
{
    unhighlightAll();
    m_active_element = -1;
}

void IconGrid::drawSeparator(IconGridItemCanvas * gridItemCanvas, bool left)
{
    int pos=-1;
    for (int i=0;i<getItems().size();i++)
    {
        if (getItems()[i]->getCanvas()==gridItemCanvas)
        {
            pos = i;
            break;
        }
    }
    if (pos==-1) return;
    if (left)
    {
        if (pos>0)
            if(pos%ConfigManager.getColumnNumber()==0)
                m_draw_separator_at = getItems()[pos]->pos();
            else
                m_draw_separator_at = (getItems()[pos]->pos()+getItems()[pos-1]->pos()+QPoint(getItems()[pos-1]->width(),0))*0.5;
        else
            m_draw_separator_at = getItems()[0]->pos();
    }
    else
    {
        if (pos<getItems().size()-1)
            if((pos+1)%ConfigManager.getColumnNumber()==0)
                m_draw_separator_at = getItems()[pos]->pos()+QPoint(getItems()[pos]->width(),0);
            else
                m_draw_separator_at = (getItems()[pos]->pos()+getItems()[pos+1]->pos()+QPoint(getItems()[pos]->width(),0))*0.5;
        else
            m_draw_separator_at = getItems()[pos]->pos()+QPoint(getItems()[pos]->width(),0);
    }
    update();
}

void IconGrid::eraseSeparator()
{
    update();
}

void IconGrid::paintEvent(QPaintEvent *event)
{
    if (getItems().size()==0) return; // search yielded no results -> no drawing
    if (!ConfigManager.getBoxSetting()) return; // no boxes -> global blur
    QPainter painter(this);
    QColor color = ConfigManager.getBaseColour();
    painter.setBrush(QBrush(color,Qt::BrushStyle::SolidPattern));
    painter.setPen(Qt::transparent);
    painter.drawRoundedRect(0,0,width(),height(),30,30);
    if (m_draw_separator_at!=QPoint(-1,-1))
    {
        int maxheight = -1;
        for (IconGridItem * i : getItems())
            if (i->getNameLabel()->pos().y()>maxheight) maxheight = i->getNameLabel()->pos().y();
        painter.setPen(ConfigManager.getSecondaryColour());
        painter.drawLine(m_draw_separator_at.x(),m_draw_separator_at.y(),m_draw_separator_at.x(),maxheight+m_draw_separator_at.y());
        m_draw_separator_at = QPoint(-1,-1);
    }
}

void IconGrid::mouseMoveEvent(QMouseEvent *event)
{
    event->ignore();
}

IconGrid::~IconGrid()
{

}
