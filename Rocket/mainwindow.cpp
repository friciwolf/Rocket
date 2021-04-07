#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pager/pager.h"
#include "pager/pageritem.h"
#include "pager/pagercircularindicator.h"
#include "pager/pagercircularactiveindicator.h"
#include "stylingparams.h"

#include <QGridLayout>
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

    searchfield = new SearchField(centralwidget,search_width,search_height);
    connect(searchfield,&QLineEdit::textEdited,pager,&Pager::activateSearch);
    connect(pager,&Pager::updated,indicator,&PagerCircularIndicator::setHidden);
    connect(pager,&Pager::updated,active_indicator,&PagerCircularActiveIndicator::setHidden);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    pager->setGeometry(0,0,this->width(),this->height());
    indicator->move(width()*0.5-indicator->width/2,height()-indicator->height);
    active_indicator->move(width()*0.5-indicator->width/2,height()-indicator->height);
    searchfield->move((width()-search_width)/2,search_height*1.5);
}

MainWindow::~MainWindow()
{
    delete ui;
}
