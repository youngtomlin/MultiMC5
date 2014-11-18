/* Copyright 2013 MultiMC Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <QString>
#include <QMap>
#include <QCryptographicHash>
#include <QUrl>
#include <QStringList>

#include "logic/BaseVersion.h"
#include "logic/net/NetJob.h"
#include "logic/net/CacheDownload.h"
#include "logic/tasks/Task.h"
#include "logic/quickmod/QuickModMetadata.h"
#include "QuickModDownload.h"
#include "QuickModVersionRef.h"
#include <logic/settings/SettingsObject.h>
#include "modutils.h"
#include "pathutils.h"
#include "MultiMC.h"

class OneSixInstance;
class BaseInstance;
typedef std::shared_ptr<BaseInstance> InstancePtr;

class BaseQuickModVersion;
typedef std::shared_ptr<BaseQuickModVersion> QuickModVersionPtr;
Q_DECLARE_METATYPE(QuickModVersionPtr)

class BaseQuickModVersion : public BaseVersion
{
public: /* types */
	enum InstallType
	{
		ForgeMod,
		ForgeCoreMod,
		LiteLoaderMod,
		Extract,
		ConfigPack,
		Group
	};

	struct Library
	{
		Library()
		{
		}
		Library(const QString &name, const QUrl &repo) : name(name), repo(repo)
		{
		}
		QJsonObject toJson() const;
		QString name;
		QUrl repo;
	};

public: /* construction */
	BaseQuickModVersion(QuickModMetadataPtr mod, InstallType installType) : mod(mod), installType(installType)
	{
	}

	static QList<QuickModVersionPtr> parse(const QJsonObject &object, QuickModMetadataPtr mod);
	static QuickModVersionPtr parseSingle(const QJsonObject &object, QuickModMetadataPtr mod);
	void parse(const QJsonObject &object);
	QJsonObject toJson() const;

public: /* methods */
	QString descriptor() const override
	{
		return m_version.toString();
	}

	QString name() const override
	{
		return versionName;
	}

	QString typeString() const
	{
		return type;
	}

	QuickModVersionRef version() const
	{
		return QuickModVersionRef(mod->uid(), m_version);
	}

	/// get the file name for the mod file.
	QString fileName() const;
	
	MetaEntryPtr cacheEntry() const;

	/// get the local cache storage path for the mod
	QString storagePath() const;

	// FIXME: make this part of the json.
	/// get the instance deploy path for the mod
	virtual QString instancePath() const = 0;

	/// Get a download to use.
	QuickModDownload highestPriorityDownload(const QuickModDownload::DownloadType type = QuickModDownload::Invalid);

	/// Does this actually need to be deployed into the instance?
	virtual bool needsDeploy() const { return true; }

	/// install this version of package into the specified instance
	void installInto(std::shared_ptr<OneSixInstance> instance);

	/// removes this version of package from the specified instance
	void removeFrom(std::shared_ptr<OneSixInstance> instance);

	bool dependsOnMinecraft() const
	{
		return dependencies.contains(QuickModRef("net.minecraft"));
	}
	QString minecraftVersionInterval() const
	{
		if (!dependsOnMinecraft())
		{
			return QString();
		}
		else
		{
			return dependencies[QuickModRef("net.minecraft")].first.toString();
		}
	}

private: /* methods */
	void installLibrariesInto(std::shared_ptr<OneSixInstance> instance);
	virtual QString fileEnding() const = 0;
	virtual void installIntoImpl(const QString &source, const QString &destination) = 0;

public: /* data */
	/// quickmod this is associated with
	QuickModMetadataPtr mod;

	/// fluff display name for the version ('name' in the spec)
	QString versionName;

	/// actual version id ('version' in the spec)
	Util::Version m_version;

	/// a thing. idk what it is
	QString versionString;

	/// A type of the version, for example Release, Dev, Alpha or Beta.
	QString type;

	QMap<QuickModRef, QPair<QuickModVersionRef, bool>> dependencies;
	QMap<QuickModRef, QuickModVersionRef> recommendations;
	QMap<QuickModRef, QuickModVersionRef> suggestions;
	QMap<QuickModRef, QuickModVersionRef> conflicts;
	QMap<QuickModRef, QuickModVersionRef> provides;

	// FIXME: move these to some 'File' object
	/// SHA1 checksum of the downloaded file.
	QString sha1;

	/// how to install the mod file?
	InstallType installType;

	/// list of libraries to add for this mod
	QList<Library> libraries;

	/// list of download locations for the file.
	QList<QuickModDownload> downloads;
};


class QuickModForgeModVersion : public BaseQuickModVersion
{
public:
	explicit QuickModForgeModVersion(QuickModMetadataPtr mod, InstallType installType = ForgeMod)
		: BaseQuickModVersion(mod, installType) {}

	QString fileEnding() const override { return ".jar"; }
	QString instancePath() const override { return "mods"; }
	void installIntoImpl(const QString &source, const QString &destination) override;
};
class QuickModLiteloaderVersion : public QuickModForgeModVersion
{
public:
	explicit QuickModLiteloaderVersion(QuickModMetadataPtr mod)
		: QuickModForgeModVersion(mod, LiteLoaderMod) {}

	QString fileEnding() const override { return ".litemod"; }
};

class QuickModExtractVersion : public BaseQuickModVersion
{
public:
	explicit QuickModExtractVersion(QuickModMetadataPtr mod, InstallType installType = Extract)
		: BaseQuickModVersion(mod, installType) {}

	QString fileEnding() const override { return ".zip"; }
	QString instancePath() const override { return "."; }
	void installIntoImpl(const QString &source, const QString &destination) override;
};
class QuickModConfigPackVersion : public QuickModExtractVersion
{
public:
	explicit QuickModConfigPackVersion(QuickModMetadataPtr mod)
		: QuickModExtractVersion(mod, ConfigPack) {}

	QString instancePath() const override { return "config"; }
};

class QuickModGroupVersion : public BaseQuickModVersion
{
public:
	explicit QuickModGroupVersion(QuickModMetadataPtr mod)
		: BaseQuickModVersion(mod, Group) {}

	QString fileEnding() const override { return QString(); }
	QString instancePath() const override { return QString(); }
	bool needsDeploy() const override { return false; }
	void installIntoImpl(const QString &source, const QString &destination) override { /* nop */ }
};
