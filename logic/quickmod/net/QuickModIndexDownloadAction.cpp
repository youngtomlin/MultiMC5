#include "QuickModIndexDownloadAction.h"

#include "logic/quickmod/QuickModMetadata.h"
#include "logic/quickmod/QuickModDatabase.h"
#include "logic/quickmod/QuickModIndexModel.h"

#include "MultiMC.h"
#include "logger/QsLog.h"
#include "logic/MMCJson.h"
#include "logic/net/NetJob.h"

QuickModIndexDownloadAction::QuickModIndexDownloadAction(const QUrl &url, NetJob *netjob)
	: QuickModBaseDownloadAction(url), m_job(netjob)
{
}

bool QuickModIndexDownloadAction::handle(const QByteArray &data)
{
	Q_ASSERT(m_job);
	try
	{
		const QJsonObject root = MMCJson::ensureObject(MMCJson::parseDocument(data, "QuickMod Index"));
		const QString baseUrlString = MMCJson::ensureString(root.value("baseUrl"));
		m_repo = MMCJson::ensureString(root.value("repo"));

		MMC->qmdb()->addRepo(m_repo, m_originalUrl);

		const QJsonArray array = MMCJson::ensureArray(root.value("index"));
		for (const QJsonValue itemVal : array)
		{
			const QJsonObject itemObj = MMCJson::ensureObject(itemVal);
			const QString uid = MMCJson::ensureString(itemObj.value("uid"));
			if (!MMC->qmdb()->haveUid(QuickModRef(uid), m_repo))
			{
				const QString urlString = MMCJson::ensureString(itemObj.value("url"));
				QUrl url;
				if (baseUrlString.contains("{}"))
				{
					url = QUrl(QString(baseUrlString).replace("{}", urlString));
				}
				else
				{
					url = QUrl(baseUrlString).resolved(QUrl(urlString));
				}
				m_job->addNetAction(QuickModBaseDownloadAction::make(m_job, url, m_repo, uid));
			}
		}
	}
	catch (MMCError &e)
	{
		m_errorString = e.cause();
		return false;
	}
	return true;
}

QString QuickModIndexDownloadAction::cacheIdentifier() const
{
	return m_repo.isEmpty() ? m_originalUrl.toString(QUrl::RemovePassword | QUrl::NormalizePathSegments).replace("/", "\\/") : m_repo;
}
