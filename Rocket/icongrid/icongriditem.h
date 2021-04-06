#ifndef ICONGRIDITEM_H
#define ICONGRIDITEM_H

#include <QObject>
#include <QWidget>
#include <QIcon>
#include <QString>
#include <QGridLayout>

#include "kapplication.h"

class IconGridItem : public QWidget
{
    Q_OBJECT
public:
    explicit IconGridItem(QWidget *parent, KApplication application);
    ~IconGridItem();

private:
    QIcon m_icon = QIcon::fromTheme("file");
    QString m_label = "file";
    QGridLayout * m_layout = new QGridLayout(this);

    int m_item_size = 150;
    int m_icon_size = 80;
};

#endif // ICONGRIDITEM_H
