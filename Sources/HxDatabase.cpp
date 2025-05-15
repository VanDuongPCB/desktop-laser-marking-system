#include "HxDatabase.h"
#include "QFile"
#include "QObject"
#include "QJsonObject"
#include "QApplication"

#include "HxMessage.h"
#include "HxEvent.h"

HxDatabase::HxDatabase() : QSqlDatabase() 
{}

HxDatabase::HxDatabase( const QSqlDatabase& other ): QSqlDatabase(other)
{ }

HxDatabase::HxDatabase( const QString& fileDB ) : QSqlDatabase()
{
    setDatabaseName( fileDB );
}

bool HxDatabase::CheckDatabaseFileExisting( const QString& file )
{
    bool exist = QFile::exists( file );
    if ( !exist )
    {
        HxMsgError( QObject::tr( "Không tìm thấy file cơ sở dữ liệu!\n"
                                 "%1\n"
                                 "Hãy kiểm tra cài đặt hoặc thực hiện migrate!" )
                    .arg( file ),
                    QObject::tr( "Lỗi cơ sở dữ liệu" ) );
        QJsonObject obj;
        obj.insert( "time", QDateTime::currentDateTime().toString( "yyyy-MM-dd hh:mm:ss" ) );
        obj.insert( "where", "Database" );
        obj.insert( "message", QString( "Không tìm thấy database: %1" ).arg( file ) );
        qApp->postEvent( qApp, new HxEvent( HxEvent::eDatabaseError, obj ) );
    }
    return exist;
}