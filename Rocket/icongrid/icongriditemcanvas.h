#ifndef ICONGRIDITEMCANVAS_H
#define ICONGRIDITEMCANVAS_H

#include <QObject>
#include <QWidget>
#include <QIcon>
#include <QDateTime>
#include "kapplication.h"
#include "stylingparams.h"

class IconGridItemCanvas : public QWidget
{
    Q_OBJECT
public:
    explicit IconGridItemCanvas(QWidget *parent, KApplication application, int iconsize);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);
    void setIconSize(int size){m_icon_size = size;}
    //void setIconOffset(int offset){m_icon_offset = offset;}
    int getIconSize(){return m_icon_size;}
    //int getIconOffset(){return m_icon_offset;}
    ~IconGridItemCanvas();


 public slots:
        void longpressanimation();

private:
    int m_icon_size;
    //int m_icon_offset = RocketStyle::icongridcanvas_icon_offset;
    QIcon m_icon;
    QPixmap pixmap;
    KApplication m_application;

    bool m_clicked = false;
    QPoint m_pressPos;


protected:
    void paintEvent(QPaintEvent *);
};

#endif // ICONGRIDITEMCANVAS_H
