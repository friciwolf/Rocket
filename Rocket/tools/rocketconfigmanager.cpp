#include "rocketconfigmanager.h"
#include <KConfigSkeleton>
#include <QPalette>
#include <iterator>
#include <vector>

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

        map<QString,QVariant> background;
        background["use_system_wallpaper"] = QString((RocketStyle::use_system_wallpaper) ? "true" : "false");
        background["wallpaper_of_screen"] = QString::number(RocketStyle::use_system_wallpaper_screen);
        items["background"] = background;
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
        qDebug() << "Invalid argument for 'base'. Falling back to defaults...";
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
        qDebug() << "Invalid argument for 'secondary'. Falling back to defaults...";
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
        qDebug() << "Invalid argument for 'secondary'. Falling back to defaults...";
        return RocketStyle::active_indicator_brush;
    }
}

int RocketConfigManager::getFontSize1()
{
    int size = getStyleValue("font","size1").toInt();
    if (size>0) return size;
    else {
        qDebug() << "Invalid argument for 'size1'. Falling back to defaults...";
        return RocketStyle::fontsize1;
    }
}

int RocketConfigManager::getFontSize2()
{
    int size = getStyleValue("font","size2").toInt();
    if (size>0) return size;
    else {
        qDebug() << "Invalid argument for 'size2'. Falling back to defaults...";
        return RocketStyle::fontsize2;
    }
}

int RocketConfigManager::getRowNumber()
{
    int rows = getStyleValue("dimensions","rows").toInt();
    if (rows>0) return rows;
    else
    {
        qDebug() << "Invalid argument for 'rows'. Falling back to defaults...";
        return RocketStyle::m_rows;
    }
}

int RocketConfigManager::getColumnNumber()
{
    int cols = getStyleValue("dimensions","columns").toInt();
    if (cols>1) return cols;
    else
    {
        qDebug() << "Invalid argument for 'columns'. Falling back to defaults...";
        if (cols==1) qDebug() << "only columns more than one are supported...";
        return RocketStyle::m_cols;
    }
}

bool RocketConfigManager::getUsingSystemWallpaper()
{
    KConfig * config = m_styleconfig;
    QString value = m_styleconfig->group("background").readEntry("use_system_wallpaper");
    if (value==QString("true")) return true;
    if (value==QString("false")) return false;
    qDebug() <<  "Invalid argument for 'use_system_wallpaper'. Falling back to defaults...";
    return RocketStyle::use_system_wallpaper;
}

int RocketConfigManager::getWallpaperScreen()
{
    KConfig * config = m_styleconfig;
    QString value = m_styleconfig->group("background").readEntry("wallpaper_of_screen");
    return value.toInt(); //returns 0 as default if a random string is entered.
}

void RocketConfigManager::checkAppGridConfigFile()
{
    KConfig * config = m_appgridconfig;
    if (config->groupList().size()==0)
    {
        qDebug() << "Appgrid configuration not found. Generating one...";
        KMenuItems menuitems;
        menuitems.scanElements();
        generateAppGridConfigFile(config,menuitems);
    }
    else
    {
        int N = config->groupList().size();
        KApplication apps[N];
        for (QString item : config->groupList())
        {
            QString name = config->group(item).readEntry("name");
            QString iconname = config->group(item).readEntry("iconname");
            QString exec = config->group(item).readEntry("exec");
            QString comment = config->group(item).readEntry("comment");
            QIcon icon = QIcon::fromTheme(iconname);
            QStringList keywords = config->group(item).readEntry("keywords").split(",");
            QString genericname = config->group(item).readEntry("genericname");
            QString untranslatedGenericName = config->group(item).readEntry("untranslatedgenericname");
            QStringList categories = config->group(item).readEntry("categories").split(",");
            bool terminal = (config->group(item).readEntry("terminal") == "false" ? false : true);
            QString entrypath = config->group(item).readEntry("entrypath");
            int pos = config->group(item).readEntry("pos").toInt();
            apps[pos] = KApplication(name,iconname,icon,exec,comment,terminal,keywords,genericname,untranslatedGenericName,categories,entrypath);
        }
        std::vector<KApplication> appvector(apps,apps+N);
        m_apps = appvector;
    }
}

void RocketConfigManager::generateAppGridConfigFile(KConfig * config,KMenuItems menuitems)
{
    int i = 0;
    for (KApplication app : menuitems.getApplications())
    {
        config->group("Entry"+QString::number(i)).writeEntry("name",app.name());
        config->group("Entry"+QString::number(i)).writeEntry("iconname",app.iconname());
        config->group("Entry"+QString::number(i)).writeEntry("exec",app.exec());
        config->group("Entry"+QString::number(i)).writeEntry("comment",app.comment());
        config->group("Entry"+QString::number(i)).writeEntry("terminal",app.terminal());
        config->group("Entry"+QString::number(i)).writeEntry("keywords",app.keywords());
        config->group("Entry"+QString::number(i)).writeEntry("genericname",app.genericname());
        config->group("Entry"+QString::number(i)).writeEntry("untranslatedgenericname",app.untranslatedGenericName());
        config->group("Entry"+QString::number(i)).writeEntry("categories",app.categories());
        config->group("Entry"+QString::number(i)).writeEntry("entrypath",app.entrypath());
        config->group("Entry"+QString::number(i)).writeEntry("pos",QString::number(i));
        i++;
    }
    m_apps = menuitems.getApplications();
}

bool RocketConfigManager::updateApplicationList()
{
    KMenuItems menuitems;
    menuitems.scanElements();

    if (menuitems.getApplications().size()!=m_apps.size())
    {
        qDebug() << "Appgrid configuration changed. Generating new configuration file...";
        for (QString group : m_appgridconfig->groupList())
        {
            m_appgridconfig->deleteGroup(group);
        }
        generateAppGridConfigFile(m_appgridconfig,menuitems);
        m_appgridconfig->reparseConfiguration();
        return true;
    }
    return false;
}
