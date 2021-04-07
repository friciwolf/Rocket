#include <QGridLayout>
#include <QLabel>
#include <klocalizedstring.h>

#include "pager/pageritem.h"

#include "icongrid/icongrid.h"
#include "icongrid/icongriditem.h"
#include "stylingparams.h"

using namespace std;

PagerItem::PagerItem(QWidget *parent, vector<KApplication> applications) : QWidget(parent)
{
    QGridLayout * itemlayout = new QGridLayout();
    itemlayout->setAlignment(Qt::AlignCenter);
    setLayout(itemlayout);

    IconGrid * grid = new IconGrid(this);

    for (int i=0;i<applications.size();i++)
    {
        IconGridItem * griditem = new IconGridItem(grid,applications[i]);
        grid->addItem(griditem);
    }

    if (applications.size()==0)
    {
        QLabel * label = new QLabel(ki18n("No results found").toString(),grid);
        label->setAutoFillBackground(true);
        label->setPalette(RocketStyle::WhiteBackground);
        label->setAlignment(Qt::AlignCenter);
        label->setMinimumSize(500,50);
        QFont f;
        label->setFont(QFont(f.defaultFamily(),16));
        itemlayout->addWidget(label,1,1);
    }
    else {
        grid->setAutoFillBackground(true);
        grid->setPalette(RocketStyle::WhiteBackground);
        itemlayout->addWidget(grid,1,1);
    }

    itemlayout->setColumnStretch(0,1);
    itemlayout->setColumnStretch(1,2);
    itemlayout->setColumnStretch(2,1);
}
