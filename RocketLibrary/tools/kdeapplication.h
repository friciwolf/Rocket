#ifndef KDEAPPLICATION_H
#define KDEAPPLICATION_H

#include <QString>
#include <QIcon>

class KDEApplication
{
public:
    KDEApplication(QString name, QString iconname, QIcon icon, QString exec, QString comment, bool terminal, QStringList keywords, QString genericname, QString untranslatedGenericName, QStringList categories, QString entrypath);
    KDEApplication();
    QString name(){return m_name;}
    QString iconname(){return m_iconname;}
    QString exec(){return m_exec;}
    QIcon icon(){return m_icon;}
    QString comment() {return m_comment;}
    bool terminal() {return m_terminal;}
    QStringList keywords() {return m_keywords;}
    QString genericname() {return m_genericname;}
    QString untranslatedGenericName() {return m_untranslatedGenericName;}
    QStringList categories() {return m_categories;}
    QString entrypath(){return m_entrypath;}
    friend bool operator==(const KDEApplication& a, const KDEApplication& b);

private:
    QString m_name;
    QString m_iconname;
    QString m_exec;
    QIcon m_icon;
    QString m_comment;
    bool m_terminal;
    QStringList m_keywords;
    QString m_genericname;
    QString m_untranslatedGenericName;
    QStringList m_categories;
    QString m_entrypath;
};

#endif // KDEAPPLICATION_H