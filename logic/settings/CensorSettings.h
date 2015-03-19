#ifndef CENSORSETTINGS_H
#define CENSORSETTINGS_H

#include <QString>
#include <QJsonObject>
#include <QList>

#include "CensorProfile.h"
#include "logic/auth/AuthSession.h"

class CensorEntry;

class CensorSettings
{
public:
    CensorSettings(QString file);
    ~CensorSettings();

    void load();
    void save();
    void reset();

    // TODO: Change so it expects a map of strings. Then change AuthSession to give such a map
    CensorProfile* makeProfile(AuthSessionPtr session);

private:

    QList<CensorEntry> m_censorEntries;
    QString m_file;

};

class CensorEntry
{
public:
    CensorEntry(QString key, QString replace, bool enabled, bool system);
    ~CensorEntry();

    QString displayName();

    bool isSystem()
    {
        return m_system;
    }

    bool isEnabled()
    {
        return m_enabled;
    }

    QString key()
    {
        return m_key;
    }

    QString replace()
    {
        return m_replace;
    }

private:


    bool m_system;
    bool m_enabled;

    QString m_key;
    QString m_replace;
};

#endif // CENSORSETTINGS_H
