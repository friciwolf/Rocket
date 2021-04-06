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
        if (p->isType(KST_KService)) {
            const KService::Ptr service(static_cast<KService *>(p.data()));
            if (service->noDisplay()) continue;
            name = service->name();
            iconname = service->icon();
            icon = QIcon::fromTheme(service->icon());
            exec = service->exec();
            comment = service->comment();
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
            applications.push_back(KApplication(name,iconname,icon,exec,comment));
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
            // ??
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
            if (applications[i].name().toStdString()>applications[i+1].name().toStdString())
            {
                KApplication temp = applications[i];
                applications[i] = applications[i+1];
                applications[i+1] = temp;

                swap = true;
            }
        }
    }
}
