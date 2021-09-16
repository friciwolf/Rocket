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

map<int,KDEApplication> RocketConfigManager::getApplicationTree(KConfigGroup group, KDEApplication * folder)
{
    map<int,KDEApplication> apptree;
    if (group.groupList().size()>1) //Folder
    {
        QString name = group.readEntry("name");
        QString iconname = group.readEntry("iconname");
        QIcon icon = QIcon::fromTheme(iconname);
        QString comment = group.readEntry("comment");
        int pos = group.readEntry("pos").toInt();
        KDEApplication fol = KDEApplication(name,iconname,icon,comment);
        std::vector<KDEApplication> children(group.groupList().size());
        fol.setChildren(children);

        for (int j=0;j<group.groupList().size();j++)
        {
            map<int,KDEApplication> foldercontent = getApplicationTree(group.group(group.groupList()[j]),&fol);
        }
        apptree[pos] = fol;
    }
    else //App
    {
        QString name = group.readEntry("name");
        QString iconname = group.readEntry("iconname");
        QIcon icon = QIcon::fromTheme(iconname);
        QString exec = group.readEntry("exec");
        QString comment = group.readEntry("comment");
        QStringList keywords = group.readEntry("keywords").split(",");
        QString genericname = group.readEntry("genericname");
        QString untranslatedGenericName = group.readEntry("untranslatedgenericname");
        QStringList categories = group.readEntry("categories").split(",");
        bool terminal = (group.readEntry("terminal") == "false" ? false : true);
        QString entrypath = group.readEntry("entrypath");
        int pos = group.readEntry("pos").toInt();
        if (folder==nullptr)
            apptree[pos] = KDEApplication(name,iconname,icon,exec,comment,terminal,keywords,genericname,untranslatedGenericName,categories,entrypath);
        else
            folder->setChild(pos,KDEApplication(name,iconname,icon,exec,comment,terminal,keywords,genericname,untranslatedGenericName,categories,entrypath));
    }
    return apptree;
}

void RocketConfigManager::checkAppGridConfigFile()
{
    KConfig * config = m_appgridconfig;
    if (config->groupList().size()==0)
    {
        qDebug() << "Appgrid configuration not found. Generating one...";
        KMenuItems menuitems;
        menuitems.scanElements();
        generateAppGridConfigFile(config,menuitems.getApplications());
        m_apps = m_app_tree;
    }
    else
    {
        std::vector<KDEApplication> appTree(config->groupList().size());

        std::vector<KDEApplication> appvector;
        for (QString item : config->groupList())
        {
            map<int,KDEApplication> app_tree = getApplicationTree(config->group(item));
            for (auto const& i: app_tree)
            {
                appTree[i.first] = i.second;
                if (!(((KDEApplication)i.second).isFolder()))
                    appvector.push_back(i.second);
                else {
                    std::vector<KDEApplication> c = ((KDEApplication)i.second).getChildren();
                    appvector.insert(appvector.end(),c.begin(),c.end());
                }
            }
        }
        m_apps = appvector;
        m_app_tree = appTree;
    }
}

void RocketConfigManager::generateAppGridConfigFile(KConfig * config, std::vector<KDEApplication> app_tree)
{
    for (QString group : config->groupList())
    {
        for (QString subgroup : config->group(group).groupList())
                config->group(group).deleteGroup(subgroup);
        config->deleteGroup(group);
    }

    int i = 0;
    for (KDEApplication app : app_tree)
    {
        if (app.isFolder())
        {
            config->group("Entry"+QString::number(i)).writeEntry("name",app.name());
            config->group("Entry"+QString::number(i)).writeEntry("iconname","folder");
            config->group("Entry"+QString::number(i)).writeEntry("comment",app.comment());
            config->group("Entry"+QString::number(i)).writeEntry("pos",QString::number(i));
            int j=0;
            for (KDEApplication a : app.getChildren())
            {
                config->group("Entry"+QString::number(i)).group("Entry"+QString::number(j)).writeEntry("name",a.name());
                config->group("Entry"+QString::number(i)).group("Entry"+QString::number(j)).writeEntry("iconname",a.iconname());
                config->group("Entry"+QString::number(i)).group("Entry"+QString::number(j)).writeEntry("exec",a.exec());
                config->group("Entry"+QString::number(i)).group("Entry"+QString::number(j)).writeEntry("comment",a.comment());
                config->group("Entry"+QString::number(i)).group("Entry"+QString::number(j)).writeEntry("terminal",a.terminal());
                if (a.keywords()==QStringList(""))
                    config->group("Entry"+QString::number(i)).group("Entry"+QString::number(j)).writeEntry("keywords",QStringList());
                else
                    config->group("Entry"+QString::number(i)).group("Entry"+QString::number(j)).writeEntry("keywords",a.keywords());
                config->group("Entry"+QString::number(i)).group("Entry"+QString::number(j)).writeEntry("genericname",a.genericname());
                config->group("Entry"+QString::number(i)).group("Entry"+QString::number(j)).writeEntry("untranslatedgenericname",a.untranslatedGenericName());
                if (a.categories()==QStringList(""))
                    config->group("Entry"+QString::number(i)).group("Entry"+QString::number(j)).writeEntry("categories",QStringList());
                else
                    config->group("Entry"+QString::number(i)).group("Entry"+QString::number(j)).writeEntry("categories",a.categories());
                config->group("Entry"+QString::number(i)).group("Entry"+QString::number(j)).writeEntry("entrypath",a.entrypath());
                config->group("Entry"+QString::number(i)).group("Entry"+QString::number(j)).writeEntry("pos",QString::number(j));
                j++;
            }
        }
        else
        {
            config->group("Entry"+QString::number(i)).writeEntry("name",app.name());
            config->group("Entry"+QString::number(i)).writeEntry("iconname",app.iconname());
            config->group("Entry"+QString::number(i)).writeEntry("exec",app.exec());
            config->group("Entry"+QString::number(i)).writeEntry("comment",app.comment());
            config->group("Entry"+QString::number(i)).writeEntry("terminal",app.terminal());
            if (app.keywords()==QStringList(""))
                config->group("Entry"+QString::number(i)).writeEntry("keywords",QStringList());
            else
                config->group("Entry"+QString::number(i)).writeEntry("keywords",app.keywords());
            config->group("Entry"+QString::number(i)).writeEntry("genericname",app.genericname());
            config->group("Entry"+QString::number(i)).writeEntry("untranslatedgenericname",app.untranslatedGenericName());
            if (app.categories()==QStringList(""))
                config->group("Entry"+QString::number(i)).writeEntry("categories",QStringList());
            else
                config->group("Entry"+QString::number(i)).writeEntry("categories",app.categories());
            config->group("Entry"+QString::number(i)).writeEntry("entrypath",app.entrypath());
            config->group("Entry"+QString::number(i)).writeEntry("pos",QString::number(i));
        }
        i++;
    }
    m_app_tree = app_tree;
}

bool RocketConfigManager::updateApplicationList()
{
    KMenuItems menuitems;
    menuitems.scanElements();

    bool changes = false;
    for (int i=0;i<(int)menuitems.getApplications().size(); i++)
    {
        for (int j=0;j<(int)m_apps.size();j++)
        {
            if (menuitems.getApplications()[i]==m_apps[j])
            {
                break;
            }
            if (j+1==(int)m_apps.size())
            {
                m_apps.push_back(menuitems.getApplications()[i]);
                m_app_tree.push_back(menuitems.getApplications()[i]);
                changes = true;
            }
        }
    }

    for (int i=0;i<(int)m_apps.size(); i++)
    {
        for (int j=0;j<(int)menuitems.getApplications().size();j++)
        {
            if (menuitems.getApplications()[j]==m_apps[i])
            {
                break;
            }
            if (j+1==(int)menuitems.getApplications().size())
            {
                bool done=false;
                for (int k=0;k<(int)m_app_tree.size();k++)
                {
                    if (m_app_tree[k].isFolder())
                    {
                        for (int k2=0;k2<(int)m_app_tree[k].getChildren().size();k2++)
                        {
                            if (m_app_tree[k].getChildren()[k2]==m_apps[i])
                            {
                                m_app_tree[k].getChildren().erase(m_app_tree[k].getChildren().begin()+k2);
                                done = true;
                                break;
                            }
                        }
                        if (m_app_tree[k].getChildren().size()==1)
                        {
                            m_app_tree[k] = m_app_tree[k].getChildren()[0];
                            m_app_tree[k].setToFolder(false);
                        }
                    }
                    else
                    {
                        if (m_app_tree[k]==m_apps[i])
                        {
                            m_app_tree.erase(m_app_tree.begin()+k);
                            done = true;
                            break;
                        }
                    }
                    if (done) break;
                }
                m_apps.erase(m_apps.begin()+i);
                changes = true;
            }
        }
    }

    if (changes)
    {
        qDebug() << "Appgrid configuration changed. Generating new configuration file...";
        generateAppGridConfigFile(m_appgridconfig,m_app_tree);
        m_appgridconfig->reparseConfiguration();
        return true;
    }
    return false;
}
