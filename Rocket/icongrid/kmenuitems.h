#ifndef KMENUITEMS_H
#define KMENUITEMS_H


#include <QString>
#include <QIcon>
#include "kapplication.h"

class KMenuItems {

private:
    std::vector<KApplication> applications;

public:
    void scanElements(QString path=QString("/"),int n=0);
    void sortElementsAlphabetically();
    std::vector<KApplication> getApplications() {return applications;}

};
#endif // KMENUITEMS_H
