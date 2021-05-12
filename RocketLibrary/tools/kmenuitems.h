#ifndef KMENUITEMS_H
#define KMENUITEMS_H


#include <QString>
#include <QIcon>
#include "kdeapplication.h"

class KMenuItems {

private:
    std::vector<KDEApplication> applications;

public:
    void scanElements(QString path=QString("/"),int n=0);
    void sortElementsAlphabetically();
    std::vector<KDEApplication> getApplications() {return applications;}
};
#endif // KMENUITEMS_H
