#pragma once
#include "QString"
#include "QDir"
#include "QFile"
#include "QFileInfo"
#include "QSettings"

class NxFileManager;
using NxFileManagerPtr = std::shared_ptr<NxFileManager>;

using NxRegSettings = QSettings;
using NxRegSettingsPtr = std::shared_ptr<QSettings>;

class NxFileManager
{
public:
    enum FileType{
        eApplicationDir,
        eDBRootDir,
        eDBOldRootDir,
        eDBProfiles,
        eDBDesigns,
        eDBModels,
        eDBPlans,
        eDBHardware,
        eDBData,
        eDefaultFilesDir
    };

    enum SettingType
    {
        eSettingData,
        eSettingLicense,
        eSettingTheme
    };

    NxFileManager();
    QString GetPath(FileType fileType) const;
    QStringList GetFilesAndFolders(const QString &path, QDir::Filters filters = QDir::Dirs| QDir::Files) const;
    NxRegSettingsPtr GetSettings(SettingType type) const;
    static NxFileManagerPtr Create();
};
