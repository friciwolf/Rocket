#ifndef SEARCHINGAPPS_H
#define SEARCHINGAPPS_H

#include <QString>
#include "icongrid/kapplication.h"

std::vector<KApplication> searchApplication(std::vector<KApplication> list, QString query);


#endif // SEARCHINGAPPS_H
