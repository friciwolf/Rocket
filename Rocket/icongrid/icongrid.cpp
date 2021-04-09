#include <QGridLayout>
#include <QMouseEvent>
#include <QDebug>
#include <QPainter>

#include "icongrid.h"


IconGrid::IconGrid(QWidget * parent) : QWidget (parent)
{
    m_layout = new QGridLayout();
    m_layout->setAlignment(Qt::AlignCenter);
    m_layout->setSpacing(1);
    setLayout(m_layout);
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
    if (delta == 1) //Key_Right
    {
        if (m_active_element % m_cols == m_cols-1) //Last Column
        {
            goToPage(1);
            return;
        }
    }
    if (delta == -1) //Key_Left
    {
        if (m_active_element % m_cols == 0) //First Column
        {
            goToPage(-1);
            return;
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
    painter.setBrush(QBrush(RocketStyle::WhiteColour,Qt::BrushStyle::SolidPattern));
    painter.setPen(RocketStyle::WhiteColour);
    painter.drawRoundedRect(0,0,width(),height(),30,30);
}

void IconGrid::resizeEvent(QResizeEvent *event)
{

}

IconGrid::~IconGrid()
{

}
