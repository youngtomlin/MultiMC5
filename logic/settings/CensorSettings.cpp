#include "CensorSettings.h"

#include <QJsonArray>
#include <QDebug>
#include "logic/Json.h"

CensorSettings::CensorSettings(QString file)
{
    m_file = file;
}

CensorSettings::~CensorSettings()
{

}

void CensorSettings::load()
{

    if (!QFile::exists(m_file))
    {
        reset();
        return;
    }

    QJsonObject data = Json::ensureObject(Json::ensureDocument(m_file));

    int version = data.value("version").toInt();
    if (version != 1)
    {
        qCritical() << QString("Censor Settings are stored in version %1 - Expected 1").arg(version);
        reset();
    }
    else
    {
        m_censorEntries.clear();

        for (auto i: Json::ensureIsArrayOf<QJsonObject>(data.value("replacements")))
        {
            if (i.contains("id"))
            {
                CensorEntry entry(i.value("id").toString(), i.value("replace").toString(), i.value("enabled").toBool(), true);
                m_censorEntries.append(entry);
            }
            else
            {
                CensorEntry entry(i.value("search").toString(), i.value("replace").toString(), i.value("enabled").toBool(), false);
                m_censorEntries.append(entry);
            }
        }
    }
}

void CensorSettings::save()
{
    QJsonObject data;
    data["version"] = 1;

    QJsonArray entryList;
    data["replacements"] = entryList;

    for (CensorEntry entry: m_censorEntries)
    {
        QJsonObject entryData;
        if (entry.isSystem())
            entryData["id"] = entry.key();
        else
            entryData["search"] = entry.key();
        entryData["replace"] = entry.replace();
        entryData["enabled"] = entry.isEnabled();

        entryList.append(entryData);
    }

    Json::write(data, m_file);
}

void CensorSettings::reset()
{
    m_censorEntries.clear();

    // Template: m_censorEntries.append(CensorEntry("key", "replace", enabled, system));
    m_censorEntries.append(CensorEntry("profile_id", "<PROFILE ID>", true, true));
    m_censorEntries.append(CensorEntry("profile_name", "<PROFILE NAME>", true, true));
    m_censorEntries.append(CensorEntry("access_token", "<ACCESS TOKEN>", true, true));
    m_censorEntries.append(CensorEntry("client_token", "<CLIENT TOKEN>", true, true));
    m_censorEntries.append(CensorEntry("session_id", "<SESSION ID>", true, true));
    m_censorEntries.append(CensorEntry("system_user", "<SYSTEM USER>", true, true));

    save();
}

CensorProfile *CensorSettings::makeProfile(AuthSessionPtr session)
{
    CensorProfile* p = new CensorProfile(session);

    // Make sure properties are always masked. Those contain sensitive information, like Twitch stream key
    p->addProperties();

    // Add all the things
    for (CensorEntry e: m_censorEntries)
    {
        if (e.isEnabled())
        {
            if (e.isSystem())
            {
                p->addSystem(e.key(), e.replace());
            }
            else
            {
                p->addString(e.key(), e.replace());
            }
        }
    }
    return p;
}


CensorEntry::CensorEntry(QString key, QString replace, bool enabled, bool system)
{
    m_key = key;
    m_replace = replace;
    m_enabled = enabled;
    m_system = system;
}

CensorEntry::~CensorEntry()
{

}

QString CensorEntry::displayName()
{
    if (m_system)
    {
        // TODO: Check for strings
        return QString("System String: %1").arg(m_key);
    }
    else
    {
        return m_key;
    }
}
