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
    QGridLayout * getItemLayout() {return m_itemlayout;}
    void resizeEvent(QResizeEvent *event);

private:
    IconGrid * m_grid;
    QGridLayout * m_itemlayout;

};

#endif // PAGERITEM_H
