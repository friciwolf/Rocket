#include <QGridLayout>
#include <QPainter>
#include <QKeyEvent>
#include <QDebug>
#include <QTimer>
#include <QtDBus>

#include <KRun>
#include <KService>
#include <KDesktopFile>
#include <KWindowEffects>
#include <KIO/JobUiDelegate>
#include <KIO/ApplicationLauncherJob>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pager/horizontalpager.h"
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
    setWindowOpacity(1);

    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_NoSystemBackground);
    KWindowEffects::enableBlurBehind(winId());

    if (ConfigManager.getVerticalModeSetting())
    {
        verticalpager = new VerticalPager(this, ConfigManager.getApplicationTree(), false);

        indicator = new PagerCircularIndicator(this,verticalpager);
        connect(verticalpager,&VerticalPager::hideCircularIndicator,indicator,&PagerCircularIndicator::setHidden);

        active_indicator = new PagerCircularActiveIndicator(this,indicator);
        connect(verticalpager,&VerticalPager::hideCircularIndicator,active_indicator,&PagerCircularActiveIndicator::setHidden);

        searchfield = new SearchField(this);
        connect(verticalpager,&VerticalPager::setSearchbarVisibility,searchfield,&QLineEdit::setVisible);
        connect(searchfield,&QLineEdit::textEdited,verticalpager,&VerticalPager::activateSearch);
        connect(searchfield,&SearchField::navigate,this,&MainWindow::navigation);
        connect(searchfield,&QLineEdit::returnPressed,this,&MainWindow::executeSelected);
    }
    else
    {
        pager = new HorizontalPager(this, ConfigManager.getApplicationTree(), false);

        indicator = new PagerCircularIndicator(this,pager);
        connect(pager,&HorizontalPager::hideCircularIndicator,indicator,&PagerCircularIndicator::setHidden);

        active_indicator = new PagerCircularActiveIndicator(this,indicator);
        connect(pager,&HorizontalPager::hideCircularIndicator,active_indicator,&PagerCircularActiveIndicator::setHidden);

        searchfield = new SearchField(this);
        connect(pager,&HorizontalPager::setSearchbarVisibility,searchfield,&QLineEdit::setVisible);
        connect(searchfield,&QLineEdit::textEdited,pager,&HorizontalPager::activateSearch);
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
    if (ConfigManager.getVerticalModeSetting())
    {
        verticalpager->setApplicationList(ConfigManager.getApplications());
        verticalpager->setApplicationTree(ConfigManager.getApplicationTree());
        verticalpager->updatePager(ConfigManager.getApplicationTree());
    }
    else
    {
        pager->setApplicationList(ConfigManager.getApplications());
        pager->setApplicationTree(ConfigManager.getApplicationTree());
        pager->updatePager(ConfigManager.getApplicationTree());
    }
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
}

void MainWindow::navigation(int key)
{
    if (key==Qt::Key::Key_Escape)
    {
        if (ConfigManager.getVerticalModeSetting())
        {
            if (verticalpager->in_subfolder == false)
                qApp->exit();
            else
            {
                verticalpager->updatePager(verticalpager->getApplicationTree());
                verticalpager->hideCircularIndicator(false);
                verticalpager->enableIconDragging(true);
                verticalpager->setSearchbarVisibility(true);
                int index = 0;
                int i = 0;
                for (KDEApplication app : verticalpager->getApplicationTree())
                    if (app==verticalpager->in_subfolder_app)
                    {
                        index=i;
                        break;
                    }
                    else i++;
                verticalpager->pages[verticalpager->current_element]->getIconGrid()->highlight((index%verticalpager->pages[0]->getIconGrid()->getItems().size()));
                verticalpager->in_subfolder = false;
                return;
            }
        }
        else
        {
            if (pager->in_subfolder==false)
                qApp->exit();
            else
            {
                pager->updatePager(pager->getApplicationTree());
                pager->hideCircularIndicator(false);
                pager->enableIconDragging(true);
                pager->setSearchbarVisibility(true);
                int index = 0;
                int i = 0;
                for (KDEApplication app : pager->getApplicationTree())
                    if (app==pager->in_subfolder_app)
                    {
                        index=i;
                        break;
                    }
                    else i++;
                pager->pages[pager->current_element]->getIconGrid()->highlight((index%pager->pages[0]->getIconGrid()->getItems().size()));
                pager->in_subfolder = false;
                return;
            }
        }
    }
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
    if (!application.isFolder())
    {
        KDesktopFile d(application.entrypath());
        KService s(&d,application.entrypath());
        KService::Ptr p(&s);
        KIO::ApplicationLauncherJob * job = new KIO::ApplicationLauncherJob(p);
        job->setUiDelegate(new KIO::JobUiDelegate(KJobUiDelegate::AutoHandlingEnabled, this));
        job->setAutoDelete(true);
        job->start();
        qDebug() << "MainWindow: Executing " + s.entryPath() + " " + s.exec();
    }
    else
    {
        if (ConfigManager.getVerticalModeSetting())
        {
            verticalpager->folderClickEvent(application);
            verticalpager->pages[verticalpager->current_element]->getIconGrid()->highlight(0);
        }
        else {
            pager->folderClickEvent(application);
            pager->pages[pager->current_element]->getIconGrid()->highlight(0);
        }
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
