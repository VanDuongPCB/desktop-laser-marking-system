#include "HxFileManager.h"
#include "QCoreApplication"
#include "QDir"
#include "QDirIterator"
#include "QSettings"

NxFileManager::NxFileManager()
{
    auto settings = GetSettings( eSettingData );
    QString rootDir = settings->value( "RootDataDir" ).toString();
    if ( rootDir.isEmpty() )
    {
        rootDir = QCoreApplication::applicationDirPath() + "/Data";
        settings->setValue( "RootDataDir", rootDir );
    }
    QDir().mkdir( rootDir );
}

QString NxFileManager::GetPath( FileType fileType ) const
{
    switch ( fileType )
    {
    case eApplicationDir:
    {
        return qApp->applicationDirPath();
    }
    break;
    case eDBRootDir:
    {
        auto settings = GetSettings( eSettingData );
        QString rootDir = settings->value( "RootDataDir" ).toString();
        return rootDir;
    }
    break;
    case eDBOldRootDir:
    {
        auto settings = GetSettings( eSettingData );
        QString rootDir = settings->value( "RootOldDataDir" ).toString();
        return rootDir;
    }
    break;
    case eDBProfiles:
    {
        QString dir = GetPath( eDBRootDir );
        QString file = dir + "/Profiles.db";
        return file;
    }
    break;
    case eDBPlans:
    {
        QString dir = GetPath( eDBRootDir );
        QString file = dir + "/Plans.db";
        return file;
    }
    break;
    case eDBModels:
    {
        QString dir = GetPath( eDBRootDir );
        QString file = dir + "/Models.db";
        return file;
    }
    break;
    case eDBDesigns:
    {
        QString dir = GetPath( eDBRootDir );
        QString file = dir + "/Designs.db";
        return file;
    }
    break;
    case eDBHardware:
    {
        QString dir = GetPath( eDBRootDir );
        QString file = dir + "/Hardware.db";
        return file;
    }
    break;
    case eDBData:
    {
        QString dir = GetPath( eDBRootDir );
        QString file = dir + "/Logs.db";
        return file;
    }
    break;
    case eDefaultFilesDir:
    {
        QString dir = GetPath( eApplicationDir );
        QString file = dir + "/Default Files";
        return file;
    }
    break;
    default:
        break;
    }
    return "";
}

QStringList NxFileManager::GetFilesAndFolders( const QString& path, QDir::Filters filters ) const
{
    QStringList items;
    QDirIterator it( path, QDir::AllEntries | QDir::NoDotAndDotDot, QDirIterator::Subdirectories );

    while ( it.hasNext() )
    {
        QString entryPath = it.next();
        QFileInfo info( entryPath );

        if ( filters & QDir::Dirs && info.isDir() )
            items.push_back( entryPath );
        else if ( filters & QDir::Files && info.isFile() )
            items.push_back( entryPath );
    }
    return items;
}

NxRegSettingsPtr NxFileManager::GetSettings( SettingType type ) const
{
    QString dir;
    switch ( type )
    {
    case eSettingData:
        dir = "Data";
        break;
    case eSettingLicense:
        dir = "License";
        break;
    case eSettingTheme:
        dir = "Themes";
    default:
        break;
    }
    NxRegSettingsPtr pSettings = std::make_shared<NxRegSettings>( QString( "HKEY_CURRENT_USER\\Software\\Laser Marker System\\%1" ).arg( dir ), QSettings::NativeFormat );
    return pSettings;
}

NxFileManagerPtr NxFileManager::Create()
{
    return std::make_shared<NxFileManager>();
}
