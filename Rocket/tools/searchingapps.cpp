#include "icongrid/kapplication.h"
#include <QString>

std::vector<KApplication> searchApplication(std::vector<KApplication> list, QString query)
{
    if (query=="") return list;
    std::vector<KApplication> res;
    for (KApplication i : list)
    {
        if (i.name().contains(query,Qt::CaseSensitivity::CaseInsensitive))
        {
            res.push_back(i);
            continue;
        }
        if  (i.genericname().contains(query,Qt::CaseSensitivity::CaseInsensitive))
        {
            res.push_back(i);
            continue;
        }
        if (i.untranslatedGenericName().contains(query,Qt::CaseSensitivity::CaseInsensitive))
        {
            res.push_back(i);
            continue;
        }
        if (i.exec().contains(query,Qt::CaseSensitivity::CaseInsensitive))
        {
            res.push_back(i);
            continue;
        }
        if (i.comment().contains(query,Qt::CaseSensitivity::CaseInsensitive))
        {
            res.push_back(i);
            continue;
        }
        bool found = false;
        for (QString i2 : i.keywords())
        {
            if (i2.contains(query,Qt::CaseSensitivity::CaseInsensitive))
            {
                res.push_back(i);
                found = true;
                break;
            }
        }
        if (found) continue;
        found = false;
        for (QString i2 : i.categories())
        {
            if(i2.contains(query,Qt::CaseSensitivity::CaseInsensitive))
            {
                res.push_back(i);
                found = true;
                break;
            }
        }
        if (found) continue;
    }
    return res;
}
