#include "kapplication.h"

KApplication::KApplication(QString name, QString iconname, QIcon icon, QString exec, QString comment)
{
    m_name = name;
    m_iconname = iconname;
    m_icon = icon;
    m_exec = exec;
    m_comment = comment;
}
