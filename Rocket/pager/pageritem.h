#ifndef PAGERITEM_H
#define PAGERITEM_H

#include <QObject>
#include <QWidget>
#include <QString>

#include "icongrid/kapplication.h"
#include "icongrid/icongrid.h"

using namespace std;

class PagerItem : public QWidget
{
    Q_OBJECT
public:
    explicit PagerItem(QWidget *parent, vector<KApplication> applications);
    IconGrid * getIconGrid(){return m_grid;}

private:
    IconGrid * m_grid;
};

#endif // PAGERITEM_H
