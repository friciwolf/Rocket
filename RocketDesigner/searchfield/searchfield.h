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
    explicit SearchField(QWidget *parent);
    void positioning();
    void keyPressEvent(QKeyEvent *event);
    void resizeEvent(QResizeEvent *event);

signals:
    void navigate(int key);
};

#endif // SEARCHFIELD_H
