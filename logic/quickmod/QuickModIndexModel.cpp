#include "QuickModIndexModel.h"

#include <QString>
#include <QMap>
#include <QUrl>
#include <QVariant>
#include <QDebug>

#include "QuickModRef.h"
#include "QuickModMetadata.h"
#include "QuickModDatabase.h"
#include "MultiMC.h"

QuickModIndexModel::QuickModIndexModel() : QAbstractListModel()
{
	connect(MMC->qmdb().get(), &QuickModDatabase::reset, this, &QuickModIndexModel::reload);
	reload();
}

int QuickModIndexModel::rowCount(const QModelIndex &parent) const
{
	return m_repos.size();
}
int QuickModIndexModel::columnCount(const QModelIndex &parent) const
{
	return 2;
}

QVariant QuickModIndexModel::data(const QModelIndex &index, int role) const
{
	const Repo repo = m_repos.at(index.row());
	if (role == Qt::DisplayRole)
	{
		switch (index.column())
		{
		case 0:
			return repo.name;
		case 1:
			return repo.url;
		}
	}
	else if (role == Qt::UserRole)
	{
		return repo.name;
	}
	return QVariant();
}
QVariant QuickModIndexModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		switch (section)
		{
		case 0:
			return tr("Name");
		case 1:
			return tr("Index URL");
		}
	}
	return QVariant();
}
Qt::ItemFlags QuickModIndexModel::flags(const QModelIndex &index) const
{
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void QuickModIndexModel::reload()
{
	QMap<QString, Repo> repos;
	const auto indices = MMC->qmdb()->indices();
	for (auto it = indices.constBegin(); it != indices.constEnd(); ++it)
	{
		repos.insert(it.key(), Repo(it.key(), it.value()));
	}
	for (const auto uidRef : MMC->qmdb()->getPackageUIDs())
	{
		for (const auto mod : MMC->qmdb()->allModMetadata(uidRef))
		{
			if (!repos.contains(mod->repo()))
			{
				repos.insert(mod->repo(), Repo(mod->repo()));
			}
		}
	}
	beginResetModel();
	m_repos = repos.values();
	endResetModel();
}
