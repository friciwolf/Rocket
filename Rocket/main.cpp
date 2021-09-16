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

    QObject::connect(qApp, &QGuiApplication::applicationStateChanged, &w, [=](Qt::ApplicationState state){
        if (state==Qt::ApplicationState::ApplicationInactive)
        {
            qDebug() << "exit application lost focus " << QGuiApplication::applicationState() << qApp->activeWindow()->isModal();
            qApp->exit();
        }
    });

    return a.exec();
}
