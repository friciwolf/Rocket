#include "mainwindow.h"
#include "tools/rocketconfigmanager.h"
#include <QApplication>
#include <QPropertyAnimation>
#include <QDir>
#include <KConfigGroup>
#include <QDebug>

extern RocketConfigManager ConfigManager = RocketConfigManager();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //a.setStartDragTime(3000);

    QString name = a.applicationName().toLower();

    KConfig styleconfig(QDir::homePath()+"/.config/"+name+"/"+name+"style",KConfig::OpenFlag::SimpleConfig);
    KConfig appgridconfig(QDir::homePath()+"/.config/"+name+"/"+name+"appgrid",KConfig::OpenFlag::SimpleConfig);

    ConfigManager.setStyleConfig(&styleconfig);
    ConfigManager.checkStyleConfigFile();
    ConfigManager.setAppGridConfig(&appgridconfig);
    ConfigManager.checkAppGridConfigFile();

    MainWindow w;

    w.setWindowOpacity(0);
    w.showFullScreen();


    QPropertyAnimation * animation = new QPropertyAnimation(&w,"windowOpacity");
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->setDuration(150);
    //animation->setEasingCurve(QEasingCurve::OutQuad);
    animation->start();

    if(ConfigManager.updateApplicationList())
    {
        w.pager->setApplicationList(ConfigManager.getApplications());
        w.pager->updatePager(ConfigManager.getApplications());
        qDebug() << "application list updated";
    }

    return a.exec();
}
