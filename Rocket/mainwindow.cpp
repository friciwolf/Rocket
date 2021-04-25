#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pager/pager.h"
#include "pager/verticalpager.h"
#include "pager/pageritem.h"
#include "pager/pagercircularindicator.h"
#include "pager/pagercircularactiveindicator.h"
#include "stylingparams.h"

#include <QGridLayout>
#include <QPainter>
#include <QKeyEvent>
#include <QDebug>
#include <KRun>
#include <QTimer>
#include <QtDBus>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    setStarupArgs(new QStringList(qApp->arguments()));

    QDBusConnection connection = QDBusConnection::sessionBus();
    connection.interface()->registerService("com.friciwolf.rocket");
    connection.registerObject("/",this,QDBusConnection::ExportAllSlots);
    connection.connect("com.friciwolf.rocket","/","com.friciwolf.rocket","dBusToggleWindowState",this,SLOT(MainWindow::dBusToggleWindowState()));

    ui->setupUi(this);
    setMouseTracking(true);

    if (ConfigManager.getVerticalModeSetting())
    {
        verticalpager = new VerticalPager(this);

        indicator = new PagerCircularIndicator(this,verticalpager);
        connect(verticalpager,&VerticalPager::updated,indicator,&PagerCircularIndicator::setHidden);

        active_indicator = new PagerCircularActiveIndicator(this,indicator);
        connect(verticalpager,&VerticalPager::updated,active_indicator,&PagerCircularActiveIndicator::setHidden);

        searchfield = new SearchField(this);
        connect(searchfield,&QLineEdit::textEdited,verticalpager,&VerticalPager::activateSearch);
        connect(searchfield,&SearchField::navigate,this,&MainWindow::navigation);
        connect(searchfield,&QLineEdit::returnPressed,this,&MainWindow::executeSelected);
    }
    else
    {
        pager = new Pager(this);

        indicator = new PagerCircularIndicator(this,pager);
        connect(pager,&Pager::updated,indicator,&PagerCircularIndicator::setHidden);

        active_indicator = new PagerCircularActiveIndicator(this,indicator);
        connect(pager,&Pager::updated,active_indicator,&PagerCircularActiveIndicator::setHidden);

        searchfield = new SearchField(this);
        connect(searchfield,&QLineEdit::textEdited,pager,&Pager::activateSearch);
        connect(searchfield,&SearchField::navigate,this,&MainWindow::navigation);
        connect(searchfield,&QLineEdit::returnPressed,this,&MainWindow::executeSelected);
    }
}

void MainWindow::dBusToggleWindowState(QString event)
{
    if (event==QString("_toggle") && !getStarupArgs()->contains("--dbus"))
    {
        qApp->exit();
    }
    if (getStarupArgs()->contains("--dbus"))
    {
        getStarupArgs()->removeAt(getStarupArgs()->indexOf("--dbus"));
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    if (ConfigManager.getVerticalModeSetting())
    {
        verticalpager->setFixedSize(width(),height());
        indicator->positioning();
        active_indicator->positioning();
        searchfield->positioning();
    }
    else
    {
        pager->setFixedSize(width(),height());
        indicator->positioning();
        active_indicator->positioning();
        searchfield->positioning();
    }
}

void MainWindow::leaveEvent(QEvent *event)
{
    qApp->exit();
    event->accept();
}

void MainWindow::navigation(int key)
{
    IconGrid * grid;
    if (ConfigManager.getVerticalModeSetting())
    {
        grid = verticalpager->pages[verticalpager->current_element]->getIconGrid();
        if (grid->getNumberOfItems()==0) return; // "No Results found" - screen
        verticalpager->page_turned = false;
    }
    else
    {
        grid = pager->pages[pager->current_element]->getIconGrid();
        if (grid->getNumberOfItems()==0) return; // "No Results found" - screen
        pager->page_turned = false;
    }
    grid->unhighlightAll();
    int active = grid->getActiveElement();
    switch (key)
    {
        case Qt::Key::Key_Right:
        {
            grid->setActiveElement(active+1);
            break;
        }
        case Qt::Key::Key_Left:
        {
            grid->setActiveElement(active-1);
            break;
        }
        case Qt::Key::Key_Up:
        {
            grid->setActiveElement(active-grid->getMaxNumberOfColumns());
            break;
        }
        case Qt::Key::Key_Down:
        {
            if (active==-1) grid->setActiveElement(0);
            else grid->setActiveElement(active+grid->getMaxNumberOfColumns());
            break;
        }
        case Qt::Key::Key_Escape:
        {
            qApp->exit();
            break;
        }
        case Qt::Key::Key_Tab:
        {
            if (active==grid->getItems().size()-1 && (ConfigManager.getVerticalModeSetting() ? verticalpager->searching : pager->searching))
            {
                grid->setActiveElement(0);
            }
            else {
                grid->setActiveElement(active+1);
            }
            break;
        }
    }
    active = grid->getActiveElement();
    if (ConfigManager.getVerticalModeSetting())
    {
        if (active!=-1 && !verticalpager->page_turned) grid->highlight(active);
    }
    else
    {
        if (active!=-1 && !pager->page_turned) grid->highlight(active);
    }
}

void MainWindow::executeSelected()
{
    IconGrid * grid;
    if (ConfigManager.getVerticalModeSetting())
    {
        grid = verticalpager->pages[verticalpager->current_element]->getIconGrid();
    }
    else {
        grid = pager->pages[pager->current_element]->getIconGrid();
    }
    int active = grid->getActiveElement();
    if (active==-1) return;
    KApplication application = grid->getItems()[active]->getApplication();
    QList<QUrl> urls;
    if (application.terminal())
    {
        if(KRun::run("konsole -e "+application.exec(),urls,nullptr,application.name(),application.iconname()))
        {
            qApp->exit();
        }
    }
    else {
        if(KRun::run(application.exec(),urls,nullptr,application.name(),application.iconname())) {
            qApp->exit();
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
