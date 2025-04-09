#include "HxProfile.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include "HxFileManager.h"

HxProfile::HxProfile()
{

}

HxProfile::~HxProfile()
{

}

/* -------------------------------------------------------- */
std::vector<std::shared_ptr<HxProfile>> HxProfile::items;
void HxProfile::load()
{
    items.clear();
    QString dir = GetFileManager()->GetPath(HxFileManager::eDBSettingDir);
    QDir().mkdir( dir );

    QFile fileReader( GetFileManager()->GetPath(HxFileManager::eDBSettingFile) );
    if ( fileReader.open( QIODevice::ReadOnly ) )
    {
        QByteArray json = fileReader.readAll();
        fileReader.close();
        QJsonArray arr = QJsonDocument::fromJson( json ).array();
        for ( auto it : arr )
        {
            QJsonObject obj = it.toObject();
            HxProfile* user = new HxProfile();
            items.emplace_back( user );
            user->name = obj.value( "name" ).toString();
            user->pass = obj.value( "pass" ).toString();
        }
    }
}
void HxProfile::save()
{
    QString dir = GetFileManager()->GetPath(HxFileManager::eDBSettingDir);
    QDir().mkdir( dir );

    QJsonArray arr;
    for ( auto& it : items )
    {
        QJsonObject obj;
        obj.insert( "name", it->name );
        obj.insert( "pass", it->pass );
        arr.push_back( obj );
    }

    QJsonDocument doc;
    doc.setArray( arr );

    QFile fileWriter( GetFileManager()->GetPath(HxFileManager::eDBSettingFile));
    if ( fileWriter.open( QIODevice::WriteOnly ) )
    {
        fileWriter.write( doc.toJson() );
        fileWriter.close();
    }
}
