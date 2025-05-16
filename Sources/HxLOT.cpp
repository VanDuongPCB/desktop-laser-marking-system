#include "HxLOT.h"

#include "QJsonObject"
#include "QJsonArray"
#include "QJsonDocument"
#include "QFile"
#include "QDir"
#include "QFileInfo"
#include "QFileInfoList"
#include "QCoreApplication"

#include "HxConvert.h"
#include "HxFileManager.h"
#include "HxDatabase.h"
#include "HxEvent.h"

HxLOT::HxLOT()
{
    m_comments[ "FIX1" ] = "";
    m_comments[ "FIX2" ] = "";
    m_comments[ "FIX3" ] = "";
    m_comments[ "FIX4" ] = "";
    m_comments[ "FIX5" ] = "";
}

HxLOT::~HxLOT()
{
}

QString HxLOT::Name() const
{
    return m_name;
}

QString HxLOT::CounterStart() const
{
    return m_counterStart;
}

QString HxLOT::CounterEnd() const
{
    int len = m_counterStart.length();
    int start = m_counterStart.toInt( 0 );
    int cnt = start + m_quantity - 1;
    return QString::number( cnt ).rightJustified( len, '0' );
}

QString HxLOT::Counter( int shift ) const
{
    int len = m_counterStart.length();
    int start = m_counterStart.toInt( 0 );
    int cnt = start + m_progress + shift;
    return QString::number( cnt ).rightJustified( len, '0' );
}

QString HxLOT::MACStart() const
{
    return m_macStart;
}

QString HxLOT::MACEnd() const
{
    return m_macEnd;
}

QString HxLOT::MAC( int shift ) const
{
    if ( m_macStart.length() != 12 || m_macEnd.length() != 12 )
    {
        return "LOT.MAC?";
    }
    uint64_t start = Uint64FromHexString( m_macStart );
    uint64_t cnt = start + m_progress + shift;
    return HexStringFromUint64( cnt, 12 );
}

int HxLOT::Quantity() const
{
    return m_quantity;
}

int HxLOT::Progress() const
{
    return m_progress;
}

QString HxLOT::Model() const
{
    return m_modelName;
}

bool HxLOT::IsRePrint() const
{
    return m_isRePrint;
}

QString HxLOT::Value( QString paramName ) const
{
    if ( paramName == "NAME" )
        return m_name.split( "-RE" ).first();
    else if ( paramName == "COUNTER" )
        return Counter();
    else if ( paramName == "MAC" )
        return MAC();
    else
    {
        auto it = m_comments.find( paramName );
        if ( it != m_comments.end() )
            return it->second;
        return "LOT." + paramName;
    }
}

std::map<QString, QString> HxLOT::Comments() const
{
    return m_comments;
}

bool HxLOT::IsCompleted()
{
    Evaluate();
    return m_progress >= m_quantity;
}

HxLOT::ProductStatus HxLOT::Status() const
{
    return m_status;
}

void HxLOT::Evaluate()
{
    if ( m_progress <= 0 )
        m_status = ePending;
    else if ( m_progress < m_quantity )
        m_status = eProduct;
    else
        m_status = eCompleted;
}

void HxLOT::SetName( const QString& value )
{
    Modify( m_name, value, eNew );
}

void HxLOT::SetCounterStart( const QString& value )
{
    Modify( m_counterStart, value, eInfo );
}

void HxLOT::SetMACStart( const QString& value )
{
    Modify( m_macStart, value, eInfo );
}

void HxLOT::SetMACEnd( const QString& value )
{
    Modify( m_macEnd, value, eInfo );
}

void HxLOT::SetQuantity( int value )
{
    Modify( m_quantity, value, eInfo );
}

void HxLOT::SetProgress( int value )
{
    Modify( m_progress, value, eProgress );
}

void HxLOT::SetModel( const QString& value )
{
    Modify( m_modelName, value, eInfo );
}

void HxLOT::SetRePrint( bool bIsEnabled )
{
    Modify( m_isRePrint, bIsEnabled, eInfo );
}

void HxLOT::SetValue( const QString& name, const QString& value )
{
    auto it = m_comments.find( name );
    if ( it == m_comments.end() || it->second != value )
    {
        m_comments[ name ] = value;
        SetModified( eComment );
    }
}

bool HxLOT::NextItem()
{
    m_progress++;
    return m_progress < m_quantity;
}

HxLOTPtr HxLOT::Clone() const
{
    auto pClone = LOTManager()->Create();
    *pClone.get() = *this;
    return pClone;
}

HxLOTManager::HxLOTManager() : QObject( nullptr )
{
    m_settings.Load();
}

HxLOTPtr HxLOTManager::Create()
{
    return std::make_shared<HxLOT>();
}

HxLOTPtr HxLOTManager::GetLOT( const QString& lotName )
{
    QString dbFilePath = m_settings.String( DatabaseFilePath );
    if ( !HxDatabase::CheckDatabaseFileExisting( dbFilePath ) )
        return nullptr;

    HxDatabase db = HxDatabase::database( "SQLITE" );
    db.setDatabaseName( dbFilePath );
    if ( !db.open() )
        return nullptr;
    HxQuery query( db );

    QString cmd = QString( "SELECT Name,CounterStart,MACStart,MACEnd,Quantity,Progress,Model,IsRePrint FROM LOTs WHERE Name='%1'" ).arg( lotName );
    if ( !query.exec( cmd ) )
    {
        qDebug() << query.lastError().text();
        db.close();
        return nullptr;
    }

    HxLOTPtr pLOT;
    if ( query.next() )
    {
        if ( !pLOT )
        {
            pLOT = Create();
            pLOT->SetName( query.value( 0 ).toString() );
            pLOT->SetCounterStart( query.value( 1 ).toString() );
            pLOT->SetMACStart( query.value( 2 ).toString() );
            pLOT->SetMACEnd( query.value( 3 ).toString() );
            pLOT->SetQuantity( query.value( 4 ).toInt() );
            pLOT->SetProgress( query.value( 5 ).toInt() );
            pLOT->SetModel( query.value( 6 ).toString() );
            pLOT->SetRePrint( query.value( 7 ).toInt() > 0 );
            pLOT->Evaluate();
        }
    }

    if ( pLOT )
    {
        cmd = QString( "SELECT Key,Value FROM LOTParams WHERE Name='%1'" ).arg( lotName );
        if ( query.exec( cmd ) )
        {
            while ( query.next() )
            {
                QString key = query.value( 0 ).toString();
                QString value = query.value( 1 ).toString();
                pLOT->SetValue( key, value );
            }
        }
    }

    db.close();

    if ( pLOT )
    {
        pLOT->Evaluate();
        pLOT->ClearModified();
    }

    return pLOT;
}

HxLOTPtrMap HxLOTManager::GetLOTs( const QDate& fromTime )
{
    HxLOTPtrMap map;
    QString dbFilePath = m_settings.String( DatabaseFilePath );
    if ( !HxDatabase::CheckDatabaseFileExisting( dbFilePath ) )
        return map;

    HxDatabase db = HxDatabase::database( "SQLITE" );
    db.setDatabaseName( dbFilePath );
    if ( !db.open() )
        return map;
    HxQuery query( db );
    QString sTime = fromTime.toString( "yyyy-MM-dd" );
    QString cmd = QString( "SELECT Name FROM LOTs WHERE FinishDate>'%1'" ).arg( sTime );
    if ( !query.exec( cmd ) )
        return map;

    std::set<QString> lotNames;

    while ( query.next() )
    {
        lotNames.insert( query.value( 0 ).toString() );
        /*HxLOTPtr pLOT = Create();
        pLOT->SetName( query.value( 0 ).toString() );
        pLOT->SetCounterStart( query.value( 1 ).toString() );
        pLOT->SetMACStart( query.value( 2 ).toString() );
        pLOT->SetMACEnd( query.value( 3 ).toString() );
        pLOT->SetQuantity( query.value( 4 ).toInt() );
        pLOT->SetProgress( query.value( 5 ).toInt() );
        pLOT->SetModel( query.value( 6 ).toString() );
        pLOT->SetRePrint( query.value( 7 ).toInt() > 0 );

        pLOT->Evaluate();
        pLOT->ClearModified();

        QString statusIndex = QString::number( pLOT->Status() );

        map[ statusIndex + "_" + pLOT->Name()] = pLOT;*/
    }

    for ( auto &name : lotNames )
    {
        HxLOTPtr pLOT = GetLOT( name );
        if ( !pLOT )
            continue;

        QString statusIndex = QString::number( pLOT->Status() );
        map[ statusIndex + "_" + pLOT->Name() ] = pLOT;
    }

    return map;
}

ReturnCode HxLOTManager::Save( HxLOTPtr pLOT )
{
    if ( !pLOT )
        return RtDataNull;

    if ( !pLOT->IsMofified() )
        return RtDataNoChanges;

    QString dbFilePath = m_settings.String( DatabaseFilePath );
    if ( !HxDatabase::CheckDatabaseFileExisting( dbFilePath ) )
        return RtDBFileNotFound;

    HxDatabase db = HxDatabase::database( "SQLITE" );
    db.setDatabaseName( dbFilePath );
    if ( !db.open() )
        return RtDBOpenFailed;
    HxQuery query( db );

    QString finishDate = "9999-99-99";
    if ( pLOT->Status() == HxLOT::eCompleted )
        finishDate = QDate::currentDate().toString( "yyyy-MM-dd" );

    if ( pLOT->IsMofified( HxLOT::eNew ) )
    {
        QString cmd = QString( "INSERT INTO LOTs "
                               "(Name,CounterStart,CounterEnd,Counter,MACStart,MACEnd,MAC,Quantity,Progress,Model,IsRePrint,FinishDate) "
                               "VALUES('%1','%2','%3','%4','%5','%6','%7',%8,%9,'%10',%11,'%12')" )
            .arg( pLOT->Name() )
            .arg( pLOT->CounterStart() )
            .arg( pLOT->CounterEnd() )
            .arg( pLOT->Counter() )
            .arg( pLOT->MACStart() )
            .arg( pLOT->MACEnd() )
            .arg( pLOT->MAC() )
            .arg( pLOT->Quantity() )
            .arg( pLOT->Progress() )
            .arg( pLOT->Model() )
            .arg( pLOT->IsRePrint() )
            .arg( finishDate );

        if ( !query.exec( cmd ) )
        {
            qDebug() << query.lastError().text();
            qDebug() << cmd;
            db.close();
            return RtDBQueryFailed;
        }
    }
    else
    {
        if ( pLOT->IsMofified( HxLOT::eProgress ) )
        {
            QString cmd = QString( "UPDATE LOTs "
                                   "SET Progress = '%1',FinishDate='%2' "
                                   "WHERE Name = '%3'" )
                .arg( pLOT->Progress() )
                .arg( finishDate )
                .arg( pLOT->Name() );
            if ( !query.exec( cmd ) )
            {
                qDebug() << query.lastError().text();
                qDebug() << cmd;
                db.close();
                return RtDBQueryFailed;
            }
        }

        if ( pLOT->IsMofified( HxLOT::eInfo ) )
        {
            QString cmd = QString( "UPDATE LOTs "
                                   "SET CounterStart = '%1',CounterEnd = '%2',MACStart = '%3',MACEnd = '%4',Quantity = '%5',Model = '%6' "
                                   "WHERE Name = '%7'" )
                .arg( pLOT->CounterStart() )
                .arg( pLOT->CounterEnd() )
                .arg( pLOT->MACStart() )
                .arg( pLOT->MACEnd() )
                .arg( pLOT->Quantity() )
                .arg( pLOT->Model() )
                .arg( pLOT->Name() );
            if ( !query.exec( cmd ) )
            {
                db.close();
                return RtDBQueryFailed;
            }
        }
    }

    if ( pLOT->IsMofified( HxLOT::eComment ) )
    {
        QString cmd = QString( "DELETE FROM LOTParams WHERE Name = '%1'" ).arg( pLOT->Name() );
        query.exec( cmd );
        auto comments = pLOT->Comments();
        for ( auto& [key, value] : comments )
        {
            cmd = QString( "INSERT INTO LOTParams "
                           "(Name,Key,Value) "
                           "VALUES('%1','%2','%3')" )
                .arg( pLOT->Name() )
                .arg( key )
                .arg( value );
            query.exec( cmd );
        }
    }

    db.close();
    bool isNew = pLOT->IsMofified( HxLOT::eNew );
    pLOT->ClearModified();
    //if ( isNew )
    //    qApp->postEvent( qApp, new HxEvent( HxEvent::eLOTAdded ) );
    //else
    //    qApp->postEvent( qApp, new HxEvent( HxEvent::eLOTChanged ) );
    return RtNormal;
}

ReturnCode HxLOTManager::Remove( HxLOTPtr pLOT )
{
    if ( !pLOT )
        return RtDataNull;

    if ( pLOT->Progress() > 0 )
        return RtDataCannotDelete;

    QString dbFilePath = m_settings.String( DatabaseFilePath );
    if ( !HxDatabase::CheckDatabaseFileExisting( dbFilePath ) )
        return RtDBFileNotFound;

    HxDatabase db = HxDatabase::database( "SQLITE" );
    db.setDatabaseName( dbFilePath );
    if ( !db.open() )
    {
        return RtDBOpenFailed;
    }
    HxQuery query( db );
    QString cmd = QString( "DELETE FROM LOTs WHERE Name = '%1'" ).arg( pLOT->Name() );
    if ( !query.exec( cmd ) )
    {
        db.close();
        return RtDBQueryFailed;
    }

    cmd = QString( "DELETE FROM LOTParams WHERE Name = '%1'" ).arg( pLOT->Name() );
    if ( !query.exec( cmd ) )
    {
        db.close();
        return RtDBQueryFailed;
    }

    db.close();
    return RtNormal;
}

ReturnCode HxLOTManager::Remove( const QString& name )
{
    QString dbFilePath = m_settings.String( DatabaseFilePath );
    if ( !HxDatabase::CheckDatabaseFileExisting( dbFilePath ) )
        return RtDBFileNotFound;

    HxDatabase db = HxDatabase::database( "SQLITE" );
    db.setDatabaseName( dbFilePath );
    if ( !db.open() )
    {
        return RtDBOpenFailed;
    }
    HxQuery query( db );
    QString cmd = QString( "DELETE FROM LOTs WHERE Name = '%1'" ).arg( name );
    if ( !query.exec( cmd ) )
    {
        db.close();
        return RtDBQueryFailed;
    }

    cmd = QString( "DELETE FROM LOTParams WHERE Name = '%1'" ).arg( name );
    if ( !query.exec( cmd ) )
    {
        db.close();
        return RtDBQueryFailed;
    }

    db.close();
    return RtNormal;
}

ReturnCode HxLOTManager::Removes( const std::set<QString>& names )
{
    for ( auto& name : names )
    {
        Remove( name );
    }
    return RtNormal;
}

QStringList HxLOTManager::Parameters()
{
    QStringList items = { "FIX1","FIX2","FIX3","FIX4","FIX5" };
    std::sort( m_paramNames.begin(), m_paramNames.end() );
    for ( auto& name : m_paramNames )
    {
        if ( items.contains( name ) )
            continue;
        items.push_back( name );
    }
    return items;
}

//ReturnCode HxLOTManager::Migration( const QString& dir )
//{
//    QString lotDir = dir;
//    QDir().mkdir( lotDir );
//    QFileInfoList fileInfos = QDir( lotDir ).entryInfoList( { "*.lot" } );
//
//    HxLOTPtrArray pLOTs;
//    std::set<QString> paramNames;
//
//    for ( auto& fileInfo : fileInfos )
//    {
//        QString lotName = fileInfo.baseName().toUpper();
//        QString filePath = dir + "/" + lotName + ".lot";
//
//        QFileInfo fileInfo( filePath );
//        QFile fileReader( filePath );
//        if ( !fileReader.open( QIODevice::ReadOnly ) )
//            continue;
//
//        QDateTime lastWrite = fileInfo.lastModified();
//
//        QByteArray json = fileReader.readAll();
//        fileReader.close();
//
//        QJsonDocument doc = QJsonDocument::fromJson( json );
//        QJsonObject obj = doc.object();
//
//        HxLOTPtr pLOT = Create();
//
//        pLOT->SetName( fileInfo.baseName().toUpper() );
//        pLOT->SetMACStart( obj.value( "mac-start" ).toString() );
//        pLOT->SetMACEnd( obj.value( "mac-end" ).toString() );
//        pLOT->SetCounterStart( obj.value( "counter-start" ).toString() );
//        pLOT->SetQuantity( obj.value( "quantity" ).toInt() );
//        pLOT->SetProgress( obj.value( "progress" ).toInt( 0 ) );
//        pLOT->SetModel( obj.value( "model" ).toString() );
//        pLOT->SetRePrint( obj.value( "is-re-print" ).toBool() );
//        QJsonObject cmtObj = obj.value( "comments" ).toObject();
//        QStringList keys = cmtObj.keys();
//        for ( auto& key : keys )
//        {
//            pLOT->SetValue( key, cmtObj.value( key ).toString() );
//            paramNames.insert( key.trimmed().toUpper() );
//        }
//        pLOT->Evaluate();
//        pLOTs.push_back( pLOT );
//    }
//
//    QString cmdFormat = "";
//    for ( auto &pLOT : pLOTs )
//        Save( pLOT );
//
//    for ( auto &paramName : paramNames )
//    {
//
//    }
//    return RtNormal;
//}

ReturnCode HxLOTManager::DeleteAll()
{
    QString dbFilePath = m_settings.String( DatabaseFilePath );
    if ( !HxDatabase::CheckDatabaseFileExisting( dbFilePath ) )
        return RtDBFileNotFound;

    HxDatabase db = HxDatabase::database( "SQLITE" );
    db.setDatabaseName( dbFilePath );
    if ( !db.open() )
    {
        return RtDBOpenFailed;
    }
    HxQuery query( db );
    QString cmd;

    cmd = QString( "DELETE FROM LOTs" );
    if ( !query.exec( cmd ) )
    {
        db.close();
        return RtDBQueryFailed;
    }

    cmd = QString( "DELETE FROM LOTParams" );
    if ( !query.exec( cmd ) )
    {
        db.close();
        return RtDBQueryFailed;
    }

    db.close();
    return RtNormal;
}

void HxLOTManager::ReloadSetting()
{
    m_settings.Load();
}

HxLOTManager* LOTManager()
{
    static HxLOTManager instance;
    return &instance;
}
