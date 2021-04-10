#ifndef ROCKETCONFIGMANAGER_H
#define ROCKETCONFIGMANAGER_H

#include <QString>
#include <KConfig>
#include <QDebug>
#include <QColor>
#include "stylingparams.h"

using namespace std;

class RocketConfigManager {
public:
    RocketConfigManager();
    void setStyleConfig(KConfig * config){m_styleconfig = config;}
    KConfig * getStyleConfig(){return m_styleconfig;}
    void checkStyleConfigFile();
    QString getStyleValue(QString group, QString key);
    QColor getBaseColour();
    QColor getSecondaryColour();
    QPalette getBaseColourBackgroundPalette();
    QBrush getActiveIndicatorBrush();
    int getFontSize1();
    int getFontSize2();
    int getRowNumber();
    int getColumnNumber();
private:
    KConfig * m_styleconfig;
};

extern RocketConfigManager ConfigManager;

#endif // ROCKETCONFIGMANAGER_H
