#include <QGridLayout>
#include <QPainter>
#include <QKeyEvent>
#include <QDebug>
#include <QTimer>
#include <QtDBus>

#include <KRun>
#include <KService>
#include <KDesktopFile>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pager/pager.h"
#include "pager/verticalpager.h"
#include "pager/pageritem.h"
#include "pager/pagercircularindicator.h"
#include "pager/pagercircularactiveindicator.h"

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
    setWindowOpacity(0);

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

    // comparing app lists might take a while, so start it in a new thread...
    ComparingApps * comparerthread = new ComparingApps();
    connect(comparerthread, &ComparingApps::pagerUpdateNeeded,this,&MainWindow::pagerUpdaterMethod);
    connect(comparerthread, &ComparingApps::finished,comparerthread,&QObject::deleteLater);
    comparerthread->start();
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

void MainWindow::pagerUpdaterMethod()
{
    pager->setApplicationList(ConfigManager.getApplications());
    pager->updatePager(ConfigManager.getApplications());
    qDebug() << "application list updated";
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    // if not in fullscreen, don't bother constructing the pager...
    if (size().width()<17)
    {
        event->accept();
        return;
    }
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

    setWindowOpacity(0);
    QPropertyAnimation * animation = new QPropertyAnimation(this,"windowOpacity");
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->setDuration(150);
    animation->start();
}

void MainWindow::leaveEvent(QEvent *event)
{
    if (!pager->isIconDraggingOn())
    {
        qApp->exit();
        event->accept();
    }
}

void MainWindow::navigation(int key)
{
    if (key==Qt::Key::Key_Escape) qApp->exit();
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
    KDEApplication application = grid->getItems()[active]->getApplication();
    QList<QUrl> urls;
    KDesktopFile d(application.entrypath());
    KService s(&d,application.entrypath());
    if (KRun::run(s,urls,nullptr))
    {
        qApp->exit();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
