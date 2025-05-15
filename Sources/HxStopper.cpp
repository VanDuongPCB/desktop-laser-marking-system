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

HxStopperManager::HxStopperManager() : QObject( nullptr )
{
    m_settings.Load();
}

HxStopperPtr HxStopperManager::Create()
{
    return std::make_shared<HxStopper>();
}

HxStopperPtr HxStopperManager::GetStopper( int index )
{
    HxStopperPtr pStopper;

    QString dbFilePath = m_settings.String( DatabaseFilePath );
    if ( !HxDatabase::CheckDatabaseFileExisting( dbFilePath ) )
        return pStopper;

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
    if ( !HxDatabase::CheckDatabaseFileExisting( dbFilePath ) )
        return map;

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

ReturnCode HxStopperManager::Save( int index, HxStopperPtr pStopper )
{
    if ( !pStopper )
        return RtDataNull;

    QString dbFilePath = m_settings.String( DatabaseFilePath );
    if ( !HxDatabase::CheckDatabaseFileExisting( dbFilePath ) )
        return RtDBFileNotFound;

    HxDatabase db = HxDatabase::database( "SQLITE" );
    db.setDatabaseName( dbFilePath );
    if ( !db.open() )
        return RtDBOpenFailed;

    HxQuery query( db );
    QString cmd = QString( "UPDATE Stoppers "
                           "SET X = '%1',Y = '%2' "
                           "WHERE Number = '%3'" )
        .arg( pStopper->x )
        .arg( pStopper->y )
        .arg( index );
    if ( !query.exec( cmd ) )
    {
        db.close();
        return RtDBQueryFailed;
    }
    db.close();
    return RtNormal;
}

void HxStopperManager::ReLoadSetting()
{
    m_settings.Load();
}

HxStopperManager* StopperManager()
{
    static HxStopperManager instance;
    return &instance;
}