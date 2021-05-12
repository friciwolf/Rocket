#include "kdeapplication.h"

KDEApplication::KDEApplication(QString name, QString iconname, QIcon icon, QString exec, QString comment, bool terminal, QStringList keywords, QString genericname, QString untranslatedGenericName, QStringList categories, QString entrypath)
{
    m_name = name;
    m_iconname = iconname;
    m_icon = icon;
    m_exec = exec;
    m_comment = comment;
    m_terminal = terminal;
    m_keywords = (keywords.size()==1 ? (keywords[0].contains(",") ? keywords[0].split(",") : keywords) : keywords);
    m_genericname = genericname;
    m_untranslatedGenericName = untranslatedGenericName;
    m_categories = (categories.size()==1 ? (categories[0].contains(",") ? categories[0].split(",") : categories) : categories);
    m_entrypath = entrypath;
}

KDEApplication::KDEApplication()
{

}

bool operator==(const KDEApplication& a, const KDEApplication& b)
{
    if (a.m_name!=b.m_name) return false;
    if (a.m_iconname!=b.m_iconname) return false;
    if (a.m_exec!=b.m_exec) return false;
    if (a.m_comment!=b.m_comment) return false;
    if (a.m_terminal!=b.m_terminal) return false;
    bool studyKeywords = true;
    if (a.m_keywords.size()==1)
        if (a.m_keywords[0]=="") studyKeywords = false;
    if (b.m_keywords.size()==1)
        if (b.m_keywords[0]=="") studyKeywords = false;
    if (a.m_keywords.size()!=b.m_keywords.size() && studyKeywords) return false;
    else {
        for (int i=0;i<a.m_keywords.size();i++){
            for (int j=0;j<a.m_keywords.size();j++)
                if (a.m_keywords[i]==b.m_keywords[j]) break;
                else if (j==a.m_keywords.size()-1) return false;
        }
    }
    if (a.m_genericname!=b.m_genericname) return false;
    if (a.m_untranslatedGenericName!=b.m_untranslatedGenericName) return false;
    bool studyCategories = true;
    if (a.m_categories.size()==1)
        if (a.m_categories[0]=="") studyCategories = false;
    if (b.m_categories.size()==1)
        if (b.m_categories[0]=="") studyCategories = false;
    if (a.m_categories.size()!=b.m_categories.size() && studyCategories) return false;
    else {
        for (int i=0;i<a.m_categories.size();i++){
            for (int j=0;j<a.m_categories.size();j++)
                if (a.m_categories[i]==b.m_categories[j]) break;
                else if (j==a.m_categories.size()-1) return false;
        }
    }
    if (a.m_entrypath!=b.m_entrypath) return false;
    return true;
}
