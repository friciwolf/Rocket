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
    void setGridProperties();
    void gridLayoutManagement();
    IconGrid * getIconGrid(){return m_grid;}
    QSize getIconGridItemSize(){return m_grid_itemsize;}
    QSize getIconGridMaxSize() {return m_grid_maxsize;}
    QGridLayout * getItemLayout() {return m_itemlayout;}
    void resizeEvent(QResizeEvent *event);

private:
    IconGrid * m_grid;
    QGridLayout * m_itemlayout;
    QSize m_grid_maxsize;
    QSize m_grid_itemsize;
    vector<KApplication> m_applications;

};

#endif // PAGERITEM_H
