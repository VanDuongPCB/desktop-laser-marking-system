#include "HxFileManager.h"
#include "QCoreApplication"
#include "QDir"
#include "QDirIterator"
#include "QSettings"

namespace
{
HxFileManager s_instance;
}

HxFileManager::HxFileManager()
{}

void HxFileManager::Init()
{
    auto settings = GetSettings(eSettingData);
    QString rootDir = settings->value("RootDataDir").toString();
    if(rootDir.isEmpty())
    {
        rootDir = QCoreApplication::applicationDirPath();
        settings->setValue("RootDataDir", rootDir);
    }
    QDir().mkdir(rootDir);
}

QString HxFileManager::GetPath(FileType fileType) const
{
    switch (fileType)
	{
    case eApplicationDir: 
        return qApp->applicationDirPath();
    case eDBDefaultFilesDir: 
        return qApp->applicationDirPath() + "/Default Files";
    case eDBRootDir:
        return GetSettings( eSettingData )->value( "RootDataDir" ).toString();
    case eDBDataDir:
        return GetPath( eDBRootDir ) + "/data";
    case eDBSettingDir:
        return GetPath( eDBRootDir ) + "/settings";
    case eDBLOTDir:
        return GetPath( eDBRootDir ) + "/data/lots";
    case eDBModelDir:
        return GetPath( eDBRootDir ) + "/data/models";
    case eDBDesignDir:
        return GetPath( eDBRootDir ) + "/data/models";
    case eDBPrintLogDir:
        return GetPath( eDBRootDir ) + "/data/print-logs";
    case eDBRePrintLogDir:
        return GetPath( eDBRootDir ) + "/data/reprint-logs";
    case eDBBarcodeDir:
        return GetPath( eDBRootDir ) + "/data/barcode-logs";
    case eDBIVProgramDir:
        return GetPath( eDBRootDir ) + "/data/iv-programs";
    case eDBSettingFile:
        return GetPath( eDBRootDir ) + "/settings/settings.json";
    case eDBProfileFile:
        return GetPath( eDBRootDir ) + "/settings/user.dat";
    case eDBStopperFile:
        return GetPath( eDBRootDir ) + "/settings/stoppers.json";
		default:
	break;
	}
    return "";
}

QStringList HxFileManager::GetFilesAndFolders(const QString &path, QDir::Filters filters) const
{
    QStringList items;
    QDirIterator it(path, QDir::AllEntries | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        QString entryPath = it.next();
        QFileInfo info(entryPath);

        if (filters & QDir::Dirs && info.isDir())
            items.push_back(entryPath);
        else if (filters & QDir::Files && info.isFile())
            items.push_back(entryPath);
    }
    return items;
}

NxRegSettingsPtr HxFileManager::GetSettings(SettingType type) const
{
    QString dir;
    switch (type) {
    case eSettingData:
        dir = "Data";
        break;
    case eSettingLicense:
        dir = "License";
        break;
    case eSettingTheme:
        dir = "Themes";
        break;
    case eSettingProtect:
        dir = "Protect";
        break;
    default:
        break;
    }
    NxRegSettingsPtr pSettings = std::make_shared<NxRegSettings>(QString("HKEY_CURRENT_USER\\Software\\Laser Marker System\\%1").arg(dir), QSettings::NativeFormat);
    return pSettings;
}

HxFileManager* GetFileManager()
{
    return &s_instance;
}
