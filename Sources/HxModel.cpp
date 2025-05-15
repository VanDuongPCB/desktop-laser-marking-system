#include "HxModel.h"
#include "QFileInfo"
#include "QJsonDocument"
#include "QJsonArray"
#include "QJsonObject"
#include "QStringList"
#include "QDir"
#include "QFile"
#include "QCoreApplication"

#include "HxFileManager.h"
#include "HxDatabase.h"
#include "HxEvent.h"

HxModel::HxModel() : HxObject()
{
    m_comments[ "FIX1" ] = "";
    m_comments[ "FIX2" ] = "";
    m_comments[ "FIX3" ] = "";
    m_comments[ "FIX4" ] = "";
    m_comments[ "FIX5" ] = "";
}

HxModel::~HxModel()
{

}

QString HxModel::Code() const
{
    return m_code;
}

QString HxModel::Name() const
{
    return m_name;
}

bool HxModel::IsPrintLo() const
{
    return m_bIsPrintLo;
}

QString HxModel::kNo() const
{
    return m_kNo;
}

QString HxModel::IVProgram() const
{
    return m_ivProgram;
}

QString HxModel::Design() const
{
    return m_design;
}

double HxModel::CvWidth() const
{
    return m_cvWidth;
}

int HxModel::Stopper() const
{
    return m_stopper;
}

std::map<int, HxPosition>& HxModel::Positions()
{
    return m_positions;
}

HxPosition HxModel::Position( int index ) const
{
    auto it = m_positions.find( index );
    if ( it != m_positions.end() )
        return it->second;
    return HxPosition();
}

std::map<QString, QString>& HxModel::Comments()
{
    return m_comments;
}

QString HxModel::Comment( const QString& name ) const
{
    auto it = m_comments.find( name );
    if ( it != m_comments.end() )
        return it->second;
    return QString();
}

QString HxModel::Value( QString paramName ) const
{
    if ( paramName == "CODE" ) return m_code;
    else if ( paramName == "NAME" ) return m_name;
    else if ( paramName == "NO" ) return m_kNo;
    else
    {
        auto it = m_comments.find( paramName );
        if ( it != m_comments.end() )
            return it->second;
        return "MODEL." + paramName;
    }
}

void HxModel::SetCode( const QString& value )
{
    Modify( m_code, value, eNew );
}

void HxModel::SetName( const QString& value )
{
    Modify( m_name, value, eInfo );
}

void HxModel::SetPrintLo( bool bIsEnable )
{
    Modify( m_bIsPrintLo, bIsEnable, eInfo );
}

void HxModel::SetkNo( const QString& value )
{
    Modify( m_kNo, value, eInfo );
}

void HxModel::SetIVProgram( const QString& value )
{
    Modify( m_ivProgram, value, eInfo );
}

void HxModel::SetDesign( const QString& value )
{
    Modify( m_design, value, eInfo );
}

void HxModel::SetDesign( size_t value )
{
    SetDesign( QString::number( value ).rightJustified( 4, '0' ) );
}

void HxModel::SetCvWidth( double value )
{
    Modify( m_cvWidth, value, eInfo );
}

void HxModel::SetStopper( int value )
{
    Modify( m_stopper, value, eInfo );
}

void HxModel::SetPositions( const std::map<int, HxPosition>& value )
{
    m_positions = value;
    SetModified( ePosition );
}

void HxModel::SetPosition( int index, const HxPosition& value )
{
    m_positions[ index ] = value;
    SetModified( ePosition );
}

void HxModel::RemovePosition( int index )
{
    m_positions.erase( index );
    SetModified( ePosition );
}

void HxModel::SetComments( const std::map<QString, QString>& comments )
{
    m_comments = comments;
    SetModified( eComment );
}

void HxModel::SetComment( const QString& key, const QString& value )
{
    m_comments[ key.trimmed().toUpper() ] = value;
    SetModified( eComment );
}

void HxModel::AddComments( const QStringList& keys )
{
    for ( auto& key : keys )
    {
        auto it = m_comments.find( key.trimmed().toUpper() );
        if ( it == m_comments.end() )
        {
            m_comments[ key.trimmed().toUpper() ] = "";
            SetModified( eComment );
        }
    }
}

void HxModel::RemoveComments( const QStringList& keys )
{
    for ( auto& key : keys )
    {
        auto it = m_comments.find( key );
        if ( it != m_comments.end() )
        {
            m_comments.erase( key );
            SetModified( eComment );
        }

        it = m_comments.find( key.trimmed().toUpper() );
        if ( it != m_comments.end() )
        {
            m_comments.erase( key.trimmed().toUpper() );
            SetModified( eComment );
        }
    }
}

void HxModel::SetValue( const QString& key, const QString& value )
{
    m_comments[ key.trimmed().toUpper() ] = value;
    SetModified( eComment );
}

QStringList HxModel::paramNames()
{
    QStringList names = { "NAME","CODE","NO","FIX1","FIX2","FIX3","FIX4","FIX5" };
    // for ( auto& it : items )
    // {
    //     for ( auto& [key, value] : it->m_comments )
    //     {
    //         if ( names.contains( key ) == false )
    //         {
    //             names.push_back( key );
    //         }
    //     }
    // }
    return names;
}

HxModelManager::HxModelManager() :QObject( nullptr )
{
    m_settings.Load();
}

HxModelPtr HxModelManager::Create( const QString& code, const QString& name )
{
    auto pModel = std::make_shared<HxModel>();
    pModel->SetCode( code );
    pModel->SetName( name );
    return pModel;
}

QStringList HxModelManager::Names()
{
    QStringList items;
    QString dbFilePath = m_settings.String( DatabaseFilePath );
    if ( !HxDatabase::CheckDatabaseFileExisting( dbFilePath ) )
        return items;

    HxDatabase db = HxDatabase::database( "SQLITE" );
    db.setDatabaseName( dbFilePath );
    if ( !db.open() )
        return items;
    HxQuery query( db );

    QString cmd = QString( "SELECT Name FROM Models" );
    if ( !query.exec( cmd ) )
    {
        db.close();
        return items;
    }

    while ( query.next() )
    {
        items.push_back( query.value( 0 ).toString() );
    }
    db.close();
    return items;
}

QStringList HxModelManager::ParamNames()
{
    QStringList items;
    return items;
}

HxModelPtrMap HxModelManager::GetModels()
{
    HxModelPtrMap map;
    QString dbFilePath = m_settings.String( DatabaseFilePath );
    if ( !HxDatabase::CheckDatabaseFileExisting( dbFilePath ) )
        return map;
   
    HxDatabase db = HxDatabase::database( "SQLITE" );
    db.setDatabaseName( dbFilePath );
    if ( !db.open() )
        return map;
    HxQuery query( db );

    QString cmd = QString( "SELECT Code,Name,Design,kNo,IV,CvWidth,Stopper,IsPrintLo FROM Models" );
    if ( !query.exec( cmd ) )
    {
        db.close();
        qDebug() << query.lastError().text();
        return map;
    }

    while ( query.next() )
    {
        HxModelPtr pModel = Create();
        pModel->SetCode( query.value( 0 ).toString() );
        pModel->SetName( query.value( 1 ).toString() );
        pModel->SetDesign( query.value( 2 ).toString() );
        pModel->SetkNo( query.value( 3 ).toString() );
        pModel->SetIVProgram( query.value( 4 ).toString() );
        pModel->SetCvWidth( query.value( 5 ).toDouble() );
        pModel->SetStopper( query.value( 6 ).toInt() );
        pModel->SetPrintLo( query.value( 7 ).toInt() > 0 );
        pModel->ClearModified();
        map[ pModel->Code() ] = pModel;
    }

    return map;
}

HxModelPtr HxModelManager::GetModel( const QString& code )
{
    QString dbFilePath = m_settings.String( DatabaseFilePath );
    if ( !HxDatabase::CheckDatabaseFileExisting( dbFilePath ) )
        return nullptr;
    
    HxDatabase db = HxDatabase::database( "SQLITE" );
    db.setDatabaseName( dbFilePath );
    if ( !db.open() )
        return nullptr;
    HxQuery query( db );

    QString cmd = QString( "SELECT Code,Name,Design,kNo,IV,CvWidth,Stopper,IsPrintLo "
                           "FROM Models "
                           "WHERE Code='%1' OR Name='%1'" )
        .arg( code );

    if ( !query.exec( cmd ) )
    {
        qDebug() << query.lastError().text();
        db.close();
        return nullptr;
    }

    HxModelPtr pModel;
    while ( query.next() )
    {
        if ( !pModel )
        {
            pModel = Create();
            pModel->SetCode( query.value( 0 ).toString() );
            pModel->SetName( query.value( 1 ).toString() );
            pModel->SetDesign( query.value( 2 ).toString() );
            pModel->SetkNo( query.value( 3 ).toString() );
            pModel->SetIVProgram( query.value( 4 ).toString() );
            pModel->SetCvWidth( query.value( 5 ).toDouble() );
            pModel->SetStopper( query.value( 6 ).toInt() );
            pModel->SetPrintLo( query.value( 7 ).toInt() > 0 );
        }
    }

    if ( !pModel )
    {
        db.close();
        return pModel;
    }

    cmd = QString( "SELECT Number,X,Y,Angle FROM ModelPositions WHERE Code='%1'" ).arg( pModel->Code() );
    if ( query.exec( cmd ) )
    {
        while ( query.next() )
        {
            HxPosition pos;
            pos.index = query.value( 0 ).toInt();
            pos.x = query.value( 1 ).toDouble();
            pos.y = query.value( 2 ).toDouble();
            pos.angle = query.value( 3 ).toInt();
            pModel->SetPosition( pos.index, pos );
        }
    }

    cmd = QString( "SELECT Key,Value FROM ModelParams WHERE Code='%1'" ).arg( pModel->Code() );
    if ( query.exec( cmd ) )
    {
        while ( query.next() )
        {
            QString key = query.value( 0 ).toString();
            QString value = query.value( 1 ).toString();
            pModel->SetValue( key, value );
        }
    }

    pModel->ClearModified();
    return pModel;
}

ReturnCode HxModelManager::Save( HxModelPtr pModel )
{
    if ( !pModel )
        return RtDataNull;

    if ( !pModel->IsMofified() )
        return RtDataNoChanges;

    QString dbFilePath = m_settings.String( DatabaseFilePath );
    if ( !HxDatabase::CheckDatabaseFileExisting( dbFilePath ) )
        return RtDBFileNotFound;

    HxDatabase db = HxDatabase::database( "SQLITE" );
    db.setDatabaseName( dbFilePath );
    if ( !db.open() )
    {
        qDebug() << "Save Model: " << db.lastError().text();
        return RtDBOpenFailed;
    }

    HxQuery query( db );

    if ( pModel->IsMofified( HxModel::eNew ) )
    {
        QString cmd = QString( "INSERT INTO Models "
                               "(Code,Name,Design,kNo,IV,CvWidth,Stopper,IsPrintLo) "
                               "VALUES('%1','%2','%3','%4','%5','%6','%7','%8'); " )
            .arg( pModel->Code() )
            .arg( pModel->Name() )
            .arg( pModel->Design() )
            .arg( pModel->kNo() )
            .arg( pModel->IVProgram() )
            .arg( pModel->CvWidth() )
            .arg( pModel->Stopper() )
            .arg( pModel->IsPrintLo() );
        if ( !query.exec( cmd ) )
        {
            qDebug() << "Save Model: " << query.lastError().text();
            db.close();
            return RtDBQueryFailed;
        }
    }
    else if ( pModel->IsMofified( HxModel::eInfo ) )
    {
        QString cmd = QString( "UPDATE Models "
                               "SET Name = '%1',Design = '%2',kNo = '%3',IV = '%4',CvWidth = '%5',Stopper = '%6',IsPrintLo = '%7' "
                               "WHERE Code = '%8'" )
            .arg( pModel->Name() )
            .arg( pModel->Design() )
            .arg( pModel->kNo() )
            .arg( pModel->IVProgram() )
            .arg( pModel->CvWidth() )
            .arg( pModel->Stopper() )
            .arg( pModel->IsPrintLo() )
            .arg( pModel->Code() );
        if ( !query.exec( cmd ) )
        {
            qDebug() << "Save Model: " << query.lastError().text();
            db.close();
            return RtDBQueryFailed;
        }
    }

    if ( pModel->IsMofified( HxModel::ePosition ) )
    {
        QString cmd = QString( "DELETE FROM ModelPositions WHERE Code = '%1'" ).arg( pModel->Code() );
        query.exec( cmd );

        auto &positions = pModel->Positions();
        for ( auto &[index, position] : positions )
        {
            cmd = QString( "INSERT INTO ModelPositions "
                           "(Code,Number,X,Y,Angle) "
                           "VALUES('%1','%2','%3','%4','%5')" )
                .arg( pModel->Code() )
                .arg( index )
                .arg( position.x )
                .arg( position.y )
                .arg( position.angle );
            if ( !query.exec( cmd ) )
            {
                qDebug() << "Save Model: " << query.lastError().text();
                db.close();
                return RtDBQueryFailed;
            }
        }
    }

    if ( pModel->IsMofified( HxModel::eComment ) )
    {
        QString cmd = QString( "DELETE FROM ModelParams WHERE Code = '%1'" ).arg( pModel->Code() );
        query.exec( cmd );

        auto &comments = pModel->Comments();
        for ( auto &[key, value] : comments )
        {
            cmd = QString( "INSERT INTO ModelParams "
                           "(Code,Key,Value) "
                           "VALUES('%1','%2','%3')" )
                .arg( pModel->Code() )
                .arg( key )
                .arg( value );
            if ( !query.exec( cmd ) )
            {
                qDebug() << "Save Model: " << query.lastError().text();
                db.close();
                return RtDBQueryFailed;
            }
        }
    }
    pModel->ClearModified();
    db.close();
    return RtNormal;
}

ReturnCode HxModelManager::Removes( const QStringList& names )
{
    QString dbFilePath = m_settings.String( DatabaseFilePath );
    if ( !HxDatabase::CheckDatabaseFileExisting( dbFilePath ) )
        return RtDBFileNotFound;

    HxDatabase db = HxDatabase::database( "SQLITE" );
    db.setDatabaseName( dbFilePath );
    if ( !db.open() )
        return RtDBOpenFailed;

    HxQuery query( db );

    for ( auto& name : names )
    {
        QString cmd = QString( "DELETE FROM Models WHERE Code = '%1'" ).arg( name );
        query.exec( cmd );

        cmd = QString( "DELETE FROM ModelPositions WHERE Code = '%1'" ).arg( name );
        query.exec( cmd );

        cmd = QString( "DELETE FROM ModelParams WHERE Code = '%1'" ).arg( name );
        query.exec( cmd );
    }
    db.close();
    qApp->postEvent( qApp, new HxEvent( HxEvent::eModelDeleted ) );
    return RtNormal;
}

void HxModelManager::AddComments( const QStringList& keys )
{

}

void HxModelManager::RemoveComments( const QStringList& keys )
{

}
//
//void HxModelManager::Migration( const QString& dir )
//{
//    QString lotDir = dir;
//    QDir().mkdir( lotDir );
//    QFileInfoList fileInfos = QDir( lotDir ).entryInfoList( { "*.model" } );
//
//    std::set<QString> paramNames;
//
//    for ( auto& fileInfo : fileInfos )
//    {
//        QString designName = fileInfo.baseName().toUpper();
//        QString filePath = dir + "/" + designName + ".model";
//
//        QFileInfo fileInfo( filePath );
//        QFile fileReader( filePath );
//        if ( !fileReader.open( QIODevice::ReadOnly ) )
//            continue;
//
//        QByteArray json = fileReader.readAll();
//        fileReader.close();
//
//        QJsonDocument doc = QJsonDocument::fromJson( json );
//        QJsonObject obj = doc.object();
//
//        HxModelPtr pModel = Create();
//
//        pModel->SetCode( obj.value( "code" ).toString() );
//        pModel->SetName( designName );
//        pModel->SetPrintLo( obj.value( "is-print-lo" ).toBool() );
//        pModel->SetkNo( obj.value( "k-no" ).toString() );
//        pModel->SetIVProgram( obj.value( "iv-program" ).toString() );
//        pModel->SetDesign( obj.value( "design" ).toString() );
//        pModel->SetCvWidth( obj.value( "cv-width" ).toDouble() );
//        pModel->SetStopper( obj.value( "stopper" ).toInt( 1 ) );
//
//        // markPositions;
//        QJsonArray posArr = obj.value( "pos" ).toArray();
//        int posIndex = 1;
//        for ( auto posItem : posArr )
//        {
//            QJsonObject objPos = posItem.toObject();
//            HxPosition pos;
//            pos.index = objPos.value( "index" ).toInt();
//            if ( pos.index <= 0 )
//                pos.index = posIndex;
//            pos.x = objPos.value( "x" ).toDouble( 0 );
//            pos.y = objPos.value( "y" ).toDouble( 0 );
//            pos.angle = objPos.value( "angle" ).toDouble( 0 );
//
//            pModel->SetPosition( pos.index, pos );
//            posIndex++;
//        }
//
//        // comments;
//        QJsonObject commentObj = obj.value( "comments" ).toObject();
//        QStringList keys = commentObj.keys();
//        for ( auto& key : keys )
//        {
//            if ( key.startsWith( "LBLDIR" ) ) continue;
//            if ( key.startsWith( "LBLPOSX" ) ) continue;
//            if ( key.startsWith( "LBLPOSY" ) ) continue;
//            if ( key.startsWith( "LBLSIZE" ) ) continue;
//            if ( key.startsWith( "LBLTYPE" ) ) continue;
//            pModel->SetComment( key, commentObj.value( key ).toString() );
//        }
//
//        Save( pModel );
//    }
//
//    QString cmdFormat = "";
//
//    for ( auto& paramName : paramNames )
//    {
//
//    }
//}

ReturnCode HxModelManager::DeleteAll()
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

    cmd = QString( "DELETE FROM Models" );
    if ( !query.exec( cmd ) )
    {
        db.close();
        return RtDBQueryFailed;
    }

    cmd = QString( "DELETE FROM ModelParams" );
    if ( !query.exec( cmd ) )
    {
        db.close();
        return RtDBQueryFailed;
    }

    cmd = QString( "DELETE FROM ModelPositions" );
    if ( !query.exec( cmd ) )
    {
        db.close();
        return RtDBQueryFailed;
    }

    db.close();
    return RtNormal;
}

void HxModelManager::ReloadSetting()
{
    m_settings.Load();
}

HxModelManager* ModelManager()
{
    static HxModelManager instance;
    return &instance;
}
