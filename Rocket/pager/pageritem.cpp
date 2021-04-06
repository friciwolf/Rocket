#include <QGridLayout>
#include <QLabel>

#include "pager/pageritem.h"

#include "icongrid/icongrid.h"
#include "icongrid/icongriditem.h"

using namespace std;

PagerItem::PagerItem(QWidget *parent, vector<KApplication> applications) : QWidget(parent)
{
    QGridLayout * itemlayout = new QGridLayout();
    itemlayout->setAlignment(Qt::AlignCenter);
    setLayout(itemlayout);

    //setAutoFillBackground(true);
    //setPalette(p);

    IconGrid * grid = new IconGrid(this);

    for (int i=0;i<applications.size();i++)
    {
        IconGridItem * griditem = new IconGridItem(grid,applications[i]);
        grid->addItem(griditem);
    }

    QPalette p;
    p.setColor(QPalette::ColorRole::Background,QColor(255,255,255,150));
    grid->setAutoFillBackground(true);
    grid->setPalette(p);
    itemlayout->addWidget(grid,1,1);

    itemlayout->setColumnStretch(0,1);
    itemlayout->setColumnStretch(1,2);
    itemlayout->setColumnStretch(2,1);
}
