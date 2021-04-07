#include "kapplication.h"

KApplication::KApplication(QString name, QString iconname, QIcon icon, QString exec, QString comment, bool terminal, QStringList keywords, QString genericname, QString untranslatedGenericName, QStringList categories)
{
    m_name = name;
    m_iconname = iconname;
    m_icon = icon;
    m_exec = exec;
    m_comment = comment;
    m_terminal = terminal;
    m_keywords = keywords;
    m_genericname = genericname;
    m_untranslatedGenericName = untranslatedGenericName;
    m_categories = categories;
}

KApplication::KApplication()
{

}

bool operator==(const KApplication& a, const KApplication& b)
{
    if (a.m_name!=b.m_name) return false;
    if (a.m_iconname!=b.m_iconname) return false;
    if (a.m_comment!=b.m_comment) return false;
    if (a.m_terminal!=b.m_terminal) return false;
    if (a.m_keywords!=b.m_keywords) return false;
    if (a.m_genericname!=b.m_genericname) return false;
    if (a.m_untranslatedGenericName!=b.m_untranslatedGenericName) return false;
    if (a.m_categories!=b.m_categories) return false;
    return true;
}
