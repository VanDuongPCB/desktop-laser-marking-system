#include "HxProfile.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include "HxFileManager.h"

namespace
{
HxProfileManager s_instance;
}


HxProfile::HxProfile()
{

}

HxProfile::~HxProfile()
{

}

/* -------------------------------------------------------- */
// std::vector<std::shared_ptr<HxProfile>> HxProfile::items;
// void HxProfile::load()
// {
//     items.clear();
//     QString dir = GetFileManager()->GetPath(HxFileManager::eDBSettingDir);
//     QDir().mkdir( dir );

//     QFile fileReader( GetFileManager()->GetPath(HxFileManager::eDBSettingFile) );
//     if ( fileReader.open( QIODevice::ReadOnly ) )
//     {
//         QByteArray json = fileReader.readAll();
//         fileReader.close();
//         QJsonArray arr = QJsonDocument::fromJson( json ).array();
//         for ( auto it : arr )
//         {
//             QJsonObject obj = it.toObject();
//             HxProfile* user = new HxProfile();
//             items.emplace_back( user );
//             user->name = obj.value( "name" ).toString();
//             user->pass = obj.value( "pass" ).toString();
//         }
//     }
// }
// void HxProfile::save()
// {
//     QString dir = GetFileManager()->GetPath(HxFileManager::eDBSettingDir);
//     QDir().mkdir( dir );

//     QJsonArray arr;
//     for ( auto& it : items )
//     {
//         QJsonObject obj;
//         obj.insert( "name", it->name );
//         obj.insert( "pass", it->pass );
//         arr.push_back( obj );
//     }

//     QJsonDocument doc;
//     doc.setArray( arr );

//     QFile fileWriter( GetFileManager()->GetPath(HxFileManager::eDBSettingFile));
//     if ( fileWriter.open( QIODevice::WriteOnly ) )
//     {
//         fileWriter.write( doc.toJson() );
//         fileWriter.close();
//     }
// }

HxProfilePtrArray HxProfileManager::GetProfiles()
{
    HxProfilePtrArray items;
    QString profileDir = GetFileManager()->GetPath(HxFileManager::eDBProfileDir);
    if(QDir(profileDir).exists())
    {
        QFileInfoList fileInfos = QDir( profileDir ).entryInfoList( { "*.profile" } );
        for ( auto& fileInfo : fileInfos )
        {
            QFile fileReader(fileInfo.absoluteFilePath());
            if(!fileReader.open(QIODevice::ReadOnly))
                continue;

            QByteArray json = fileReader.readAll();
            fileReader.close();

            QJsonDocument doc = QJsonDocument::fromJson( json );
            QJsonObject obj = doc.object();

            QString name = fileInfo.baseName().toUpper();
            HxProfilePtr pProfile = std::make_shared<HxProfile>();
            pProfile->name = name;
            pProfile->pass = obj.value("password").toString();
            items.push_back(pProfile);
        }
    }
    else
    {
        QFile fileReader( GetFileManager()->GetPath(HxFileManager::eDBProfileFile) );
        if ( fileReader.open( QIODevice::ReadOnly ) )
        {
            QByteArray json = fileReader.readAll();
            fileReader.close();
            QJsonArray arr = QJsonDocument::fromJson( json ).array();
            for ( auto it : arr )
            {
                QJsonObject obj = it.toObject();
                HxProfilePtr pProfile = std::make_shared<HxProfile>();
                pProfile->name = obj.value( "name" ).toString();
                pProfile->pass = obj.value( "pass" ).toString();
                items.emplace_back( pProfile );
                Save(pProfile);
            }
        }

    }
    return items;
}

HxProfilePtr HxProfileManager::GetProfile(const QString& name)
{
    HxProfilePtrArray items = GetProfiles();
    for(auto &pProfile : items)
    {
        if(pProfile->name.trimmed().toUpper() == name.trimmed().toUpper())
            return pProfile;
    }
    HxProfilePtr empty;
    return empty;
}

void HxProfileManager::Save(HxProfilePtr pProfile)
{
    if(!pProfile)
        return;
    QString profileDir = GetFileManager()->GetPath(HxFileManager::eDBProfileDir);
    QDir().mkdir(profileDir);
    QJsonObject obj;
    obj.insert( "password", pProfile->pass );
    QJsonDocument doc;
    doc.setObject(obj);
    QFile fileWriter( profileDir + "/" + pProfile->name + ".profile");
    if ( fileWriter.open( QIODevice::WriteOnly ) )
    {
        fileWriter.write( doc.toJson() );
        fileWriter.close();
    }
}

void HxProfileManager::Remove(const QString& profileName)
{
    QString profileDir = GetFileManager()->GetPath(HxFileManager::eDBProfileDir);
    QDir().mkdir(profileDir);
    QString path = profileDir + "/" + profileName + ".profile";
    QFile::remove(path);
}

HxProfileManager* GetProfileManager()
{
    return &s_instance;
}
