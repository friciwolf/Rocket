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
    void iconDraggingOn(bool on);

private:
    void m_starticondragging();

    QIcon m_icon;
    QPixmap pixmap;
    KDEApplication m_application;

    bool m_clicked = false;
    bool m_draggable = true;
    QPoint m_pressPos;
    QTimer * m_longclicktimer = new QTimer();


protected:
    void paintEvent(QPaintEvent *);
};

#endif // ICONGRIDITEMCANVAS_H
