#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDesktopWidget>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_layout = new QGridLayout();
    m_pager = new Pager(centralWidget());
    m_btn = new QPushButton("button",centralWidget());
    m_btn->setFixedSize(100,30);
    m_layout->addWidget(m_btn,0,0);
    m_layout->addWidget(m_pager,0,1,Qt::AlignCenter);
    //m_layout->setGeometry(geometry());
    centralWidget()->setLayout(m_layout);
    QPalette palette;
    palette.setColor(QPalette::ColorRole::Background,Qt::green);
    centralWidget()->setPalette(palette);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    //m_btn->setFixedSize(m_layout->cellRect(0,0).size());
    float ratio = (float)QApplication::desktop()->screenGeometry(0).height()/QApplication::desktop()->screenGeometry(0).width();
    //m_pager->setFixedSize(width(),width()*ratio);
    m_pager->setFixedWidth(width()*0.7);
    m_pager->setFixedHeight(m_pager->width()*ratio);
    qDebug() << centralWidget()->geometry() << m_pager->geometry() << ratio << QApplication::desktop()->screenGeometry().width();
}

MainWindow::~MainWindow()
{
    delete ui;
}
