#include "HxStopper.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>


HxStopper::HxStopper()
{

}

HxStopper::~HxStopper()
{

}

QMap<int, std::shared_ptr<HxStopper>> HxStopper::items;

void HxStopper::Load()
{
    if ( !items.contains( 1 ) ) items.insert( 1, std::make_shared<HxStopper>( HxStopper() ) );
    if ( !items.contains( 2 ) ) items.insert( 2, std::make_shared<HxStopper>( HxStopper() ) );
    if ( !items.contains( 3 ) ) items.insert( 3, std::make_shared<HxStopper>( HxStopper() ) );

    QString dir = QCoreApplication::applicationDirPath() + "/settings";
    QDir().mkdir( dir );
    QString path = dir + "/stoppers.json";
    QFile reader( path );
    if ( reader.open( QIODevice::ReadOnly ) )
    {
        QByteArray json = reader.readAll();
        reader.close();
        QJsonDocument doc = QJsonDocument::fromJson( json );
        QJsonArray arr = doc.array();
        for ( auto arrItem : arr )
        {
            QJsonObject obj = arrItem.toObject();
            int index = obj.value( "index" ).toInt( 0 );
            if ( !items.contains( index ) ) continue;
            items[ index ]->x = obj.value( "x" ).toDouble( 0 );
            items[ index ]->y = obj.value( "y" ).toDouble( 0 );
        }
    }
}

void HxStopper::Save()
{
    QJsonArray arr;
    for ( int i = 1; i <= 3; i++ )
    {
        if ( !items.contains( i ) ) continue;
        QJsonObject obj;
        obj.insert( "index", i );
        obj.insert( "x", items[ i ]->x );
        obj.insert( "y", items[ i ]->y );
        arr.push_back( obj );
    }

    QJsonDocument doc;
    doc.setArray( arr );


    QString dir = QCoreApplication::applicationDirPath() + "/settings";
    QDir().mkdir( dir );
    QString path = dir + "/stoppers.json";
    QFile writer( path );
    if ( writer.open( QIODevice::WriteOnly ) )
    {
        writer.write( doc.toJson() );
        writer.close();
    }
}

std::shared_ptr<HxStopper> HxStopper::Find( int index )
{
    if ( items.contains( index ) ) return items[ index ];
    return {};
}
