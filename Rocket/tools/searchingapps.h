#ifndef SEARCHINGAPPS_H
#define SEARCHINGAPPS_H

#include <QString>
#include "../RocketLibrary/tools/kdeapplication.h"

std::vector<KDEApplication> searchApplication(std::vector<KDEApplication> list, QString query);
std::vector<int> searchApplicationTree(std::vector<KDEApplication> apptree, KDEApplication item);

#endif // SEARCHINGAPPS_H
