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

void RocketConfigManager::checkAppGridConfigFile()
{
    KConfig * config = m_appgridconfig;
    if (config->groupList().size()==0)
    {
        qDebug() << "Appgrid configuration not found. Generating one...";
        KMenuItems m_menuitems;
        m_menuitems.scanElements();
        int i = 0;
        for (KApplication app : m_menuitems.getApplications())
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
            config->group("Entry"+QString::number(i)).writeEntry("pos",QString::number(i));
            i++;
        }
        m_apps = m_menuitems.getApplications();
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
            int pos = config->group(item).readEntry("pos").toInt();
            apps[pos] = KApplication(name,iconname,icon,exec,comment,terminal,keywords,genericname,untranslatedGenericName,categories);
        }
        std::vector<KApplication> appvector(apps,apps+N);
        m_apps = appvector;
    }
}
