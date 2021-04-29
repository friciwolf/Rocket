#include "mainwindow.h"
#include "tools/rocketconfigmanager.h"
#include <QApplication>
#include <QPropertyAnimation>
#include <QDir>
#include <KConfigGroup>
#include <QDebug>
#include <QLockFile>
#include <QProcess>

extern RocketConfigManager ConfigManager = RocketConfigManager();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString name = a.applicationName().toLower();
    QLockFile mainlockFile(QDir::homePath()+"/.config/"+name+"/."+name+"main.lock");
    if (!mainlockFile.tryLock(100))
    {
        QProcess p;
        p.start("pkill rocket");
        p.waitForFinished(1000);
        return 1;
    }

    KConfig styleconfig(QDir::homePath()+"/.config/"+name+"/"+name+"style",KConfig::OpenFlag::SimpleConfig);
    KConfig appgridconfig(QDir::homePath()+"/.config/"+name+"/"+name+"appgrid",KConfig::OpenFlag::SimpleConfig);

    ConfigManager.setStyleConfig(&styleconfig);
    ConfigManager.checkStyleConfigFile();

    ConfigManager.setAppGridConfig(&appgridconfig);
    ConfigManager.checkAppGridConfigFile();

    MainWindow w;
    w.showFullScreen();

    return a.exec();
}
