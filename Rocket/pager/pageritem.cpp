#include <QGridLayout>
#include <QLabel>
#include <klocalizedstring.h>
#include <QDebug>

#include "pager/pageritem.h"

#include "icongrid/icongrid.h"
#include "icongrid/icongriditem.h"
#include "stylingparams.h"

PagerItem::PagerItem(QWidget *parent, vector<KApplication> applications) : QWidget(parent)
{
    m_applications = applications;
    m_itemlayout = new QGridLayout();
    setLayout(m_itemlayout);
    m_grid = new IconGrid(this);
    setGridProperties();
    for (KApplication app : applications)
    {
        IconGridItem * griditem = new IconGridItem(m_grid,app,m_grid_itemsize);
        m_grid->addItem(griditem);
    }
    gridLayoutManagement();
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
    int size1 = m_grid_maxsize.width()/RocketStyle::m_cols;
    int size2 = m_grid_maxsize.height()/RocketStyle::m_rows;
    m_grid_itemsize = QSize(size1,size2);
}

void PagerItem::gridLayoutManagement()
{
    if (m_applications.size()==0)
    {
        QLabel * label = new QLabel(ki18n("No results found").toString(),m_grid);
        label->setAutoFillBackground(true);
        label->setPalette(RocketStyle::WhiteBackground);
        label->setAlignment(Qt::AlignCenter);
        label->setMinimumSize(RocketStyle::pageritem_error_label_size);
        label->setMaximumSize(RocketStyle::pageritem_error_label_size);
        label->setFont(RocketStyle::pageritem_error_label_font);
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

void PagerItem::resizeEvent(QResizeEvent *event)
{
    setGridProperties();
}
