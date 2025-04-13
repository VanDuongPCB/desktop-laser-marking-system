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
#include "HxDebugger.h"

namespace
{
HxLOTManager s_instance;
}



HxLOT::HxLOT()
{
    m_comments.insert( "FIX1", "" );
    m_comments.insert( "FIX2", "" );
    m_comments.insert( "FIX3", "" );
    m_comments.insert( "FIX4", "" );
    m_comments.insert( "FIX5", "" );
}

HxLOT::~HxLOT()
{

}

// QString HxLOT::counter()
// {
//     int len = m_counterStart.length();
//     int start = m_counterStart.toInt( 0 );
//     int cnt = start + m_progress;
//     return QString::number( cnt ).rightJustified( len, '0' );
// }

// QString HxLOT::mac()
// {
//     uint64_t start = Uint64FromHexString( m_macStart );
//     uint64_t cnt = start + m_progress;
//     return HexStringFromUint64( cnt, 12 );
// }

// QString HxLOT::status()
// {
//     if ( m_progress <= 0 )
//     {
//         return "Chưa sản xuất";
//     }
//     else if ( m_progress < m_quantity )
//     {
//         return "Đang sản xuất";
//     }
//     else
//     {
//         return "Đã hoàn thành";
//     }
// }

bool HxLOT::nextItem()
{
    m_progress++;
    return m_progress < m_quantity;
}

bool HxLOT::isCompleted()
{
    Evaluate();
    return m_progress >= m_quantity;
}

QString HxLOT::Name()
{
    return m_name;
}

QString HxLOT::CounterStart()
{
    return m_counterStart;
}

QString HxLOT::CounterEnd()
{
    int len = m_counterStart.length();
    int start = m_counterStart.toInt( 0 );
    int cnt = start + m_quantity - 1;
    return QString::number( cnt ).rightJustified( len, '0' );
}

QString HxLOT::Counter()
{
    int len = m_counterStart.length();
    int start = m_counterStart.toInt( 0 );
    int cnt = start + m_progress;
    return QString::number( cnt ).rightJustified( len, '0' );
}

QString HxLOT::MACStart()
{
    return m_macStart;
}

QString HxLOT::MACEnd()
{
    return m_macEnd;
}

QString HxLOT::MAC()
{
    uint64_t start = Uint64FromHexString( m_macStart );
    uint64_t cnt = start + m_progress;
    return HexStringFromUint64( cnt, 12 );
}

int HxLOT::Quantity()
{
    return m_quantity;
}

int HxLOT::Progress()
{
    return m_progress;
}

QString HxLOT::Model()
{
    return m_modelName;
}

QString HxLOT::Value( QString paramName )
{
    if ( paramName == "NAME" )
        return m_name.split( "-RE" ).first();
    else if ( paramName == "COUNTER" )
        return Counter();
    else if ( paramName == "MAC" )
        return MAC();
    else if ( m_comments.contains( paramName ) )
    {
        return m_comments.value( paramName );
    }
    else
    {
        return "LOT." + paramName;
    }
}

HxLOT::ProductStatus HxLOT::Status()
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

void HxLOT::SetName(const QString& value)
{
    m_name = value;
}

void HxLOT::SetCounterStart(const QString& value)
{
    m_counterStart = value;
}

void HxLOT::SetMACStart(const QString& value)
{
    m_macStart = value;
}

void HxLOT::SetMACEnd(const QString& value)
{
    m_macEnd = value;
}

void HxLOT::SetQuantity(int value)
{
    m_quantity = value;
}

void HxLOT::SetProgress(int value)
{
    m_progress = value;
}

void HxLOT::SetModel(const QString& value)
{
    m_modelName = value;
}

void HxLOT::SetValue( const QString& name, const QString& value )
{
    m_comments[name] = value;
}



HxLOTPtr HxLOT::Clone()
{
    auto pClone = GetLOTManager()->Create();
    *pClone.get() = *this;
    return pClone;
}

/* ------------------------------------------------------------- */

// std::vector<std::shared_ptr<HxLOT>> HxLOT::items;

// void HxLOT::load()
// {
//     items.clear();

//     QString lotDir = GetFileManager()->GetPath(HxFileManager::eDBLOTDir);
//     QDir().mkdir( lotDir );
//     QFileInfoList files = QDir( lotDir ).entryInfoList( { "*.lot" } );
//     for ( auto& file : files )
//     {
//         QDateTime lastWrite = file.lastModified();

//         QFile fileReader( file.absoluteFilePath() );
//         if ( fileReader.open( QIODevice::ReadOnly ) )
//         {
//             QByteArray json = fileReader.readAll();
//             fileReader.close();

//             QJsonDocument doc = QJsonDocument::fromJson( json );
//             QJsonObject obj = doc.object();

//             HxLOT temp;
//             temp.name = file.baseName().toUpper();
//             temp.dateCreate = obj.value( "time-create" ).toString();
//             temp.macStart = obj.value( "mac-start" ).toString();
//             temp.macEnd = obj.value( "mac-end" ).toString();
//             temp.counterStart = obj.value( "counter-start" ).toString();
//             temp.quantity = obj.value( "quantity" ).toInt();
//             temp.progress = obj.value( "progress" ).toInt( 0 );
//             temp.modelName = obj.value( "model" ).toString();
//             temp.isRePrint = obj.value( "re-print" ).toBool( false );
//             QJsonObject cmtObj = obj.value( "comments" ).toObject();
//             QStringList keys = cmtObj.keys();
//             for ( auto& key : keys )
//             {
//                 temp.comments.insert( key, cmtObj.value( key ).toString() );
//             }

//             int days = lastWrite.daysTo( QDateTime::currentDateTime() );
//             if ( temp.progress >= temp.quantity && days >= 45 )
//             {
//                 QFile( file.absoluteFilePath() ).remove();
//             }
//             else
//             {
//                 HxLOT* lot = new HxLOT();
//                 lot[ 0 ] = temp;
//                 items.emplace_back( lot );
//             }
//         }
//     }
//     sort();
// }

// void HxLOT::saveLot( std::shared_ptr<HxLOT> data )
// {
//     QJsonObject obj;
//     obj.insert( "mac-start", data->m_macStart );
//     obj.insert( "mac-end", data->m_macEnd );
//     obj.insert( "counter-start", data->m_counterStart );
//     obj.insert( "quantity", data->m_quantity );
//     obj.insert( "progress", data->m_progress );
//     obj.insert( "model", data->m_modelName );
//     obj.insert( "re-print", data->m_isRePrint );

//     QJsonObject commObj;
//     QStringList keys = data->m_comments.keys();
//     for ( auto& key : keys )
//     {
//         commObj.insert( key, data->m_comments.value( key ) );
//     }

//     obj.insert( "comments", commObj );

//     QJsonDocument doc;
//     doc.setObject( obj );

//     QString dir = GetFileManager()->GetPath(HxFileManager::eDBLOTDir);
//     QDir().mkdir( dir );
//     QString filePath = dir + "/" + data->m_name + ".lot";
//     QFile fileWriter( filePath );
//     if ( fileWriter.open( QIODevice::WriteOnly ) )
//     {
//         fileWriter.write( doc.toJson() );
//         fileWriter.close();
//     }
// }

// void HxLOT::remove( int index )
// {
//     if ( index < 0 || index >= items.size() ) return;
//     // delete
//     QString dir = GetFileManager()->GetPath(HxFileManager::eDBLOTDir);
//     QDir().mkdir( dir );
//     QString filePath = dir + "/" + items[ index ]->m_name + ".lot";
//     QFile( filePath ).remove();

//     // remove from list
//     items.erase( items.begin() + index );
// }

// void HxLOT::saveAll()
// {
//     for ( auto& item : items )
//     {
//         saveLot( item );
//     }
// }

QStringList HxLOT::paramNames()
{
    QStringList names = { "NAME","COUNTER","FIX1","FIX2","FIX3","FIX4","FIX5" };
    // for ( auto& it : items )
    // {
    //     QStringList keys = it->m_comments.keys();
    //     for ( auto& key : keys )
    //     {
    //         if ( names.contains( key ) == false )
    //         {
    //             names.push_back( key );
    //         }
    //     }
    // }

    return names;
}





HxLOTPtr HxLOTManager::Create()
{
    return std::make_shared<HxLOT>();
}

HxLOTPtr HxLOTManager::GetLOT(const QString& lotName)
{
    QString dir = GetFileManager()->GetPath(HxFileManager::eDBLOTDir);
    QDir().mkdir( dir );
    QString filePath = dir + "/" + lotName + ".lot";

    HxLOTPtr pLOT;
    QFileInfo fileInfo(filePath);
    QFile fileReader(filePath);
    if(!fileReader.open(QIODevice::ReadOnly))
        return pLOT;

    QDateTime lastWrite = fileInfo.lastModified();

    QByteArray json = fileReader.readAll();
    fileReader.close();

    QJsonDocument doc = QJsonDocument::fromJson( json );
    QJsonObject obj = doc.object();

    if(!pLOT)
        pLOT = Create();

    pLOT->m_name = fileInfo.baseName().toUpper();
    pLOT->m_macStart = obj.value( "mac-start" ).toString();
    pLOT->m_macEnd = obj.value( "mac-end" ).toString();
    pLOT->m_counterStart = obj.value( "counter-start" ).toString();
    pLOT->m_quantity = obj.value( "quantity" ).toInt();
    pLOT->m_progress = obj.value( "progress" ).toInt( 0 );
    pLOT->m_modelName = obj.value( "model" ).toString();
    pLOT->m_isRePrint = obj.value( "re-print" ).toBool( false );
    QJsonObject cmtObj = obj.value( "comments" ).toObject();
    QStringList keys = cmtObj.keys();
    for ( auto& key : keys )
    {
        pLOT->m_comments[ key ] = cmtObj.value( key ).toString();
    }

    int days = lastWrite.daysTo( QDateTime::currentDateTime() );
    if ( pLOT->m_progress >= pLOT->m_quantity && days >= 45 )
    {
        // QFile( fileInfo.absoluteFilePath() ).remove();
        pLOT.reset();
    }

    pLOT->ClearModified();
    pLOT->Evaluate();
    return pLOT;
}

HxLOTPtrMap HxLOTManager::GetLOTs()
{
    HxLOTPtrMap map;
    QString lotDir = GetFileManager()->GetPath(HxFileManager::eDBLOTDir);
    QDir().mkdir( lotDir );
    QFileInfoList fileInfos = QDir( lotDir ).entryInfoList( { "*.lot" } );
    for ( auto& fileInfo : fileInfos )
    {
        QString name = fileInfo.baseName().toUpper();
        HxLOTPtr pLOT = GetLOT(name);
        if(!pLOT)
            continue;

        name = QString("%1_%2").arg(pLOT->Status()).arg(name);
        map[name] = pLOT;
    }

    return map;
}

void HxLOTManager::Save(HxLOTPtr pLOT)
{
    if(!pLOT)
        return;

    QJsonObject obj;
    obj.insert( "mac-start", pLOT->m_macStart );
    obj.insert( "mac-end", pLOT->m_macEnd );
    obj.insert( "counter-start", pLOT->m_counterStart );
    obj.insert( "quantity", pLOT->m_quantity );
    obj.insert( "progress", pLOT->m_progress );
    obj.insert( "model", pLOT->m_modelName );
    obj.insert( "re-print", pLOT->m_isRePrint );

    QJsonObject commObj;
    QStringList keys = pLOT->m_comments.keys();
    for ( auto& key : keys )
    {
        commObj.insert( key, pLOT->m_comments.value( key ) );
    }

    obj.insert( "comments", commObj );

    QJsonDocument doc;
    doc.setObject( obj );

    QString dir = GetFileManager()->GetPath(HxFileManager::eDBLOTDir);
    QDir().mkdir( dir );
    QString filePath = dir + "/" + pLOT->m_name + ".lot";
    QFile fileWriter( filePath );
    if ( fileWriter.open( QIODevice::WriteOnly ) )
    {
        fileWriter.write( doc.toJson() );
        fileWriter.close();
    }
}

void HxLOTManager::Removes(const QStringList& names)
{
    QString dir = GetFileManager()->GetPath(HxFileManager::eDBLOTDir);
    QDir().mkdir( dir );
    for(auto &name : names)
    {
        QString filePath = dir + "/" + name + ".lot";
        QFile::remove( filePath );
    }

}

QStringList HxLOTManager::Parameters()
{
    QStringList items;


    return items;
}

HxLOTManager* GetLOTManager()
{
    return &s_instance;
}
