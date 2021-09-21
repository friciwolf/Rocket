#include <QApplication>
#include <QPropertyAnimation>
#include <QDir>
#include <QDebug>
#include <QLockFile>
#include <QProcess>

#include <KConfigGroup>

#include "mainwindow.h"

#include "../RocketLibrary/tools/rocketconfigmanager.h"

extern RocketConfigManager ConfigManager = RocketConfigManager();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString name = a.applicationName().toLower();

    // Create .config folder
    if (!QDir(QDir::homePath()+"/.config/"+name).exists())
        QDir().mkdir(QDir::homePath()+"/.config/"+name);

    // Check if another instance of Rocket is running
    QLockFile mainlockFile(QDir::homePath()+"/.config/"+name+"/."+name+"main.lock");
    if (!mainlockFile.tryLock(100))
    {
        // Kill Rocket if already running and exit
        QProcess p;
        p.start("pkill rocket");
        p.waitForFinished(1000);
        return 1;
    }

    // Loading settings and show main window
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
