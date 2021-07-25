#include "../rocketlibrary.h"
#include "rocketconfigmanager.h"
#include <KConfigSkeleton>
#include <QPalette>
#include <iterator>
#include <vector>

using namespace RocketConfig;

class DefaultGroupsAndKeys {
public:
    map<QString,map<QString,QVariant>> items;
    DefaultGroupsAndKeys()
    {
        map<QString,QVariant> colors;
        colors[Color::base] = RocketStyle::BaseColour;
        colors[Color::secondary] = RocketStyle::SecondaryColour;
        colors[Color::selection] = RocketStyle::SelectionColour;
        items[Color::group] = colors;

        map<QString,QVariant> font;
        font[Font::size1] = RocketStyle::fontsize1;
        font[Font::size2] = RocketStyle::fontsize2;
        items[Font::group] = font;

        map<QString,QVariant> dimensions;
        dimensions[Dimensions::rows] = RocketStyle::m_rows;
        dimensions[Dimensions::columns] = RocketStyle::m_cols;
        dimensions[Dimensions::verticalorientation] = RocketStyle::pager_vertical_orientation;
        items[Dimensions::group] = dimensions;

        map<QString,QVariant> background;
        background[Background::usesystemwallpaper] = RocketStyle::use_system_wallpaper;
        background[Background::wallpaperofscreen] = RocketStyle::use_system_wallpaper_screen;
        background[Background::blurradius] = RocketStyle::blurradius;
        items[Background::group] = background;

        map<QString,QVariant> settings;
        settings[Settings::invertedscrollingx] = RocketStyle::inverted_scrolling_x;
        settings[Settings::invertedscrollingy] = RocketStyle::inverted_scrolling_y;
        settings[Settings::enable_boxes] = RocketStyle::enable_boxes;
        settings[Settings::tightlayout] = RocketStyle::tightlayout;
        settings[Settings::enable_folders] = RocketStyle::enable_folders;
        items[Settings::group] = settings;
    }
};


RocketConfigManager::RocketConfigManager()
{

}

void RocketConfigManager::checkStyleConfigFile()
{
    /*
     *  This function checks whether the config file exists and if it contains the neccessary key-value pairs.
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

QColor RocketConfigManager::getStyleValue(QString group, QString key, QColor defaultvalue)
{
    return getStyleConfig()->group(group).readEntry(key,defaultvalue);
}

int RocketConfigManager::getStyleValue(QString group, QString key, int defaultvalue)
{
    return getStyleConfig()->group(group).readEntry(key,defaultvalue);
}

bool RocketConfigManager::getStyleValue(QString group, QString key, bool defaultvalue)
{
    bool value = getStyleConfig()->group(group).readEntry(key,defaultvalue);
    QString str = getStyleValue(group,key);
    if (str==QString("true") || str==QString("false")) return value;
    else {
        qDebug() << "Invalid argument for" << key << "(" << str << "). Falling back to defaults...";
        return defaultvalue;
    }
}

QColor RocketConfigManager::getBaseColour()
{
    return getStyleValue(Color::group,Color::base,RocketStyle::BaseColour);
}

QColor RocketConfigManager::getSecondaryColour()
{
    return getStyleValue(Color::group,Color::secondary,RocketStyle::SecondaryColour);
}

QColor RocketConfigManager::getSelectionColour()
{
    return getStyleValue(Color::group,Color::selection,RocketStyle::SelectionColour);
}

QPalette RocketConfigManager::getBaseColourBackgroundPalette()
{
    QPalette p;
    p.setColor(QPalette::ColorRole::Background,getBaseColour());
    return p;
}

QPalette RocketConfigManager::getSelectionColourBackgroundPalette()
{
    QPalette p;
    p.setColor(QPalette::ColorRole::Background,getSelectionColour());
    return p;
}

int RocketConfigManager::getFontSize1()
{
    int size = getStyleValue(Font::group,Font::size1,RocketStyle::fontsize1);
    if (size>0) return size;
    else {
        qDebug() << "Invalid argument for" << Font::size1 << "(" << size << "). Falling back to defaults...";
        return RocketStyle::fontsize1;
    }
}

int RocketConfigManager::getFontSize2()
{
    int size = getStyleValue(Font::group,Font::size2,RocketStyle::fontsize2);
    if (size>0) return size;
    else {
        qDebug() << "Invalid argument for" << Font::size2 << "(" << size << "). Falling back to defaults...";
        return RocketStyle::fontsize2;
    }
}

int RocketConfigManager::getBlurRadius()
{
    int radius= getStyleValue(Background::group,Background::blurradius,RocketStyle::blurradius);
    if (radius>=0) return radius;
    else {
        qDebug() << "Invalid argument for" << Background::blurradius << "(" << radius << "). Falling back to defaults...";
        return RocketStyle::blurradius;
    }
}

bool RocketConfigManager::getBoxSetting()
{
    return getStyleValue(Settings::group,Settings::enable_boxes,RocketStyle::enable_boxes);
}

bool RocketConfigManager::getTightLayoutSetting()
{
    return getStyleValue(Settings::group,Settings::tightlayout,RocketStyle::tightlayout);
}

bool RocketConfigManager::getFolderSetting()
{
    return getStyleValue(Settings::group,Settings::enable_folders,RocketStyle::enable_folders);
}

int RocketConfigManager::getRowNumber()
{
    int rows = getStyleValue(Dimensions::group,Dimensions::rows,RocketStyle::m_rows);
    if (rows>0) return rows;
    else
    {
        qDebug() << "Invalid argument for" << Dimensions::rows << "(" << rows << "). Falling back to defaults...";
        return RocketStyle::m_rows;
    }
}

int RocketConfigManager::getColumnNumber()
{
    int cols = getStyleValue(Dimensions::group,Dimensions::columns,RocketStyle::m_cols);
    if (cols>1) return cols;
    else
    {
        qDebug() << "Invalid argument for" << Dimensions::columns << "(" << cols << "). Falling back to defaults...";
        if (cols==1) qDebug() << "only columns more than one are supported...";
        return RocketStyle::m_cols;
    }
}

bool RocketConfigManager::getVerticalModeSetting()
{
    return getStyleValue(Dimensions::group,Dimensions::verticalorientation,RocketStyle::pager_vertical_orientation);
}

bool RocketConfigManager::getUsingSystemWallpaper()
{
    return getStyleValue(Background::group,Background::usesystemwallpaper,RocketStyle::use_system_wallpaper);
}

int RocketConfigManager::getWallpaperScreen()
{
    return getStyleValue(Background::group,Background::wallpaperofscreen,RocketStyle::use_system_wallpaper_screen);
}

int RocketConfigManager::getInvertedScrollFactorXfromSettings()
{
    bool val = getStyleValue(Settings::group,Settings::invertedscrollingx,RocketStyle::inverted_scrolling_x);
    return (val ? -1 : 1);
}

int RocketConfigManager::getInvertedScrollFactorYfromSettings()
{
    bool inverted = getStyleValue(Settings::group,Settings::invertedscrollingy,RocketStyle::inverted_scrolling_y);
    return (inverted ? -1 : 1);
}

int RocketConfigManager::getNumberOfElementsOfAGroup(KConfigGroup group)
{
    int i=0;
    if (group.groupList().size()>1)
    {
        for (int j=0; j<group.groupList().size();j++)
            i+=getNumberOfElementsOfAGroup(group.group(group.groupList()[j]));
    }
    else {
        i=1;
    }
    return i;
}


map<int,KDEApplication> RocketConfigManager::getApplicationsFromAGroup(KConfigGroup group, int depth)
{
    map<int,KDEApplication> apps;
    int i=0;
    if (group.groupList().size()>1)
    {
        for (int j=0; j<group.groupList().size();j++)
        {
            map<int,KDEApplication> apps2 = getApplicationsFromAGroup(group.group(group.groupList()[j]),depth+1);
            apps.insert(apps2.begin(),apps2.end());
        }
    }
    else {
        if (depth==0) //App
        {
            QString name = group.readEntry("name");
            QString iconname = group.readEntry("iconname");
            QString exec = group.readEntry("exec");
            QString comment = group.readEntry("comment");
            QIcon icon = QIcon::fromTheme(iconname);
            QStringList keywords = group.readEntry("keywords").split(",");
            QString genericname = group.readEntry("genericname");
            QString untranslatedGenericName = group.readEntry("untranslatedgenericname");
            QStringList categories = group.readEntry("categories").split(",");
            bool terminal = (group.readEntry("terminal") == "false" ? false : true);
            QString entrypath = group.readEntry("entrypath");
            int pos = group.readEntry("pos").toInt();
            apps[pos] = KDEApplication(name,iconname,icon,exec,comment,terminal,keywords,genericname,untranslatedGenericName,categories,entrypath);
        }
        else //Folder
        {
            qDebug() << "folder found!";
        }
    }
    return apps;
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
        int N=0; //Number of applications stored in the config file
        for (QString s : config->groupList())
        {
            N+=getNumberOfElementsOfAGroup(config->group(s));
        }
        KDEApplication apps[N];
        for (QString item : config->groupList())
        {
            map<int,KDEApplication> apps_found = getApplicationsFromAGroup(config->group(item));
            for (auto const& i : apps_found)
                apps[i.first] = i.second;
        }
        std::vector<KDEApplication> appvector(apps,apps+N);
        m_apps = appvector;
    }
}

void RocketConfigManager::generateAppGridConfigFile(KConfig * config,KMenuItems menuitems)
{
    int i = 0;
    for (KDEApplication app : menuitems.getApplications())
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

    bool changes = false;
    if (menuitems.getApplications().size()!=m_apps.size()) changes = true;
    else
    {
        for (int i=0;i<m_apps.size(); i++)
        {
            for (int j=0;j<m_apps.size();j++)
            {
                if (menuitems.getApplications()[i]==m_apps[j])
                {
                    break;
                }
                if (j+1==m_apps.size())
                {
                    qDebug() << m_apps[i].name();
                    changes = true;
                }
            }
            if (changes)
                break;
        }
    }
    if (changes)
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
