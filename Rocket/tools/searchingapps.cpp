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
        if (i.keywords().contains(query,Qt::CaseSensitivity::CaseInsensitive))
        {
            res.push_back(i);
            continue;
        }
        if (i.categories().contains(query,Qt::CaseSensitivity::CaseInsensitive))
        {
            res.push_back(i);
            continue;
        }
    }
    return res;
}
