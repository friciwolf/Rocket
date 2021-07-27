#include <QWidget>
#include <QPainter>
#include <QLabel>
#include <QFont>
#include <QDebug>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>

#include "icongriditem.h"
#include "icongriditemcanvas.h"

#include "../RocketLibrary/tools/rocketconfigmanager.h"


IconGridItem::IconGridItem(QWidget *parent, KDEApplication application, QSize itemsize) : QWidget(parent)
{
    m_application = application;
    m_icon = application.icon();
    m_label = application.name();
    m_item_size = itemsize;
    setFixedSize(m_item_size);
    setMouseTracking(true);
    setAcceptDrops(true);

    //QPalette p;
    //setAutoFillBackground(true);
    //p.setColor(QPalette::ColorRole::Background,Qt::green);
    //setPalette(p);

    setLayout(m_layout);

    m_canvas = new IconGridItemCanvas(this,application);

    //QPalette p2;
    //name_label->setAutoFillBackground(true);
    //p2.setColor(QPalette::ColorRole::Background,Qt::green);
    //name_label->setPalette(p2);

    m_name_label = new QLabel(m_label,this);
    QFont label_font = m_name_label->font();
    label_font.setPointSize(ConfigManager.getFontSize1());
    m_name_label->setMaximumWidth(m_item_size.width());
    m_name_label->setFont(label_font);
    m_name_label->setWordWrap(true);
    m_name_label->setAlignment(Qt::AlignCenter);
    m_name_label->setGeometry(0,0,m_item_size.width(),10);
    m_name_label->setMouseTracking(true);
    m_layout->addWidget(m_name_label,1,0);

    m_layout->addWidget(m_canvas,0,0);
    m_layout->setRowStretch(1,m_ratio_rows[1]);
    m_layout->setRowStretch(0,m_ratio_rows[0]);
}


void IconGridItem::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if(m_highlighted)
    {
        painter.setBrush(QBrush(ConfigManager.getSelectionColour(),Qt::BrushStyle::SolidPattern));
        painter.setPen(Qt::transparent);
        painter.drawRoundedRect(0,0,width()*0.99,height()*0.99,15,15);
    }
}

void IconGridItem::resizeEvent(QResizeEvent *event)
{
    m_canvas->update();
}

void IconGridItem::mouseMoveEvent(QMouseEvent *event)
{
    event->ignore();
}

IconGridItem::~IconGridItem()
{

}



