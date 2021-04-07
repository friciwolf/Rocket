#ifndef SEARCHFIELD_H
#define SEARCHFIELD_H

#include <QObject>
#include <QWidget>
#include <QLineEdit>
#include <pager/pager.h>

class SearchField : public QLineEdit
{
    Q_OBJECT
public:
    explicit SearchField(QWidget *parent, int search_width, int search_height);
};

#endif // SEARCHFIELD_H
