#ifndef KAPPLICATION_H
#define KAPPLICATION_H

#include <QString>
#include <QIcon>

class KApplication
{
public:
    KApplication(QString name, QString iconname, QIcon icon, QString exec, QString comment);
    QString name(){return m_name;}
    QString iconname(){return m_iconname;}
    QString exec(){return m_exec;}
    QIcon icon(){return m_icon;}
    QString comment() {return m_comment;}

private:
    QString m_name;
    QString m_iconname;
    QString m_exec;
    QIcon m_icon;
    QString m_comment;
};

#endif // KAPPLICATION_H
