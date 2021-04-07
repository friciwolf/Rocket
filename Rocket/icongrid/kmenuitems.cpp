#include "kmenuitems.h"
#include "kapplication.h"
#include <KServiceGroup>
#include <QDebug>

/*
 * This algorithm parses every element in the start menu by category and outputs the menu as such.
 * Duplicate programs, which belong to two categories thus are present.
 *
*/
void KMenuItems::scanElements(QString path,int n)
{
    KServiceGroup::Ptr root = KServiceGroup::group(path);
    const KServiceGroup::List list = root->entries();

    for (const KSycocaEntry::Ptr &p : list) {
        QString name;
        QString iconname;
        QString exec;
        QString comment;
        QIcon icon;
        QStringList keywords;
        QString genericname;
        QString untranslatedGenericName;
        QStringList categories;
        bool terminal;
        if (p->isType(KST_KService)) {
            const KService::Ptr service(static_cast<KService *>(p.data()));
            if (service->noDisplay()) continue;
            name = service->name();
            iconname = service->icon();
            icon = QIcon::fromTheme(service->icon());
            exec = service->exec();
            comment = service->comment();
            terminal = service->terminal();
            keywords = service->keywords();
            genericname = service->genericName();
            untranslatedGenericName = service->untranslatedGenericName();
            categories = service->categories();
            bool doubled = false;
            for (KApplication a : applications)
            {
                if (a.name()==name)
                {
                    doubled=true;
                    break;
                }
            }
            if (doubled) continue;
            applications.push_back(KApplication(name,iconname,icon,exec,comment,terminal,keywords,genericname,untranslatedGenericName,categories));
        }
        else if (p->isType(KST_KServiceGroup)) {
            const KServiceGroup::Ptr serviceGroup(static_cast<KServiceGroup *>(p.data()));
            if (serviceGroup->noDisplay() || serviceGroup->childCount() == 0)  continue;
            //name = serviceGroup->caption();
            //w->itemnames.push_back(text);
            //w->icons.push_back(QIcon::fromTheme(serviceGroup->icon()));
            scanElements(serviceGroup->entryPath(),n+1);
        }
        else {
            // ?? (Even KDE devs don't know what this case means...)
            continue;
        }
    }

    // Sorting alphabetically...
    sortElementsAlphabetically();
}

void KMenuItems::sortElementsAlphabetically()
{
    bool swap = true;
    int j=0;
    while (swap)
    {
        swap = false;
        j++;
        for (int i=0;i<applications.size()-j;i++)
        {
            if (applications[i].name().toLower().toStdString()>applications[i+1].name().toLower().toStdString())
            {
                KApplication temp = applications[i];
                applications[i] = applications[i+1];
                applications[i+1] = temp;

                swap = true;
            }
        }
    }
}

KApplication KMenuItems::search(const QString &query)
{
    return applications[0];
}
