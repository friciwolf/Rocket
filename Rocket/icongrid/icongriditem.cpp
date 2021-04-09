#include <QWidget>
#include <QPainter>
#include <QLabel>
#include <QFont>
#include <QDebug>
#include <QMouseEvent>

#include "icongriditem.h"
#include "icongriditemcanvas.h"
#include "stylingparams.h"


IconGridItem::IconGridItem(QWidget *parent, KApplication application, int itemsize) : QWidget(parent)
{
    m_application = application;
    m_icon = application.icon();
    m_label = application.name();
    m_item_size = itemsize;
    setFixedSize(m_item_size,m_item_size);

    //QPalette p;
    //setAutoFillBackground(true);
    //p.setColor(QPalette::ColorRole::Background,Qt::green);
    //setPalette(p);

    setLayout(m_layout);
    initIconSize();

    m_canvas = new IconGridItemCanvas(this,application,m_icon_size);

    m_name_label = new QLabel(m_label,this);
    QFont label_font = m_name_label->font();
    label_font.setPointSize(10);
    m_name_label->setMaximumWidth(m_item_size);
    m_name_label->setFont(label_font);
    m_name_label->setWordWrap(true);
    m_name_label->setAlignment(Qt::AlignCenter);

    //QPalette p2;
    //name_label->setAutoFillBackground(true);
    //p2.setColor(QPalette::ColorRole::Background,Qt::green);
    //name_label->setPalette(p2);

    m_layout->addWidget(m_canvas,0,0);
    m_layout->addWidget(m_name_label,1,0);
    m_layout->setRowStretch(0,m_ratio_rows[0]);
    m_layout->setRowStretch(1,m_ratio_rows[1]);
}

void IconGridItem::initIconSize()
{
    int icon_area_height = m_item_size*m_ratio_rows[0]/(m_ratio_rows[0]+m_ratio_rows[1]);
    int icon_area_width = m_item_size;
    m_icon_size = std::min({icon_area_width,icon_area_height})*0.9;
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
    initIconSize();
    m_canvas->setIconSize(m_icon_size);
    m_name_label->setMaximumWidth(m_item_size);
    m_canvas->update();
}

void IconGridItem::mouseMoveEvent(QMouseEvent *event)
{
    event->ignore();
}

IconGridItem::~IconGridItem()
{

}



