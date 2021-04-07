#ifndef ICONGRIDITEM_H
#define ICONGRIDITEM_H

#include <QObject>
#include <QWidget>
#include <QIcon>
#include <QString>
#include <QGridLayout>

#include "kapplication.h"
#include "stylingparams.h"

class IconGridItem : public QWidget
{
    Q_OBJECT
public:
    explicit IconGridItem(QWidget *parent, KApplication application);
    void mouseMoveEvent(QMouseEvent * event);
    void paintEvent(QPaintEvent *event);
    ~IconGridItem();

    void setHighlighted(bool highlighted){m_highlighted = highlighted;}
    KApplication getApplication(){return m_application;}

private:
    KApplication m_application;
    QIcon m_icon = QIcon::fromTheme("file");
    QString m_label = "file";
    QGridLayout * m_layout = new QGridLayout(this);
    QPixmap map;

    int m_item_size = RocketStyle::icongrid_item_size;
    int m_icon_size = RocketStyle::icongrid_icon_size;
    bool m_highlighted = false;
};

#endif // ICONGRIDITEM_H
