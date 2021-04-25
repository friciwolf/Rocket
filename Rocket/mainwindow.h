#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include <QLockFile>
#include "pager/pager.h"
#include "pager/verticalpager.h"
#include "pager/pagercircularindicator.h"
#include "pager/pagercircularactiveindicator.h"
#include "searchfield/searchfield.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    Pager * pager;
    VerticalPager * verticalpager;

    PagerCircularIndicator * indicator;
    PagerCircularActiveIndicator * active_indicator;
    SearchField * searchfield;

    int search_width;
    int search_height;

    explicit MainWindow(QWidget *parent = nullptr);

    void resizeEvent(QResizeEvent *event);
    void leaveEvent(QEvent *event);
    void setStarupArgs(QStringList * args){m_startupargs = args;}
    QStringList * getStarupArgs(){return m_startupargs;}


    ~MainWindow();

public slots:
    void navigation(int key);
    void executeSelected();
    void dBusToggleWindowState(QString event);

private:
    Ui::MainWindow *ui;
    QStringList * m_startupargs;
};

#endif // MAINWINDOW_H
