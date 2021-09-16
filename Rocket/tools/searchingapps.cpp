#include "../RocketLibrary/tools/kdeapplication.h"

std::vector<KDEApplication> searchApplication(std::vector<KDEApplication> list, QString query)
{
    if (query=="") return list;
    std::vector<KDEApplication> res;
    for (KDEApplication i : list)
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

std::vector<int> searchApplicationTree(std::vector<KDEApplication> apptree, KDEApplication item)
{
    int index1 = 0;
    int index2 = -1;
    bool found = false;
    for (int i=0;i<(int)apptree.size();i++)
    {
        if (apptree[i].isFolder())
            if (!(item==apptree[i]))
                for (int j=0;j<(int)apptree[i].getChildren().size();j++)
                {
                    if (item==apptree[i].getChildren()[j])
                    {
                        index1 = i;
                        index2 = j;
                        found = true;
                        break;
                    }
                }
            else {
                index1 = i;
                found = true;
            }
        else {
            if (item==apptree[i])
            {
                index1 = i;
                found = true;
            }
        }
        if (found) break;
    }
    return std::vector<int>({index1,index2});
}
