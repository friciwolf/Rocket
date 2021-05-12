#include <QApplication>
#include <QDebug>
#include <QDir>

#include "mainwindow.h"

#include "../RocketLibrary/rocketlibrary.h"
#include "../RocketLibrary/tools/rocketconfigmanager.h"

extern RocketConfigManager ConfigManager = RocketConfigManager();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString name = QString("rocket");

    KConfig styleconfig(QDir::homePath()+"/.config/"+name+"/"+name+"style",KConfig::OpenFlag::SimpleConfig);
    KConfig appgridconfig(QDir::homePath()+"/.config/"+name+"/"+name+"appgrid",KConfig::OpenFlag::SimpleConfig);

    ConfigManager.setStyleConfig(&styleconfig);
    ConfigManager.checkStyleConfigFile();
    ConfigManager.setAppGridConfig(&appgridconfig);
    ConfigManager.checkAppGridConfigFile();

    MainWindow w;
    w.show();

    return a.exec();
}
