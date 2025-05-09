#include "HxStopper.h"
#include "QCoreApplication"
#include "QDir"
#include "QFile"
#include "QJsonObject"
#include "QJsonArray"
#include "QJsonDocument"

#include "HxFileManager.h"
#include "HxDatabase.h"
#include "HxEvent.h"
//
//QMap<int, std::shared_ptr<HxStopper>> HxStopper::items;
//
//void HxStopper::load()
//{
//    //if ( !items.contains( 1 ) ) items.insert( 1, std::make_shared<HxStopper>( HxStopper() ) );
//    //if ( !items.contains( 2 ) ) items.insert( 2, std::make_shared<HxStopper>( HxStopper() ) );
//    //if ( !items.contains( 3 ) ) items.insert( 3, std::make_shared<HxStopper>( HxStopper() ) );
//
//    //QString dir = FileManager()->GetPath( HxFileManager::eDBSettingDir );
//    //QDir().mkdir( dir );
//    //QString path = FileManager()->GetPath( HxFileManager::eDBStopperFile );
//    //QFile reader( path );
//    //if ( reader.open( QIODevice::ReadOnly ) )
//    //{
//    //    QByteArray json = reader.readAll();
//    //    reader.close();
//    //    QJsonDocument doc = QJsonDocument::fromJson( json );
//    //    QJsonArray arr = doc.array();
//    //    for ( auto arrItem : arr )
//    //    {
//    //        QJsonObject obj = arrItem.toObject();
//    //        int index = obj.value( "index" ).toInt( 0 );
//    //        if ( !items.contains( index ) ) continue;
//    //        items[ index ]->x = obj.value( "x" ).toDouble( 0 );
//    //        items[ index ]->y = obj.value( "y" ).toDouble( 0 );
//    //    }
//    //}
//}
//
//void HxStopper::save()
//{
//    //QJsonArray arr;
//    //for ( int i = 1; i <= 3; i++ )
//    //{
//    //    if ( !items.contains( i ) ) continue;
//    //    QJsonObject obj;
//    //    obj.insert( "index", i );
//    //    obj.insert( "x", items[ i ]->x );
//    //    obj.insert( "y", items[ i ]->y );
//    //    arr.push_back( obj );
//    //}
//
//    //QJsonDocument doc;
//    //doc.setArray( arr );
//
//
//    //QString dir = FileManager()->GetPath( HxFileManager::eDBSettingDir );
//    //QDir().mkdir( dir );
//    //QString path = FileManager()->GetPath( HxFileManager::eDBStopperFile );
//    //QFile writer( path );
//    //if ( writer.open( QIODevice::WriteOnly ) )
//    //{
//    //    writer.write( doc.toJson() );
//    //    writer.close();
//    //}
//}
//
//std::shared_ptr<HxStopper> HxStopper::find( int index )
//{
//    if ( items.contains( index ) ) return items[ index ];
//    return {};
//}

HxStopperManager::HxStopperManager() : QObject( nullptr )
{
    //qApp->installEventFilter( this );
}

HxStopperPtr HxStopperManager::Create()
{
    return std::make_shared<HxStopper>();
}

HxStopperPtr HxStopperManager::GetStopper( int index )
{
    HxStopperPtr pStopper;
    HxStopperPtrMap map;

    QString dbFilePath = m_settings.String( DatabaseFilePath );

    HxDatabase db = HxDatabase::database( "SQLITE" );
    db.setDatabaseName( dbFilePath );
    if ( !db.open() )
        return pStopper;
    HxQuery query( db );
    QString cmd = QString( "SELECT Number,X,Y FROM Stoppers WHERE Number='%1'" ).arg( index );
    if ( !query.exec( cmd ) )
    {
        db.close();
        return pStopper;
    }

    while ( query.next() )
    {
        if ( !pStopper )
        {
            pStopper = Create();
            pStopper->x = query.value( 1 ).toDouble();
            pStopper->y = query.value( 2 ).toDouble();
        }
    }
    db.close();
    return pStopper;
}

HxStopperPtrMap HxStopperManager::GetStoppers()
{
    HxStopperPtrMap map;

    QString dbFilePath = m_settings.String( DatabaseFilePath );

    HxDatabase db = HxDatabase::database( "SQLITE" );
    db.setDatabaseName( dbFilePath );
    if ( !db.open() )
        return map;
    HxQuery query( db );
    QString cmd = QString( " SELECT Number,X,Y FROM Stoppers" );
    if ( !query.exec( cmd ) )
    {
        db.close();
        return map;
    }
    while ( query.next() )
    {
        HxStopperPtr pStopper = Create();
        int index = query.value( 0 ).toInt();
        pStopper->x = query.value( 1 ).toDouble();
        pStopper->y = query.value( 2 ).toDouble();
        map[ index ] = pStopper;
    }
    db.close();
    return map;
}

void HxStopperManager::Save( int index, HxStopperPtr pStopper )
{
    QString dbFilePath = m_settings.String( DatabaseFilePath );

    HxDatabase db = HxDatabase::database( "SQLITE" );
    db.setDatabaseName( dbFilePath );
    if ( !db.open() )
        return;
    HxQuery query( db );
    QString cmd = QString( "UPDATE Stoppers "
                           "SET X = '%1',Y = '%2' "
                           "WHERE Number = '%3'" )
        .arg( pStopper->x )
        .arg( pStopper->y )
        .arg( index );
    query.exec( cmd );
    db.close();
}

bool HxStopperManager::eventFilter( QObject* watched, QEvent* event )
{
    HxEvent* hxEvent( nullptr );
    HxEvent::Type type;
    if ( !HxEvent::IsCustomEvent( event, hxEvent, type ) )
        return QObject::eventFilter( watched, event );

    switch ( type )
    {
    case HxEvent::eSettingChanged:
        m_settings.Load();
        break;
    default:
        break;
    }
    return QObject::eventFilter( watched, event );
}

void HxStopperManager::Migration( const QString& dir )
{
    QDir().mkdir( dir );
    QString path = dir + "/stoppers.json";
    QFile reader( path );
    if ( !reader.open( QIODevice::ReadOnly ) )
        return;

    QByteArray json = reader.readAll();
    reader.close();
    QJsonDocument doc = QJsonDocument::fromJson( json );
    QJsonArray arr = doc.array();
    for ( auto arrItem : arr )
    {
        QJsonObject obj = arrItem.toObject();
        HxStopperPtr pStopper = Create();
        int index = obj.value( "index" ).toInt( 0 );
        pStopper->x = obj.value( "x" ).toDouble( 0 );
        pStopper->y = obj.value( "y" ).toDouble( 0 );
        Save( index, pStopper );
    }
}
HxStopperManager* StopperManager()
{
    static HxStopperManager instance;
    return &instance;
}