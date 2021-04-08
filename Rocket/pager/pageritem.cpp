#include <QGridLayout>
#include <QLabel>
#include <klocalizedstring.h>
#include <QDebug>

#include "pager/pageritem.h"

#include "icongrid/icongrid.h"
#include "icongrid/icongriditem.h"
#include "stylingparams.h"

using namespace std;

PagerItem::PagerItem(QWidget *parent, vector<KApplication> applications) : QWidget(parent)
{
    m_itemlayout = new QGridLayout();
    setLayout(m_itemlayout);

    m_grid = new IconGrid(this);

    for (int i=0;i<applications.size();i++)
    {
        IconGridItem * griditem = new IconGridItem(m_grid,applications[i]);
        m_grid->addItem(griditem);
    }

    if (applications.size()==0)
    {
        QLabel * label = new QLabel(ki18n("No results found").toString(),m_grid);
        label->setAutoFillBackground(true);
        label->setPalette(RocketStyle::WhiteBackground);
        label->setAlignment(Qt::AlignCenter);
        label->setMinimumSize(RocketStyle::pageritem_error_label_size);
        label->setMaximumSize(RocketStyle::pageritem_error_label_size);
        label->setFont(RocketStyle::pageritem_error_label_font);
        m_itemlayout->addWidget(label,1,1);
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
    //setFixedSize(parentWidget()->size());
    m_grid->resizeEvent(event);
}
