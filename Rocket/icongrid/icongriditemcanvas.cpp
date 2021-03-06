#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QTimer>
#include <QApplication>
#include <QGridLayout>
#include <QDrag>
#include <QMimeData>
#include <QCursor>
#include <QWindow>

#include <KRun>
#include <KService>
#include <KIconUtils>
#include <KDesktopFile>
#include <KIO/JobUiDelegate>
#include <KIO/ApplicationLauncherJob>

#include "icongriditemcanvas.h"
#include "tools/searchingapps.h"

#include "../RocketLibrary/rocketlibrary.h"
#include "../RocketLibrary/tools/kdeapplication.h"
#include "../RocketLibrary/tools/rocketconfigmanager.h"

IconGridItemCanvas::IconGridItemCanvas(QWidget *parent, KDEApplication application)
{
    m_icon = application.icon();
    m_application = application;
    setMouseTracking(true);
    setAcceptDrops(true);

//    QPalette p;
//    setAutoFillBackground(true);
//    p.setColor(QPalette::ColorRole::Background,Qt::cyan);
//    setPalette(p);
}

void IconGridItemCanvas::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    int size = std::min({width(),height()});
    int pos_x = (width()-size)/2;
    int pos_y = (height()-size)/2;
    m_icon = QIcon::fromTheme(m_application.iconname());

    painter.setViewport(0,0,width(),height());

    QPixmap qpixmap(size,size);
    qpixmap.fill(Qt::transparent);
    QPainter qpainter(&qpixmap);
    auto drawFolderBackgournd = [=](QPainter & painter) {
        m_icon = QIcon::fromTheme("");

        // pen for folders
        QPen pen;
        pen.setWidth(2);
        pen.setColor(ConfigManager.getSelectionColour().rgb());
        painter.setPen(pen);

        painter.setBrush(QBrush(ConfigManager.getSelectionColour()));
        painter.drawRoundedRect(size*0.01,size*0.01,size*0.99,size*0.99,7,7);
    };

    if(m_draw_folder)
    {
        if (!m_application.isFolder())
        {
            drawFolderBackgournd(qpainter);
            m_application.icon().paint(&qpainter,size*0.05,size*0.05,size*0.4,size*0.4);
            m_icon.addPixmap(qpixmap);
        }
        else
        {
            drawFolderBackgournd(qpainter);
            for (int i=0;i<(int)m_application.getChildren().size() && i<4;i++)
                m_application.getChildren()[i].icon().paint(&qpainter,size*0.05+i%2*size/2,size*0.05+i/2*size/2,size*0.4,size*0.4);
            m_icon.addPixmap(qpixmap);
        }
    }
    else {
        if (m_application.isFolder())
        {
            drawFolderBackgournd(qpainter);
            for (int i=0;i<(int)m_application.getChildren().size() && i<4;i++)
                m_application.getChildren()[i].icon().paint(&qpainter,size*0.05+i%2*size/2,size*0.05+i/2*size/2,size*0.4,size*0.4);
            m_icon.addPixmap(qpixmap);
        }
    }

    m_icon.paint(&painter,pos_x,pos_y,size,size);

}

void IconGridItemCanvas::mousePressEvent(QMouseEvent *event)
{
    if (event->button()==Qt::LeftButton)
    {
        m_clicked = true;
        m_pressPos = QCursor::pos();
        m_longclicktimer = new QTimer();
        connect(m_longclicktimer,&QTimer::timeout,this,&IconGridItemCanvas::initIconDragging);
        m_longclicktimer->setSingleShot(true);
        m_longclicktimer->start(500);
    }
    event->ignore();
}

void IconGridItemCanvas::setDraggable(bool draggable)
{
    m_draggable = draggable;
}

void IconGridItemCanvas::initIconDragging()
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
        std::vector<int> i = searchApplicationTree(ConfigManager.getApplicationTree(),m_application);
        mime->setText(QString::number(i[0])+";"+QString::number(i[1]));
        drag->setMimeData(mime);
        if (m_application.isFolder())
        {
            int size = std::min({width(),height()});
            drag->setPixmap(m_icon.pixmap(QSize(size,size)));
        }
        else
        {
            m_icon = m_application.icon();
            drag->setPixmap(m_application.icon().pixmap(size()));
        }
        drag->setHotSpot(QPoint(drag->pixmap().width()/2,drag->pixmap().height()/2));
        drag->exec();
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
        if (!m_application.isFolder())
        {
            KDesktopFile d(m_application.entrypath());
            KService s(&d,m_application.entrypath());
            KService::Ptr p(&s);
            KIO::ApplicationLauncherJob * job = new KIO::ApplicationLauncherJob(p);
            job->setUiDelegate(new KIO::JobUiDelegate(KJobUiDelegate::AutoHandlingEnabled, this));
            job->setAutoDelete(true);
            job->start();
            qDebug() << "IconGridItemCanvas: Executing " + s.entryPath() + " " + s.exec();
            connect(job,&KIO::ApplicationLauncherJob::finished,qApp,[=]{
                qApp->exit();
            });
            event->ignore();
            return;
        }
        else
        {
            folderClickEvent(m_application);
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
    std::vector<KDEApplication> apptree = ConfigManager.getApplicationTree();
    QStringList indices = event->mimeData()->text().split(";");
    int index1 = indices[0].toInt();
    int index2 = indices[1].toInt();
    KDEApplication dragged_item = (index2 == -1 ? apptree[index1] : apptree[index1].getChildren()[index2]);
    if (!(dragged_item==m_application) && !dragged_item.isFolder() && index2==-1)
    {
        m_draw_folder = true;
        update();
    }
    event->acceptProposedAction();
}

void IconGridItemCanvas::dropEvent(QDropEvent *event)
{
    std::vector<KDEApplication> apptree = ConfigManager.getApplicationTree();
    QStringList indices = event->mimeData()->text().split(";");
    int index1 = indices[0].toInt();
    int index2 = indices[1].toInt();
    KDEApplication dragged_item = (index2 == -1 ? apptree[index1] : apptree[index1].getChildren()[index2]);
    if (!dragged_item.isFolder() && !(dragged_item==m_application) && index2==-1)
    {
        makeFolder(m_application,dragged_item);
        event->accept();
        return;
    }
    // An item has been moved outside of a folder and has been dropped
    // on itself (and not on the pager)
    if (dragged_item==m_application && index2!=-1)
    {
        enterIconDraggingMode(false,((IconGridItemCanvas*)(event->source())));
        event->accept();
        return;
    }
    // In a folder, an a item has been dropped on another one
    if (index2!=-1)
    {
        enterIconDraggingMode(false,this);
        return;
    }
    enterIconDraggingMode(false);
}

void IconGridItemCanvas::dragMoveEvent(QDragMoveEvent *event)
{
    //qDebug() << "dragMoveIconGriditem";
    event->acceptProposedAction();
}

void IconGridItemCanvas::dragLeaveEvent(QDragLeaveEvent *event)
{
    //qDebug() << "dragging left" << m_application.name();
    m_draw_folder = false;
    update();
    event->accept();
}


void IconGridItemCanvas::resizeEvent(QResizeEvent *event)
{
    update();
}


IconGridItemCanvas::~IconGridItemCanvas()
{

}
