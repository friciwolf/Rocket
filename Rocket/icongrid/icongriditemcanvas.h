#ifndef ICONGRIDITEMCANVAS_H
#define ICONGRIDITEMCANVAS_H

#include <QObject>
#include <QWidget>
#include <QIcon>
#include <QDateTime>
#include <QTimer>

#include "../RocketLibrary/tools/kdeapplication.h"

class IconGridItemCanvas : public QWidget
{
    Q_OBJECT
public:
    explicit IconGridItemCanvas(QWidget *parent, KDEApplication application);
    KDEApplication getApplication(){return m_application;}
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void resizeEvent(QResizeEvent *event);
    ~IconGridItemCanvas();

public slots:
    void setDraggable(bool draggable);

signals:
    void enterIconDraggingMode(bool on, IconGridItemCanvas * canvas = nullptr);
    void folderClickEvent(KDEApplication folder);
    void makeFolder(KDEApplication app_dropped_on, KDEApplication app_dragged);

private:
    void initIconDragging();


    QIcon m_icon;
    QPixmap pixmap;
    KDEApplication m_application;

    bool m_clicked = false;
    bool m_draggable = true;
    QPoint m_pressPos;
    QTimer * m_longclicktimer = new QTimer();
    bool m_draw_folder = false; // draws folder

protected:
    void paintEvent(QPaintEvent *);
};

#endif // ICONGRIDITEMCANVAS_H
