#ifndef PAGERITEM_H
#define PAGERITEM_H

#include <QObject>
#include <QWidget>
#include <QString>

#include "icongrid/kapplication.h"

using namespace std;

class PagerItem : public QWidget
{
    Q_OBJECT
public:
    explicit PagerItem(QWidget *parent, vector<KApplication> applications);
};

#endif // PAGERITEM_H
