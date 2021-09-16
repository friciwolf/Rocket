#ifndef SEARCHFIELD_H
#define SEARCHFIELD_H

#include <QObject>
#include <QWidget>
#include <QLineEdit>
#include <pager/horizontalpager.h>

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

class keyEnterReceiver : public QObject
{
    Q_OBJECT
protected:
    bool eventFilter(QObject* obj, QEvent* event);
};

#endif // SEARCHFIELD_H
