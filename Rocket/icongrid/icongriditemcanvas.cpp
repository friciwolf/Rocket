#include "icongriditemcanvas.h"
#include <QPainter>
#include <KMessageBox>
#include <iostream>
#include <QDebug>
#include <QMouseEvent>
#include <QTimer>
#include <QProcess>
#include <QApplication>
#include <KRun>

IconGridItemCanvas::IconGridItemCanvas(QWidget *parent, KApplication application, int area_width, int area_height, int iconsize)
{
    m_icon = application.icon();
    m_area_width = area_width;
    m_area_height = area_height;
    m_icon_size = iconsize;
    m_application = new KApplication(application.name(),application.iconname(),application.icon(),application.exec(),application.comment());
    QPalette p;
    //this->setAutoFillBackground(true);
    //p.setColor(QPalette::ColorRole::Background,Qt::cyan);
    this->setPalette(p);
}

void IconGridItemCanvas::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    int pos_x = (m_area_width-m_icon_size)/2;
    int pos_y = (m_area_height-m_icon_size)/2;
    // Some icons are drawn weirdly, thus the -5
    m_icon.paint(&painter,pos_x-5,pos_y-5,m_icon_size,m_icon_size);

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
    if (dx*dx+dy*dy>=100)
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
        qDebug() << m_application->exec();
        if(KRun::run(m_application->exec(),QList<QUrl>(),nullptr,m_application->name(),m_application->iconname())) {
            qApp->exit();
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


IconGridItemCanvas::~IconGridItemCanvas()
{

}
