#ifndef KDEAPPLICATION_H
#define KDEAPPLICATION_H

#include <QString>
#include <QIcon>

class KDEApplication
{
public:
    ~KDEApplication() {}
    KDEApplication(QString name, QString iconname, QIcon icon, QString exec, QString comment, bool terminal, QStringList keywords, QString genericname, QString untranslatedGenericName, QStringList categories, QString entrypath);
    KDEApplication(QString name, QString iconname, QIcon icon, QString comment);
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
    QString entrypath() {return m_entrypath;}
    bool isFolder() {return m_isfolder;}
    friend bool operator==(const KDEApplication& a, const KDEApplication& b);

    void setChild(int index,KDEApplication child) {m_children[index] = child;}
    std::vector<KDEApplication> getChildren() {return m_children;}
    void setChildren(std::vector<KDEApplication> children){m_children=children;}
    void setToFolder(bool isFolder){m_isfolder = isFolder; m_iconname="folder"; m_icon=QIcon::fromTheme(m_iconname);}
    void setName(QString newname){m_name=newname;}

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
    bool m_isfolder;
    std::vector<KDEApplication> m_children;
};

#endif // KDEAPPLICATION_H
