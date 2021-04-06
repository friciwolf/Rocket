#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>
#include "pager/pager.h"
#include "pager/pagercircularindicator.h"
#include "pager/pagercircularactiveindicator.h"

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
    QLineEdit * searchfield;

    int search_width = 300;
    int search_height = 30;

    explicit MainWindow(QWidget *parent = nullptr);

    void resizeEvent(QResizeEvent *event);

    ~MainWindow();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
