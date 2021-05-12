#ifndef SEARCHINGAPPS_H
#define SEARCHINGAPPS_H

#include <QString>
#include "../RocketLibrary/tools/kdeapplication.h"

std::vector<KDEApplication> searchApplication(std::vector<KDEApplication> list, QString query);


#endif // SEARCHINGAPPS_H
