#ifndef PAGERITEM_H
#define PAGERITEM_H

#include <QObject>
#include <QWidget>
#include <QString>

#include "icongrid/icongrid.h"

#include "../RocketLibrary/tools/kdeapplication.h"


using namespace std;

class PagerItem : public QWidget
{
    Q_OBJECT
public:
    explicit PagerItem(QWidget *parent, vector<KDEApplication> applications);
    void setGridProperties();
    void gridLayoutManagement();
    IconGrid * getIconGrid(){return m_grid;}
    QSize getIconGridItemSize(){return m_grid_itemsize;}
    QSize getIconGridMaxSize() {return m_grid_maxsize;}
    QGridLayout * getItemLayout() {return m_itemlayout;}

    void mouseMoveEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    IconGrid * m_grid;
    QGridLayout * m_itemlayout;
    QSize m_grid_maxsize;
    QSize m_grid_itemsize;
    vector<KDEApplication> m_applications;

};

#endif // PAGERITEM_H
