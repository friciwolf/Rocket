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

void IconGrid::paintEvent(QPaintEvent *event)
{
    if (getItems().size()==0) return; // search yielded no results -> no drawing
    QPainter painter(this);
    if (ConfigManager.getBoxSetting()) // no boxes -> global blur
    {
        QColor color = ConfigManager.getBaseColour();
        painter.setBrush(QBrush(color,Qt::BrushStyle::SolidPattern));
        painter.setPen(Qt::transparent);
        painter.drawRoundedRect(0,0,width(),height(),30,30);
    }
}

void IconGrid::mouseMoveEvent(QMouseEvent *event)
{
    event->ignore();
}

IconGrid::~IconGrid()
{

}
