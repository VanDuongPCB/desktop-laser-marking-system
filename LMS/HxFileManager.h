#pragma once
#include "QString"
#include "QDir"
#include "QFile"
#include "QFileInfo"
#include "QSettings"

class HxFileManager;
using HxFileManagerPtr = std::shared_ptr<HxFileManager>;

using NxRegSettings = QSettings;
using NxRegSettingsPtr = std::shared_ptr<QSettings>;

class HxFileManager
{
public:
    enum FileType{
        eApplicationDir,
        eDBDefaultFilesDir,
        eDBRootDir,
        eDBDataDir,
        eDBSettingDir,
        eDBLOTDir,
        eDBModelDir,
        eDBDesignDir,
        eDBPrintLogDir,
        eDBRePrintLogDir,
        eDBBarcodeDir,
        eDBIVProgramDir,

        eDBSettingFile,
        eDBProfileFile,
        eDBStopperFile,
    };

    enum SettingType
    {
        eSettingData,
        eSettingLicense,
        eSettingTheme,
        eSettingProtect,
    };

    HxFileManager();
    QString GetPath(FileType fileType) const;
    QStringList GetFilesAndFolders(const QString &path, QDir::Filters filters = QDir::Dirs| QDir::Files) const;
    NxRegSettingsPtr GetSettings(SettingType type) const;
};

HxFileManager* GetFileManager();
