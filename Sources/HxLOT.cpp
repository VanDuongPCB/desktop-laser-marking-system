#include "HxLOT.h"
#include "HxSettings.h"
#include "HxConvert.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QCoreApplication>

HxLOT::HxLOT()
{
    comments.insert( "FIX1", "" );
    comments.insert( "FIX2", "" );
    comments.insert( "FIX3", "" );
    comments.insert( "FIX4", "" );
    comments.insert( "FIX5", "" );
}

HxLOT::~HxLOT()
{

}

QString HxLOT::Counter()
{
    int len = counterStart.length();
    int start = counterStart.toInt( 0 );
    int cnt = start + progress;
    return QString::number( cnt ).rightJustified( len, '0' );
}

QString HxLOT::MAC()
{
    uint64_t start = HexStringToUInt64( macStart );
    uint64_t cnt = start + progress;
    return UInt64ToHexString( cnt, 12 );
}

QString HxLOT::Status()
{
    if ( progress <= 0 )
    {
        return "Chưa sản xuất";
    }
    else if ( progress < quantity )
    {
        return "Đang sản xuất";
    }
    else
    {
        return "Đã hoàn thành";
    }
}

bool HxLOT::NextItem()
{
    progress++;
    return progress < quantity;
}

bool HxLOT::IsCompleted()
{
    return progress >= quantity;
}

QString HxLOT::Value( QString paramName )
{
    if ( paramName == "NAME" ) return name.split( "-RE" ).first();
    else if ( paramName == "COUNTER" ) return Counter();
    else if ( paramName == "MAC" ) return MAC();
    else if ( comments.contains( paramName ) )
    {
        return comments.value( paramName );
    }
    else
    {
        return "LOT." + paramName;
    }
}

/* ------------------------------------------------------------- */

std::vector<std::shared_ptr<HxLOT>> HxLOT::items;

void HxLOT::Sort()
{
    for ( auto& it : items )
    {
        it->sIndex = 0;
        if ( it->Status() == "Đang sản xuất" ) it->sIndex += 1000000;
        else if ( it->Status() == "Chưa sản xuất" ) it->sIndex += 2000000;
        else if ( it->Status() == "Đã hoàn thành" ) it->sIndex += 3000000;
    }

    std::sort( items.begin(), items.end(), []( std::shared_ptr<HxLOT>& it1, std::shared_ptr<HxLOT>& it2 )
               {
                   return it1->sIndex < it2->sIndex;
               } );
}

void HxLOT::Load()
{
    items.clear();
    QDir().mkdir( QCoreApplication::applicationDirPath() + "/data" );
    QString dir = QCoreApplication::applicationDirPath() + "/data/LOTS";
    QDir().mkdir( dir );
    QFileInfoList files = QDir( dir ).entryInfoList( { "*.lot" } );
    for ( auto& file : files )
    {

        QDateTime lastWrite = file.lastModified();

        QFile fileReader( file.absoluteFilePath() );
        if ( fileReader.open( QIODevice::ReadOnly ) )
        {
            QByteArray json = fileReader.readAll();
            fileReader.close();

            QJsonDocument doc = QJsonDocument::fromJson( json );
            QJsonObject obj = doc.object();

            HxLOT temp;
            temp.name = file.baseName().toUpper();
            temp.dateCreate = obj.value( "time-create" ).toString();
            temp.macStart = obj.value( "mac-start" ).toString();
            temp.macEnd = obj.value( "mac-end" ).toString();
            temp.counterStart = obj.value( "counter-start" ).toString();
            temp.quantity = obj.value( "quantity" ).toInt();
            temp.progress = obj.value( "progress" ).toInt( 0 );
            temp.modelName = obj.value( "model" ).toString();
            temp.isRePrint = obj.value( "re-print" ).toBool( false );
            QJsonObject cmtObj = obj.value( "comments" ).toObject();
            QStringList keys = cmtObj.keys();
            for ( auto& key : keys )
            {
                temp.comments.insert( key, cmtObj.value( key ).toString() );
            }

            int days = lastWrite.daysTo( QDateTime::currentDateTime() );
            if ( temp.progress >= temp.quantity && days >= 45 )
            {
                QFile( file.absoluteFilePath() ).remove();
            }
            else
            {
                HxLOT* lot = new HxLOT();
                lot[ 0 ] = temp;
                items.emplace_back( lot );
            }
        }
    }
    Sort();
}

void HxLOT::SaveLot( std::shared_ptr<HxLOT> data )
{
    QJsonObject obj;
    obj.insert( "time-create", data->dateCreate );
    obj.insert( "mac-start", data->macStart );
    obj.insert( "mac-end", data->macEnd );
    obj.insert( "counter-start", data->counterStart );
    obj.insert( "quantity", data->quantity );
    obj.insert( "progress", data->progress );
    obj.insert( "model", data->modelName );
    obj.insert( "re-print", data->isRePrint );

    QJsonObject commObj;
    QStringList keys = data->comments.keys();
    for ( auto& key : keys )
    {
        commObj.insert( key, data->comments.value( key ) );
    }

    obj.insert( "comments", commObj );

    QJsonDocument doc;
    doc.setObject( obj );

    QDir().mkdir( QCoreApplication::applicationDirPath() + "/data" );
    QString dir = QCoreApplication::applicationDirPath() + "/data/LOTS";
    QDir().mkdir( dir );
    QString filePath = dir + "/" + data->name + ".lot";
    QFile fileWriter( filePath );
    if ( fileWriter.open( QIODevice::WriteOnly ) )
    {
        fileWriter.write( doc.toJson() );
        fileWriter.close();
    }
}

void HxLOT::Remove( int index )
{
    if ( index < 0 || index >= items.size() ) return;
    // delete
    QDir().mkdir( QCoreApplication::applicationDirPath() + "/data" );
    QString dir = QCoreApplication::applicationDirPath() + "/data/LOTS";
    QDir().mkdir( dir );
    QString filePath = dir + "/" + items[ index ]->name + ".lot";
    QFile( filePath ).remove();

    // remove from list
    items.erase( items.begin() + index );
}

void HxLOT::SaveAll()
{
    for ( auto& item : items )
    {
        SaveLot( item );
    }
}

std::shared_ptr<HxLOT> HxLOT::Create()
{
    auto lot = std::make_shared<HxLOT>( HxLOT() );
    items.push_back( lot );
    return lot;
}

std::shared_ptr<HxLOT> HxLOT::Find( QString name )
{
    for ( auto& item : items )
    {
        if ( item->name == name ) return item;
    }
    return {};
}

QStringList HxLOT::ParamNames()
{
    QStringList names = { "NAME","COUNTER","FIX1","FIX2","FIX3","FIX4","FIX5" };
    for ( auto& it : items )
    {
        QStringList keys = it->comments.keys();
        for ( auto& key : keys )
        {
            if ( names.contains( key ) == false )
            {
                names.push_back( key );
            }
        }
    }

    return names;
}
