#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "pager/pager.h"

#include <QMainWindow>
#include <QPushButton>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *event);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Pager * m_pager;
    QGridLayout * m_layout;
    QPushButton * m_btn;
};

#endif // MAINWINDOW_H
