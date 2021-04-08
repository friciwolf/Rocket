#include "icongriditemcanvas.h"
#include "stylingparams.h"
#include <QPainter>
#include <KMessageBox>
#include <iostream>
#include <QDebug>
#include <QMouseEvent>
#include <QTimer>
#include <QProcess>
#include <QApplication>
#include <QGridLayout>
#include <KRun>
#include "kapplication.h"
#include "stylingparams.h"

IconGridItemCanvas::IconGridItemCanvas(QWidget *parent, KApplication application, int area_width, int area_height, int iconsize)
{
    m_icon = application.icon();
    m_area_width = area_width;
    m_area_height = area_height;
    m_icon_size = iconsize;
    m_application = application;

    //QPalette p;
    //setAutoFillBackground(true);
    //p.setColor(QPalette::ColorRole::Background,Qt::cyan);
    //setPalette(p);
}

void IconGridItemCanvas::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setBrush(QBrush(Qt::red,Qt::BrushStyle::SolidPattern));
    int pos_x = (width()-m_icon_size)/2;
    int pos_y = (height()-m_icon_size)/2;
    //painter.drawRect(pos_x,pos_y,m_icon_size,m_icon_size);
    m_icon.paint(&painter,pos_x,pos_y,m_icon_size,m_icon_size,Qt::AlignCenter);

}

void IconGridItemCanvas::mousePressEvent(QMouseEvent *event)
{
    m_clicked = true;
    m_pressPos = QCursor::pos();
    QTimer * timer = new QTimer();
    connect(timer,&QTimer::timeout,this,&IconGridItemCanvas::longpressanimation);
    timer->start(1000);
    timer->setSingleShot(true);
    event->ignore();
}

void IconGridItemCanvas::mouseMoveEvent(QMouseEvent *event)
{
    int dx = QCursor::pos().x()-m_pressPos.x();
    int dy = QCursor::pos().y()-m_pressPos.y();
    if (dx*dx+dy*dy>=RocketStyle::icongridcanvas_threshold)
    {
        m_clicked = false;
        event->ignore();
    }
}

void IconGridItemCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_clicked)
    {
        m_clicked = false;
        QList<QUrl> urls;
        if (m_application.terminal())
        {
            if(KRun::run("konsole -e "+m_application.exec(),urls,nullptr,m_application.name(),m_application.iconname()))
            {
                qApp->exit();
            }
        }
        else {
            if(KRun::run(m_application.exec(),urls,nullptr,m_application.name(),m_application.iconname())) {
                qApp->exit();
            }
        }

    }
    else
    {
        event->ignore();
    }
}

void IconGridItemCanvas::longpressanimation()
{
    if (m_clicked)
    {
        KMessageBox::information(nullptr,"move_animation");
        m_clicked = false;
    }
}

void IconGridItemCanvas::resizeEvent(QResizeEvent *event)
{
    m_icon_size = (width()<height() ? width() : height());
}


IconGridItemCanvas::~IconGridItemCanvas()
{

}
