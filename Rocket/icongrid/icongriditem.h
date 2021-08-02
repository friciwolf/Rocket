#ifndef ICONGRIDITEM_H
#define ICONGRIDITEM_H

#include <QObject>
#include <QWidget>
#include <QIcon>
#include <QString>
#include <QGridLayout>
#include <QLabel>

#include "icongriditemcanvas.h"

#include "../RocketLibrary/rocketlibrary.h"
#include "../RocketLibrary/tools/kdeapplication.h"

class IconGridItem : public QWidget
{
    Q_OBJECT
public:
    explicit IconGridItem(QWidget *parent, KDEApplication application, QSize itemsize);
    void mouseMoveEvent(QMouseEvent * event);
    void paintEvent(QPaintEvent *event);
    void initIconSize();
    void resizeEvent(QResizeEvent *event);
    ~IconGridItem();

    void setHighlighted(bool highlighted){m_highlighted = highlighted;}
    void setItemSize(QSize size) {m_item_size = size;}
    QSize getItemSize(){return  m_item_size;}
    KDEApplication getApplication(){return m_application;}
    IconGridItemCanvas * getCanvas(){return m_canvas;}
    void setCanvas(IconGridItemCanvas * canvas){m_canvas = canvas;}
    QLabel * getNameLabel() {return m_name_label;}

private:
    KDEApplication m_application;
    IconGridItemCanvas * m_canvas;
    QIcon m_icon = QIcon::fromTheme("file");
    QString m_label = "file";
    QGridLayout * m_layout = new QGridLayout(this);
    QLabel * m_name_label;
    QPixmap map;

    QSize m_item_size;
    std::vector<int> m_ratio_rows = RocketStyle::icongrid_ratio_rows;
    bool m_highlighted = false;
};

#endif // ICONGRIDITEM_H
