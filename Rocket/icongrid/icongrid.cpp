#include "icongrid.h"
#include <QGridLayout>

IconGrid::IconGrid(QWidget * parent) : QWidget (parent)
{
    m_layout = new QGridLayout();
    m_layout->setAlignment(Qt::AlignCenter);
    m_layout->setSpacing(1);
    this->setLayout(m_layout);
}

void IconGrid::addItem(IconGridItem * item)
{
    int column_position = getCurrentNumberOfItems()%getMaxNumberOfColumns();
    int row_position = (int)getCurrentNumberOfItems()/getMaxNumberOfColumns();
    m_layout->addWidget(item,row_position,column_position,Qt::AlignCenter);
    m_items.push_back(item);
}

IconGrid::~IconGrid()
{

}
