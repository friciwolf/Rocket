#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include "pager/pager.h"
#include "pager/pagercircularindicator.h"
#include "pager/pagercircularactiveindicator.h"
#include "searchfield/searchfield.h"
#include "stylingparams.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    Pager * pager;
    PagerCircularIndicator * indicator;
    PagerCircularActiveIndicator * active_indicator;
    SearchField * searchfield;

    int search_width = RocketStyle::searchfield_width;
    int search_height = RocketStyle::searchfield_height;

    explicit MainWindow(QWidget *parent = nullptr);

    void resizeEvent(QResizeEvent *event);
    void wheelEvent(QWheelEvent *event);


    ~MainWindow();

public slots:
    void navigation(int key);
    void executeSelected();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
