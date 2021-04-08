#include "mainwindow.h"
#include <QApplication>
#include <QPropertyAnimation>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowOpacity(0);
    w.showFullScreen();

    QPropertyAnimation * animation = new QPropertyAnimation(&w,"windowOpacity");
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->setDuration(500);
    //animation->setEasingCurve(QEasingCurve::OutQuad);
    animation->start();
    return a.exec();
}
