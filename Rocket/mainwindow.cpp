#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "pager/pager.h"
#include "pager/pageritem.h"
#include "pager/pagercircularindicator.h"
#include "pager/pagercircularactiveindicator.h"
#include "stylingparams.h"

#include <QGridLayout>
#include <QPainter>
#include <QKeyEvent>
#include <QDebug>
#include <KRun>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    pager = new Pager(this);

    indicator = new PagerCircularIndicator(this,pager);

    active_indicator = new PagerCircularActiveIndicator(this,indicator);

    searchfield = new SearchField(this);
    connect(searchfield,&QLineEdit::textEdited,pager,&Pager::activateSearch);
    connect(pager,&Pager::updated,indicator,&PagerCircularIndicator::setHidden);
    connect(pager,&Pager::updated,active_indicator,&PagerCircularActiveIndicator::setHidden);
    connect(searchfield,&SearchField::navigate,this,&MainWindow::navigation);
    connect(searchfield,&QLineEdit::returnPressed,this,&MainWindow::executeSelected);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    pager->setFixedSize(width(),height());
    indicator->positioning();
    active_indicator->positioning();
    searchfield->positioning();
}


void MainWindow::navigation(int key)
{
    IconGrid * grid = pager->pages[pager->current_element]->getIconGrid();
    if (grid->getNumberOfItems()==0) return; // "No Results found" - screen
    pager->page_turned = false;
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
            if (active==grid->getItems().size()-1 && pager->searching)
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
    if (active!=-1 && !pager->page_turned)
    {
        grid->highlight(active);
    }
}

void MainWindow::executeSelected()
{
    IconGrid * grid = pager->pages[pager->current_element]->getIconGrid();
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

void MainWindow::wheelEvent(QWheelEvent *event)
{
    if (event->angleDelta().y()>0)
    {
        pager->goToPage(1);
    }
    else
    {
        pager->goToPage(-1);
    }
    pager->pages[pager->current_element]->getIconGrid()->resetHighlightAndActiveElement();
}


MainWindow::~MainWindow()
{
    delete ui;
}
