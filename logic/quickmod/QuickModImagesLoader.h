#pragma once

#include <QObject>

class QuickModMetadata;
class QuickModRef;

class QuickModImagesLoader : public QObject
{
	Q_OBJECT
public:
	explicit QuickModImagesLoader(QuickModMetadata *metadata);

signals:
	void iconUpdated(QuickModRef uid);
	void logoUpdated(QuickModRef uid);

private
slots:
	void iconDownloadFinished(int index);
	void logoDownloadFinished(int index);

private:
	friend class QuickModMetadata;
	bool m_imagesLoaded;
	QuickModMetadata *m_metadata;
	void fetchImages();
	QString fileName(const QUrl &url) const;
};
