#pragma once

#include <QAbstractListModel>
#include <QUrl>
#include <memory>

class QString;
class QuickModRef;
typedef std::shared_ptr<class QuickModMetadata> QuickModMetadataPtr;

class QuickModIndexModel : public QAbstractListModel
{
	Q_OBJECT
public:
	explicit QuickModIndexModel();

	int rowCount(const QModelIndex &parent) const override;
	int columnCount(const QModelIndex &parent) const override;

	QVariant data(const QModelIndex &index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;

private slots:
	void reload();

private:
	struct Repo
	{
		explicit Repo(const QString &name, const QUrl &url = QString())
			: name(name), url(url)
		{
		}
		explicit Repo() {}

		QString name;
		QUrl url;
	};
	QList<Repo> m_repos;
};
