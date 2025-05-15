﻿#include "HxLogger.h"
#include "HxModel.h"
#include "HxLOT.h"
#include "HxDesign.h"
#include "HxSettings.h"
#include <QDir>
#include <QCoreApplication>

#include "QStandardPaths"
#include "QFileDialog"

#include "HxDataGenerator.h"
#include "HxDatabase.h"
#include "HxMessage.h"

HxLogger::HxLogger()
{
    m_settings.Load();
}

ReturnCode HxLogger::CheckSerialExisting( const QString& serial )
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

    QString cmd = QString( "SELECT 1 FROM PrintLogs WHERE Serial='%1' LIMIT 1" ).arg( serial );
    HxQuery query( db );
    if ( !query.exec( cmd ) )
    {
        db.close();
        return RtDBQueryFailed;
    }
    bool exist = query.next();
    db.close();

    return exist ? RtDBDataExisting : RtNormal;
}

ReturnCode HxLogger::SavePrint( HxLOTPtr pLOT, HxModelPtr pModel, HxDesignPtr pDesign, std::map<int, QString>& blockdata )
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
    
    bool bIsRePrint = ( pModel->IsPrintLo() ) || ( pLOT->IsRePrint() );
    QString tableName = bIsRePrint ? "RePrintLogs" : "PrintLogs";
    
    QDateTime time = QDateTime::currentDateTime();
    int codeBockIndex = pDesign->IndexOfBlockCode();
    QString code = blockdata[ codeBockIndex ];
    if ( code.isEmpty() )
    {
        code = QString( "ATLCODE-%1" ).arg( time.toString( "HHmmsszzz" ) );
    }

    QStringList itemData;
    for ( auto& [index, data] : blockdata )
        itemData.push_back( data );

    QString cmd = QString( "INSERT INTO %1 "
                           "(Time,Serial,LOT,Model,Data) "
                           "VALUES('%2','%3','%4','%5','%6')" )
        .arg( tableName )
        .arg( time.toString( "yyyy-MM-dd HH:mm:ss.zzz" ) )
        .arg( code )
        .arg( pLOT->Name() )
        .arg( pModel->Name() )
        .arg( itemData.join( "," ) );

    HxQuery query( db );
    if ( !query.exec( cmd ) )
    {
        db.close();
        QString errorMsg = query.lastError().text();
        qDebug() << errorMsg;
        if ( errorMsg.contains( "UNIQUE" ) )
            return RtDBDataExisting;
        return RtDBInsertFailed;
    }

    db.close();
    return RtNormal;
}

ReturnCode HxLogger::SaveError( const QString& time, const QString& where, const QString& message )
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

    QString cmd = QString( "INSERT INTO Exceptions "
                           "(Time,Object,Exception) "
                           "VALUES('%1','%2','%3')" )
        .arg( time )
        .arg( where )
        .arg( message );
    HxQuery query( db );
    if ( !query.exec( cmd ) )
    {
        db.close();
        return RtDBInsertFailed;
    }
    db.close();
    return RtNormal;
}

ReturnCode HxLogger::SaveBarcode( const QString& code )
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

    QString cmd = QString( "INSERT INTO BarcodeLogs "
                           "(Time,Data) "
                           "VALUES('%1','%2')" )
        .arg( QDateTime::currentDateTime().toString( "yyyy-MM-dd HH:mm:ss.zzz" ) )
        .arg( code );
    HxQuery query( db );
    if ( !query.exec( cmd ) )
    {
        db.close();
        return RtDBInsertFailed;
    }
    db.close();
    return RtNormal;
}

ReturnCode HxLogger::Get( QDate fromDate, QDate toDate, HxLogArray& items )
{
    items.clear();
    QString dbFilePath = m_settings.String( DatabaseFilePath );
    if ( !HxDatabase::CheckDatabaseFileExisting( dbFilePath ) )
        return RtDBFileNotFound;
    HxDatabase db = HxDatabase::database( "SQLITE" );
    db.setDatabaseName( dbFilePath );
    if ( !db.open() )
    {
        return RtDBOpenFailed;
    }

    QString from = fromDate.toString( "yyyy-MM-dd 00:00:00.000" );
    QString to = toDate.toString( "yyyy-MM-dd 23:59:59.999" );
    QString cmd = QString( "SELECT Time,Serial,LOT,Model,Data "
                           "FROM PrintLogs "
                           "WHERE Time>='%1' AND Time<='%2'" )
        .arg( from )
        .arg( to );

    HxQuery query( db );
    if ( !query.exec( cmd ) )
    {
        db.close();
        return RtDBQueryFailed;
    }

    while ( query.next() )
    {
        HxLog log;
        log.Time = query.value( 0 ).toString();
        log.Serial = query.value( 1 ).toString();
        log.LOT = query.value( 2 ).toString();
        log.Model = query.value( 3 ).toString();
        log.items = query.value( 4 ).toString().split( "," );
        items.push_back( log );
    }
    db.close();
    return RtNormal;
}

void HxLogger::Export( HxLogArray& items, QDate fromDate, QDate toDate )
{
    QString tempFile;
    if ( fromDate == toDate )
        tempFile = QString( "%1.csv" ).arg( fromDate.toString( "yyyy-MM-dd" ) );
    else
        tempFile = QString( "%1_%2.csv" )
        .arg( fromDate.toString( "yyyy-MM-dd" ) )
        .arg( toDate.toString( "yyyy-MM-dd" ) );
    QStringList csvLineData;
    csvLineData.push_back( "\"TIME\",\"SERIAL\",\"LOT\",\"MODEL\",\"DATA1\",\"DATA2\",\"DATA3\",\"DATA4\",\"DATA5\",\"DATA6\",\"DATA7\",\"DATA8\",\"DATA9\",\"DATA10\"," );
    for ( auto& log : items )
    {
        QString lineData;
        lineData += QString( "\"%1\"," ).arg( log.Time );
        lineData += QString( "\"%1\"," ).arg( log.Serial );
        lineData += QString( "\"%1\"," ).arg( log.LOT );
        lineData += QString( "\"%1\"," ).arg( log.Model );
        for ( auto& it : log.items )
            lineData += QString( "\"%1\"," ).arg( it );

        csvLineData.push_back( lineData );
    }

    QString csvData = csvLineData.join( "\n" );

    static QString oldDir = QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation );
    QString filePath = QFileDialog::getSaveFileName( nullptr, "Chọn thư mục dữ liệu", oldDir + "/" + tempFile, "Csv Files (*.csv);;All Files (*)");
    QFile fileWriter( filePath );
    if ( fileWriter.open( QIODevice::WriteOnly | QIODevice::Text ) )
    {
        fileWriter.write( csvData.toUtf8() );
        fileWriter.close();
        HxMsgInfo( tr( "Đã lưu csv" ), tr( "Lưu log" ) );
        oldDir = QFileInfo( filePath ).absoluteDir().absolutePath();
    }
    else
    {
        HxMsgError( tr( "Không thể mở file. Kiểm tra lại đường dẫn!" ) );
    }
}

void HxLogger::ReLoadSetting()
{
    m_settings.Load();
}

HxLogger* Logger()
{
    static HxLogger instance;
    return &instance;
}