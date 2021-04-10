#include "rocketconfigmanager.h"
#include <KConfigSkeleton>
#include <QPalette>

class DefaultGroupsAndKeys {
public:
    map<QString,map<QString,QVariant>> items;
    DefaultGroupsAndKeys()
    {
        map<QString,QVariant> colors;
        colors["base"] = RocketStyle::BaseColour;
        colors["secondary"] = RocketStyle::SecondaryColour;
        items["color"] = colors;

        map<QString,QVariant> font;
        font["size1"] = QString::number(RocketStyle::fontsize1);
        font["size2"] = QString::number(RocketStyle::fontsize2);
        items["font"] = font;

        map<QString,QVariant> dimensions;
        dimensions["rows"] = QString::number(RocketStyle::m_rows);
        dimensions["columns"] = QString::number(RocketStyle::m_cols);
        items["dimensions"] = dimensions;
    }
};


RocketConfigManager::RocketConfigManager()
{

}

void RocketConfigManager::checkStyleConfigFile()
{
    /*
     *  This function checks whether the config file exists and contains the neccessary key-value pairs.
     *  It does not check however, whether they are valid.
     * */
    KConfig * config = m_styleconfig;
    for (auto groups : DefaultGroupsAndKeys().items)
    {
        QString group = groups.first;
        if (!config->hasGroup(group))
        {
            for (auto child : groups.second)
            {
                config->group(group).writeEntry(child.first,child.second);
            }
        }
        else {
            for (auto child : groups.second)
            {
                if (!config->group(group).hasKey(child.first))
                {
                    config->group(group).writeEntry(child.first,child.second);
                }
            }
        }
    }
}

QString RocketConfigManager::getStyleValue(QString group, QString key)
{
    return getStyleConfig()->group(group).readEntry(key);
}

QColor RocketConfigManager::getBaseColour()
{
    QString res = getStyleValue("color","base");
    QStringList r = res.split(",");
    if (r.size()==4) return QColor(r[0].toInt(),r[1].toInt(),r[2].toInt(),r[3].toInt());
    if (r.size()==3) return QColor(r[0].toInt(),r[1].toInt(),r[2].toInt(),255);
    else
    {
        qDebug() << "Invalid argument for 'color' and 'base'. Falling back to defaults...";
        return RocketStyle::BaseColour;
    }
}

QColor RocketConfigManager::getSecondaryColour()
{
    QString res = getStyleValue("color","secondary");
    QStringList r = res.split(",");
    if (r.size()==4) return QColor(r[0].toInt(),r[1].toInt(),r[2].toInt(),r[3].toInt());
    if (r.size()==3) return QColor(r[0].toInt(),r[1].toInt(),r[2].toInt(),255);
    else
    {
        qDebug() << "Invalid argument for 'color' and 'secondary'. Falling back to defaults...";
        return RocketStyle::SecondaryColour;
    }
}

QPalette RocketConfigManager::getBaseColourBackgroundPalette()
{
    QPalette p;
    p.setColor(QPalette::ColorRole::Background,getBaseColour());
    return p;
}

QBrush RocketConfigManager::getActiveIndicatorBrush()
{
    QString res = getStyleValue("color","secondary");
    QStringList r = res.split(",");
    if (r.size()==4) return QBrush(QColor(r[0].toInt(),r[1].toInt(),r[2].toInt(),r[3].toInt()),Qt::BrushStyle::SolidPattern);
    if (r.size()==3) return QBrush(QColor(r[0].toInt(),r[1].toInt(),r[2].toInt(),255),Qt::BrushStyle::SolidPattern);
    else
    {
        qDebug() << "Invalid argument for 'color' and 'secondary'. Falling back to defaults...";
        return RocketStyle::active_indicator_brush;
    }
}

int RocketConfigManager::getFontSize1()
{
    int size = getStyleValue("font","size1").toInt();
    if (size>0) return size;
    else {
        qDebug() << "Invalid argument for 'font' and 'size1'. Falling back to defaults...";
        return RocketStyle::fontsize1;
    }
}

int RocketConfigManager::getFontSize2()
{
    int size = getStyleValue("font","size2").toInt();
    if (size>0) return size;
    else {
        qDebug() << "Invalid argument for 'font' and 'size2'. Falling back to defaults...";
        return RocketStyle::fontsize2;
    }
}

int RocketConfigManager::getRowNumber()
{
    int rows = getStyleValue("dimensions","rows").toInt();
    if (rows>0) return rows;
    else
    {
        qDebug() << "Invalid argument for 'dimensions' and 'rows'. Falling back to defaults...";
        return RocketStyle::m_rows;
    }
}

int RocketConfigManager::getColumnNumber()
{
    int cols = getStyleValue("dimensions","columns").toInt();
    if (cols>1) return cols;
    else
    {
        qDebug() << "Invalid argument for 'dimensions' and 'columns'. Falling back to defaults...";
        if (cols==1) qDebug() << "only columns more than one are supported...";
        return RocketStyle::m_cols;
    }
}
