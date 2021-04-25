#ifndef ICONGRIDITEM_H
#define ICONGRIDITEM_H

#include <QObject>
#include <QWidget>
#include <QIcon>
#include <QString>
#include <QGridLayout>
#include <QLabel>

#include "../RocketLibrary/rocketlibrary.h"
#include "kapplication.h"
#include "icongriditemcanvas.h"

class IconGridItem : public QWidget
{
    Q_OBJECT
public:
    explicit IconGridItem(QWidget *parent, KApplication application, QSize itemsize);
    void mouseMoveEvent(QMouseEvent * event);
    void paintEvent(QPaintEvent *event);
    void initIconSize();
    //void dragEnterEvent(QDragEnterEvent *event);
    //void dragMoveEvent(QDragMoveEvent *event);
    //void dropEvent(QDropEvent *event);
    //void dragLeaveEvent(QDragLeaveEvent *event);
    void resizeEvent(QResizeEvent *event);
    ~IconGridItem();

    void setHighlighted(bool highlighted){m_highlighted = highlighted;}
    void setItemSize(QSize size) {m_item_size = size;}
    QSize getItemSize(){return  m_item_size;}
    KApplication getApplication(){return m_application;}
    IconGridItemCanvas * getCanvas(){return m_canvas;}

private:
    KApplication m_application;
    IconGridItemCanvas * m_canvas;
    QIcon m_icon = QIcon::fromTheme("file");
    QString m_label = "file";
    QGridLayout * m_layout = new QGridLayout(this);
    QLabel * m_name_label;
    QPixmap map;
    bool m_hoveredover_during_drag_and_drop = false;

    QSize m_item_size;
    std::vector<int> m_ratio_rows = RocketStyle::icongrid_ratio_rows;
    bool m_highlighted = false;
};

#endif // ICONGRIDITEM_H
