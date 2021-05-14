#include <QGridLayout>
#include <QLabel>
#include <klocalizedstring.h>
#include <QDebug>
#include <QApplication>
#include <QDropEvent>
#include <QMimeData>

#include "pager/pageritem.h"
#include "icongrid/icongrid.h"
#include "icongrid/icongriditem.h"

#include "../RocketLibrary/tools/rocketconfigmanager.h"

PagerItem::PagerItem(QWidget *parent, vector<KDEApplication> applications) : QWidget(parent)
{
    m_applications = applications;
    m_itemlayout = new QGridLayout();
    setLayout(m_itemlayout);
    m_grid = new IconGrid(this);
    setGridProperties();
    for (KDEApplication app : applications)
    {
        IconGridItem * griditem = new IconGridItem(m_grid,app,m_grid_itemsize);
        m_grid->addItem(griditem);
    }
    gridLayoutManagement();
    setMouseTracking(true);
}

void PagerItem::setGridProperties()
{
    /*
     *  Updates the properties of the grid for initialization and resizing.
     *
     *  The grid occupies maximum 90 % and 80 % of the screen horizontally and vertically, respectively.
     *
     * */
    m_grid_maxsize = QSize(parentWidget()->size().width()*0.9,parentWidget()->size().height()*0.8);
    m_grid->setMaximumSize(m_grid_maxsize);

    // Estimate for a griditemsize
    int size1 = m_grid_maxsize.width()/ConfigManager.getColumnNumber();
    int size2 = m_grid_maxsize.height()/ConfigManager.getRowNumber();
    m_grid_itemsize = QSize(size1,size2);
}

void PagerItem::gridLayoutManagement()
{
    if (m_applications.size()==0)
    {
        QLabel * label = new QLabel(ki18n("No results found").toString(),m_grid);
        label->setAutoFillBackground(true);
        if (ConfigManager.getBoxSetting())
            label->setPalette(ConfigManager.getBaseColourBackgroundPalette());
        else
            label->setPalette(ConfigManager.getSelectionColourBackgroundPalette());
        label->setAlignment(Qt::AlignCenter);
        label->setFixedHeight(m_grid_itemsize.height());
        QFont label_font = label->font();
        label_font.setPointSize(ConfigManager.getFontSize2());
        label->setFont(label_font);
        m_itemlayout->addWidget(label,1,1);
        setLayout(m_itemlayout);
    }
    else {
        m_itemlayout->addWidget(m_grid,1,1);
    }

    m_itemlayout->setColumnStretch(0,1);
    m_itemlayout->setColumnStretch(1,m_grid->getMaxNumberOfColumns());
    m_itemlayout->setColumnStretch(2,1);
    m_itemlayout->setRowStretch(0,1);
    m_itemlayout->setRowStretch(1,m_grid->getCurrentNumberOfRows());
    m_itemlayout->setRowStretch(2,1);
}

void PagerItem::mouseMoveEvent(QMouseEvent *event)
{
    event->ignore();
}

void PagerItem::resizeEvent(QResizeEvent *event)
{
    setGridProperties();
}
