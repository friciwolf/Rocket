#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QTimer>
#include <QApplication>
#include <QGridLayout>
#include <QDrag>
#include <QMimeData>
#include <QCursor>

#include <KRun>
#include <KService>
#include <KDesktopFile>

#include "icongriditemcanvas.h"

#include "../RocketLibrary/rocketlibrary.h"
#include "../RocketLibrary/tools/kdeapplication.h"
#include "../RocketLibrary/tools/rocketconfigmanager.h"

IconGridItemCanvas::IconGridItemCanvas(QWidget *parent, KDEApplication application)
{
    m_icon = application.icon();
    m_application = application;
    setMouseTracking(true);
    setAcceptDrops(true);

    //QPalette p;
    //setAutoFillBackground(true);
    //p.setColor(QPalette::ColorRole::Background,Qt::cyan);
    //setPalette(p);
}

void IconGridItemCanvas::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    //painter.setBrush(QBrush(Qt::red,Qt::BrushStyle::SolidPattern));
    painter.setPen(Qt::transparent);
    int size = std::min({width(),height()});
    int pos_x = (width()-size)/2;
    int pos_y = (height()-size)/2;
    painter.drawRect(0,0,width(),height());
    m_icon.paint(&painter,pos_x,pos_y,size,size,Qt::AlignCenter);
    //manual adjustment to the middle
    QSize parentsize = parentWidget()->geometry().size();
    setGeometry((parentsize.width()-size)*0.5,geometry().y(),size,size);
}

void IconGridItemCanvas::mousePressEvent(QMouseEvent *event)
{
    if (event->button()==Qt::LeftButton)
    {
        m_clicked = true;
        m_pressPos = QCursor::pos();
        m_longclicktimer = new QTimer();
        connect(m_longclicktimer,&QTimer::timeout,this,&IconGridItemCanvas::m_starticondragging);
        m_longclicktimer->setSingleShot(true);
        m_longclicktimer->start(500);
    }
    event->ignore();
}

void IconGridItemCanvas::setDraggable(bool draggable)
{
    m_draggable = draggable;
}

void IconGridItemCanvas::m_starticondragging()
{
    if (!m_draggable)
    {
        return;
    }
    if (m_clicked) //i.e. cursor has not moved a lot
    {
        enterIconDraggingMode(true, this);
        QDrag *drag = new QDrag(this);
        QMimeData *mime = new QMimeData;
        mime->setText(m_application.name());
        drag->setMimeData(mime);
        drag->setPixmap(m_application.icon().pixmap(size()));
        drag->setHotSpot(QPoint(drag->pixmap().width()/2,drag->pixmap().height()/2));
        Qt::DropAction dropAction = drag->exec();
    }
    else {
        enterIconDraggingMode(false);
    }
    m_clicked = false;
}

void IconGridItemCanvas::mouseMoveEvent(QMouseEvent *event)
{
    int dx = QCursor::pos().x()-m_pressPos.x();
    int dy = QCursor::pos().y()-m_pressPos.y();
    if (dx*dx+dy*dy>=RocketStyle::click_tolerance && m_clicked)
    {
        m_longclicktimer->stop();
        m_clicked = false;
    }
    event->ignore();
}

void IconGridItemCanvas::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_clicked)
    {
        m_clicked = false;
        QList<QUrl> urls;
        KDesktopFile d(m_application.entrypath());
        KService s(&d,m_application.entrypath());
        if (KRun::run(s,urls,nullptr))
        {
            qApp->exit();
        }
        event->accept();
    }
    else
    {
        event->ignore();
    }
}


void IconGridItemCanvas::dragEnterEvent(QDragEnterEvent *event)
{
    //qDebug() << "dragging into" << m_application.name();
    event->acceptProposedAction();
}

void IconGridItemCanvas::dropEvent(QDropEvent *event)
{
    //qDebug() << "dropped" << event->mimeData()->text() << "on"<< m_application.name();
    enterIconDraggingMode(false);
    event->acceptProposedAction();
}

void IconGridItemCanvas::dragMoveEvent(QDragMoveEvent *event)
{
    //qDebug() << "dragMoveIconGriditem";
    event->ignore();
}

void IconGridItemCanvas::dragLeaveEvent(QDragLeaveEvent *event)
{
    //qDebug() << "dragging left" << m_application.name();
    event->accept();
}


void IconGridItemCanvas::resizeEvent(QResizeEvent *event)
{
    update();
}


IconGridItemCanvas::~IconGridItemCanvas()
{

}
