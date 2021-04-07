#ifndef ICONGRIDITEMCANVAS_H
#define ICONGRIDITEMCANVAS_H

#include <QObject>
#include <QWidget>
#include <QIcon>
#include <QDateTime>
#include "kapplication.h"

class IconGridItemCanvas : public QWidget
{
    Q_OBJECT
public:
    explicit IconGridItemCanvas(QWidget *parent, KApplication application, int area_width, int area_height, int iconsize);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    ~IconGridItemCanvas();


 public slots:
        void longpressanimation();

private:
    int m_area_height;
    int m_area_width;
    int m_icon_size;
    QIcon m_icon;
    QPixmap pixmap;
    KApplication m_application;

    bool m_clicked = false;
    QPoint m_pressPos;


protected:
    void paintEvent(QPaintEvent *);
};

#endif // ICONGRIDITEMCANVAS_H
