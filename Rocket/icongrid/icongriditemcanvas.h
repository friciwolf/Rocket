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
    explicit IconGridItemCanvas(QWidget *parent, KApplication application);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);
    ~IconGridItemCanvas();


 public slots:
        void longpressanimation();

private:
    QIcon m_icon;
    QPixmap pixmap;
    KApplication m_application;

    bool m_clicked = false;
    QPoint m_pressPos;


protected:
    void paintEvent(QPaintEvent *);
};

#endif // ICONGRIDITEMCANVAS_H
