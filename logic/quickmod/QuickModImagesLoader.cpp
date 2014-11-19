#include "QuickModImagesLoader.h"

#include "logic/net/CacheDownload.h"
#include "logic/net/NetJob.h"
#include "logic/net/HttpMetaCache.h"

#include "logic/quickmod/QuickModMetadata.h"

#include "MultiMC.h"

QuickModImagesLoader::QuickModImagesLoader(QuickModMetadata *metadata)
	: QObject(), m_imagesLoaded(false), m_metadata(metadata)
{
}

void QuickModImagesLoader::iconDownloadFinished(int index)
{
	auto download = qobject_cast<CacheDownload *>(sender());
	m_metadata->m_icon = QIcon(download->getTargetFilepath());
	if (!m_metadata->m_icon.isNull())
	{
		emit iconUpdated(m_metadata->uid());
	}
}
void QuickModImagesLoader::logoDownloadFinished(int index)
{
	auto download = qobject_cast<CacheDownload *>(sender());
	m_metadata->m_logo = QPixmap(download->getTargetFilepath());
	if (!m_metadata->m_logo.isNull())
	{
		emit logoUpdated(m_metadata->uid());
	}
}

void QuickModImagesLoader::fetchImages()
{
	if (m_imagesLoaded)
	{
		return;
	}
	auto job = new NetJob("QuickMod image download: " + m_metadata->m_name);
	bool download = false;
	m_imagesLoaded = true;
	const auto iconUrl = m_metadata->iconUrl();
	const auto logoUrl = m_metadata->logoUrl();
	if (iconUrl.isValid() && m_metadata->m_icon.isNull())
	{
		auto icon = CacheDownload::make(
			iconUrl, MMC->metacache()->resolveEntry("quickmods/icons", fileName(iconUrl)));
		connect(icon.get(), &CacheDownload::succeeded, this,
				&QuickModImagesLoader::iconDownloadFinished);
		icon->m_followRedirects = true;
		job->addNetAction(icon);
		download = true;
	}
	if (logoUrl.isValid() && m_metadata->m_logo.isNull())
	{
		auto logo = CacheDownload::make(
			logoUrl, MMC->metacache()->resolveEntry("quickmods/logos", fileName(logoUrl)));
		connect(logo.get(), &CacheDownload::succeeded, this,
				&QuickModImagesLoader::logoDownloadFinished);
		logo->m_followRedirects = true;
		job->addNetAction(logo);
		download = true;
	}
	if (download)
	{
		job->start();
		connect(job, &NetJob::succeeded, job, &NetJob::deleteLater);
		connect(job, &NetJob::failed, job, &NetJob::deleteLater);
	}
	else
	{
		delete job;
	}
}

QString QuickModImagesLoader::fileName(const QUrl &url) const
{
	const QString path = url.path();
	return m_metadata->uid().toString() + path.mid(path.lastIndexOf("."));
}
