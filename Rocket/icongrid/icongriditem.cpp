#include "icongriditem.h"
#include "icongriditemcanvas.h"
#include "stylingparams.h"
#include <QWidget>
#include <QPainter>
#include <QLabel>
#include <QFont>
#include <QScrollArea>
#include <QScrollBar>
#include <QPropertyAnimation>
#include <QDebug>
#include <QMouseEvent>

IconGridItem::IconGridItem(QWidget *parent, KApplication application) : QWidget(parent)
{
    m_application = application;
    m_icon = application.icon();
    m_label = application.name();
    setMinimumSize(m_item_size,m_item_size);

    //QPalette p;
    //setAutoFillBackground(true);
    //p.setColor(QPalette::ColorRole::Background,Qt::green);
    //setPalette(p);


    this->setLayout(m_layout);

    int ratio_rows[2] = {3,1};
    int icon_area_height = m_item_size*ratio_rows[0]/(ratio_rows[0]+ratio_rows[1]);
    int icon_area_width = m_item_size;

    m_canvas = new IconGridItemCanvas(this,application,icon_area_width,icon_area_height,m_icon_size);
    m_canvas->move(width()*0.5-m_canvas->width()*0.5,0);

    QLabel * name_label = new QLabel(m_label,this);
    QFont label_font = name_label->font();
    label_font.setPointSize(10);
    name_label->setMaximumWidth(m_item_size);
    name_label->setFont(label_font);
    name_label->setWordWrap(true);
    name_label->setAlignment(Qt::AlignCenter);

    //QPalette p2;
    //name_label->setAutoFillBackground(true);
    //p2.setColor(QPalette::ColorRole::Background,Qt::green);
    //name_label->setPalette(p2);

    m_layout->addWidget(m_canvas,0,0);
    m_layout->addWidget(name_label,1,0);
    m_layout->setRowStretch(0,ratio_rows[0]);
    m_layout->setRowStretch(1,ratio_rows[1]);
}

void IconGridItem::paintEvent(QPaintEvent *event)
{
    if (!m_highlighted) return;
    QPainter painter(this);
    painter.setBrush(QBrush(RocketStyle::WhiteColour,Qt::BrushStyle::SolidPattern));
    painter.setPen(Qt::white);
    painter.drawRoundedRect(0,0,width(),height()*0.99,15,15);
}

void IconGridItem::resizeEvent(QResizeEvent *event)
{
    QRect g = parentWidget()->geometry();
    m_item_size = g.width()*0.95/RocketStyle::m_cols;
    setMinimumSize(m_item_size,m_item_size);
    update();
    m_canvas->resizeEvent(event);
}

void IconGridItem::mouseMoveEvent(QMouseEvent *event)
{
    event->ignore();
}

IconGridItem::~IconGridItem()
{

}



