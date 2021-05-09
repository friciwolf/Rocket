#ifndef ROCKETCONFIGMANAGER_H
#define ROCKETCONFIGMANAGER_H

#include <QString>
#include <KConfig>
#include <QDebug>
#include <QColor>
#include "icongrid/kmenuitems.h"

using namespace std;

class RocketConfigManager {
public:
    RocketConfigManager();
    void setStyleConfig(KConfig * config){m_styleconfig = config;}
    KConfig * getStyleConfig(){return m_styleconfig;}
    void checkStyleConfigFile();
    void setAppGridConfig(KConfig * config){m_appgridconfig = config;}
    KConfig * getAppGridConfig(){return m_appgridconfig;}
    void checkAppGridConfigFile();
    QString getStyleValue(QString group, QString key);
    QColor getStyleValue(QString group, QString key, QColor defaultvalue);
    int getStyleValue(QString group, QString key, int defaultvalue);
    bool getStyleValue(QString group, QString key, bool defaultvalue);
    QColor getBaseColour();
    QColor getSecondaryColour();
    QPalette getBaseColourBackgroundPalette();
    QBrush getActiveIndicatorBrush();
    int getFontSize1();
    int getFontSize2();
    int getRowNumber();
    int getColumnNumber();
    bool getVerticalModeSetting();
    bool getUsingSystemWallpaper();
    int getWallpaperScreen();
    int getBlurRadius();
    int getInvertedScrollFactorXfromSettings();
    int getInvertedScrollFactorYfromSettings();
    std::vector<KApplication> getApplications(){return m_apps;}
    bool updateApplicationList();
private:
    KConfig * m_styleconfig;
    KConfig * m_appgridconfig;
    std::vector<KApplication> m_apps;
    void generateAppGridConfigFile(KConfig * config,KMenuItems menuitems);
};

extern RocketConfigManager ConfigManager;

#endif // ROCKETCONFIGMANAGER_H
