#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pager/pager.h"
#include "pager/pageritem.h"
#include "pager/pagercircularindicator.h"
#include "pager/pagercircularactiveindicator.h"

#include <QGridLayout>
#include <QLineEdit>
#include <QGraphicsBlurEffect>
#include <QPainter>
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QWidget * centralwidget = this->findChild<QWidget*>("centralWidget");

    pager = new Pager(centralwidget);

    indicator = new PagerCircularIndicator(centralwidget,pager);
    indicator->move(width()*0.5-indicator->width/2,height()-indicator->height);

    active_indicator = new PagerCircularActiveIndicator(centralwidget,indicator);
    active_indicator->move(width()*0.5-indicator->width/2,height()-indicator->height);

    searchfield = new QLineEdit(centralwidget);
    searchfield->setMinimumSize(search_width,search_height);
    searchfield->setMaximumSize(search_width,search_height);
    searchfield->setPlaceholderText("search");
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    pager->setGeometry(0,0,this->width(),this->height());
    indicator->move(width()*0.5-indicator->width/2,height()-indicator->height);
    active_indicator->move(width()*0.5-indicator->width/2,height()-indicator->height);
    searchfield->move((width()-search_width)/2,search_height*1.5);

    QPixmap bkgnd("grid.jpeg");
    bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
    QPalette palette;
    palette.setBrush(QPalette::Background, bkgnd);
    this->setPalette(palette);
}

MainWindow::~MainWindow()
{
    delete ui;
}
