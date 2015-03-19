#ifndef CENSORPROFILE_H
#define CENSORPROFILE_H

#include <QString>
#include <QList>
#include <QPair>

#include "logic/auth/AuthSession.h"

class CensorProfile
{
public:

    CensorProfile(AuthSessionPtr session);
    ~CensorProfile();

    QString censorString(QString in);

    void addString(QString search, QString replace);
    void addSystem(QString id, QString replace);

    void addProperties();

private:

    QString resolveSystemSearch(QString id);

    AuthSessionPtr m_session;
    QList<QPair<QString, QString>> m_replaceStrings;
};

#endif // CENSORPROFILE_H
