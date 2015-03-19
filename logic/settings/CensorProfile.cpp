#include "CensorProfile.h"

#include <QDebug>

CensorProfile::CensorProfile(AuthSessionPtr session)
{
    m_session = session;
}

CensorProfile::~CensorProfile()
{

}

QString CensorProfile::censorString(QString in)
{
    for(QPair<QString, QString> &p: m_replaceStrings)
    {
        in.replace(p.first, p.second);
    }
    return in;
}

void CensorProfile::addString(QString search, QString replace)
{
    m_replaceStrings.append(QPair<QString, QString>(search, replace));
}

void CensorProfile::addSystem(QString id, QString replace)
{
    m_replaceStrings.append(QPair<QString, QString>(resolveSystemSearch(id), replace));
}

void CensorProfile::addProperties()
{
    auto i = m_session->u.properties.begin();
    while (i != m_session->u.properties.end())
    {
        m_replaceStrings.append(QPair<QString, QString>(i.value(), "<" + i.key().toUpper() + ">"));
        ++i;
    }
}

QString CensorProfile::resolveSystemSearch(QString id)
{
    if (id == "profile_id")
        return m_session->uuid;
    if (id == "profile_name")
        return m_session->player_name;
    if (id == "access_token")
        return m_session->access_token;
    if (id == "client_token")
        return m_session->client_token;
    if (id == "session_id")
        return m_session->session;
    if (id == "system_user")
    {
#ifdef Q_OS_WIN32
        return qgetenv("USERNAME");
#else
        return qgetenv("USER");
#endif
    }
    return "";
}

