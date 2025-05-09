#include "HxDesign.h"
#include "QDir"
#include "QFile"
#include "QFileInfo"
#include "QJsonObject"
#include "QJsonArray"
#include "QJsonDocument"
#include "QCoreApplication"

#include "HxFileManager.h"
#include "HxDatabase.h"
#include "HxEvent.h"

HxDesign::HxDesign()
{

}

HxDesign::~HxDesign()
{

}

QString HxDesign::Name() const
{
    return m_name;
}

double HxDesign::Width() const
{
    return m_width;
}

double HxDesign::Height() const
{
    return m_height;
}

HxBlock HxDesign::Block( int index )
{
    return m_blocks[ index ];
}

std::map<int, HxBlock> HxDesign::Blocks()
{
    return m_blocks;
}

int HxDesign::IndexOfBlockCode()
{
    if ( m_blocks.empty() )
        return -1;

    int rindex = 1;
    for ( auto& [index, block] : m_blocks )
    {
        if ( block.isCode )
        {
            rindex = index;
            break;
        }
    }
    return rindex;
}


void HxDesign::SetName( const QString& name )
{
    Modify( m_name, name, eNew );
}

void HxDesign::SetWidth( double value )
{
    Modify( m_width, value, eSize );
}

void HxDesign::SetHeight( double value )
{
    Modify( m_height, value, eSize );
}

void HxDesign::SetBlock( int index, const HxBlock& block )
{
    auto it = m_blocks.find( index );
    if ( it == m_blocks.end() || it->second.data != block.data || it->second.isCode != block.isCode || it->second.textLen != block.textLen )
    {
        m_blocks[ index ] = block;
        SetModified( eBlock );
    }
}

HxDesignManager::HxDesignManager() :QObject( nullptr )
{
    //qApp->installEventFilter( this );
}

HxDesignPtr HxDesignManager::Create()
{
    return std::make_shared<HxDesign>();
}

HxDesignPtr HxDesignManager::GetDesign( const QString& name )
{
    QString dbFilePath = m_settings.String( DatabaseFilePath );
    HxDatabase db = HxDatabase::database( "SQLITE" );
    db.setDatabaseName( dbFilePath );
    if ( !db.open() )
        return nullptr;
    HxQuery query( db );

    QString cmd = QString( "SELECT Name,Width,Height "
                           "FROM Designs "
                           "WHERE Name='%1'" )
        .arg( name );

    if ( !query.exec( cmd ) )
        return nullptr;

    HxDesignPtr pDesign;
    while ( query.next() )
    {
        if ( !pDesign )
        {
            pDesign = Create();
            pDesign->SetName( query.value( 0 ).toString() );
            pDesign->SetWidth( query.value( 1 ).toDouble() );
            pDesign->SetHeight( query.value( 2 ).toDouble() );
        }
    }

    if ( pDesign )
    {
        cmd = QString( "SELECT Number,Format,Length,IsCode "
                       "FROM DesignBlocks "
                       "WHERE Name='%1'" )
            .arg( name );
        if ( query.exec( cmd ) )
        {
            while ( query.next() )
            {
                int index = query.value( 0 ).toInt();
                if ( index < 1 )
                    continue;
                HxBlock block;
                block.data = query.value( 1 ).toString();
                block.textLen = query.value( 2 ).toInt();
                block.isCode = query.value( 3 ).toInt() > 0;
                pDesign->SetBlock( index, block );
            }
        }
        pDesign->ClearModified();
    }

    db.close();

    return pDesign;
}

HxDesignPtrMap HxDesignManager::GetDesigns()
{
    HxDesignPtrMap map;
    QString dbFilePath = m_settings.String( DatabaseFilePath );
    HxDatabase db = HxDatabase::database( "SQLITE" );
    db.setDatabaseName( dbFilePath );
    if ( !db.open() )
        return map;
    HxQuery query( db );

    QString cmd = QString( "SELECT Name,Width,Height FROM Designs" );

    if ( !query.exec( cmd ) )
        return map;

    while ( query.next() )
    {
        HxDesignPtr pDesign = Create();
        pDesign->SetName( query.value( 0 ).toString() );
        pDesign->SetWidth( query.value( 1 ).toDouble() );
        pDesign->SetHeight( query.value( 2 ).toDouble() );
        pDesign->ClearModified();
        map[ pDesign->Name() ] = pDesign;
    }
    db.close();

    return map;
}

void HxDesignManager::Save( HxDesignPtr pDesign )
{
    if ( !pDesign )
        return;
    QString dbFilePath = m_settings.String( DatabaseFilePath );
    HxDatabase db = HxDatabase::database( "SQLITE" );
    db.setDatabaseName( dbFilePath );
    if ( !db.open() )
        return;
    HxQuery query( db );
    QString cmd;
    if ( pDesign->IsMofified( HxDesign::eNew ) )
    {
        cmd = QString( "INSERT INTO Designs "
                       "(Name,Width,Height) "
                       "VALUES('%1',%2,%3)" )
            .arg( pDesign->Name() )
            .arg( pDesign->Width() )
            .arg( pDesign->Height() );
        if ( !query.exec( cmd ) )
        {
            qDebug() << query.lastError().text();
        }
    }
    else
    {
        cmd = QString( "UPDATE Designs "
                       "SET Width = '%2',Height = '%3' "
                       "WHERE Name = '%1'" )
            .arg( pDesign->Name() )
            .arg( pDesign->Width() )
            .arg( pDesign->Height() );
        query.exec( cmd );
    }

    if ( pDesign->IsMofified( HxDesign::eNew | HxDesign::eBlock ) )
    {
        cmd = QString( "DELETE FROM DesignBlocks WHERE Name = '%1'" ).arg( pDesign->Name() );
        query.exec( cmd );

        auto blocks = pDesign->Blocks();
        for ( auto& [index, block] : blocks )
        {
            cmd = QString( "INSERT INTO DesignBlocks "
                           "(Name,Number,Format,Length,IsCode) "
                           "VALUES('%1',%2,'%3',%4,%5);" )
                .arg( pDesign->Name() )
                .arg( index )
                .arg( block.data )
                .arg( block.textLen )
                .arg( block.isCode );
            if ( !query.exec( cmd ) )
            {
                qDebug() << query.lastError().text() << " - " << cmd;
            }
        }
    }

    db.close();
    bool isNew = pDesign->IsMofified( HxDesign::eNew );
    pDesign->ClearModified();
    qApp->postEvent( qApp, new HxEvent( isNew ? HxEvent::eDesignAdded : HxEvent::eDesignChanged ) );
}

bool HxDesignManager::eventFilter( QObject* watched, QEvent* event )
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

void HxDesignManager::Migration( const QString& dir )
{
    for ( int i = 0; i < 2000; i++ )
    {
        QString name = QString::number( i ).rightJustified( 4, '0' );
        QString designPath = dir + "/" + name + ".design";
        QFile fileReader( designPath );
        if ( !fileReader.open( QIODevice::ReadOnly ) )
            continue;

        QByteArray json = fileReader.readAll();
        fileReader.close();
        QJsonDocument doc = QJsonDocument::fromJson( json );
        QJsonObject obj = doc.object();

        HxDesignPtr pDesign = Create();
        pDesign->SetName( name );
        pDesign->SetWidth( obj.value( "width" ).toDouble( 5 ) );
        pDesign->SetHeight( obj.value( "height" ).toDouble( 5 ) );

        QJsonArray arr = obj.value( "blocks" ).toArray();
        for ( auto arrItem : arr )
        {
            QJsonObject objBlock = arrItem.toObject();
            int index = objBlock.value( "index" ).toInt( 0 );
            if ( index < 1 )
                continue;

            HxBlock block;
            block.isCode = objBlock.value( "is-code" ).toBool( false );
            block.data = objBlock.value( "data" ).toString();
            block.textLen = objBlock.value( "text-length" ).toInt( 1 );
            pDesign->SetBlock( index, block );
        }

        Save( pDesign );
    }
}

HxDesignManager* DesignManager()
{
    static HxDesignManager instance;
    return &instance;
}