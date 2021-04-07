#include "icongrid/kapplication.h"
#include <QString>

std::vector<KApplication> searchApplication(std::vector<KApplication> list, QString query)
{
    if (query=="") return list;
    std::vector<KApplication> res;
    for (KApplication i : list)
    {
        if (i.name().contains(query,Qt::CaseSensitivity::CaseInsensitive)) res.push_back(i);
    }
    return res;
}
