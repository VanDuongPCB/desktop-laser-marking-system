
#include "HxBarcode.h"
#include "HxSettings.h"
#include "HxTcpSocket.h"
#include "HxMessage.h"
#include "QCoreApplication"
#include "QTime"
#include "QDir"
#include "QFile"

#include "HxFileManager.h"
#include "HxEvent.h"
#include "HxException.h"

#define GetData(command) \
QString fb;\
try\
{\
HxTcpSocket sock( m_settings.String( PLCConnIPAddress ),m_settings.Int( PLCConnPort ) );\
sock.Connect();\
sock.WriteLine( command, 5000 );\
fb = sock.ReadLine().trimmed();\
}\
catch(HxException ex)\
{\
ex.SetWhere("Barcode");\
throw ex;\
}\

HxBarcode::HxBarcode() : QObject( nullptr )
{
    m_settings.Load();
}

bool HxBarcode::IsHasData()
{
    QString cmd = "RD " +m_settings.String( RegBarcodeHasData ) + "\r";
    //HxTcpSocket sock( m_settings.String( PLCConnIPAddress ), m_settings.Int( PLCConnPort ) );
    //sock.Connect();
    //sock.WriteLine( cmd, 2000 );
    //QString fb = sock.readLine( 2000 );
    GetData( cmd );
    return fb == "1";
}

bool HxBarcode::Clear()
{
    QString cmd = "WR " + m_settings.String( RegLaserTriggerConfirm ) + " 1\r";
    //HxTcpSocket sock( m_settings.String( PLCConnIPAddress ), m_settings.Int( PLCConnPort ) );
    //sock.Connect();
    //sock.WriteLine( cmd, 2000 );
    //QString fb = sock.readLine( 2000 );
    GetData( cmd );
    return fb == "OK";
}

QString HxBarcode::Read()
{
    QString cmd = "RDS " + m_settings.String( RegBarcodeData ) + " 40\r";
    //HxTcpSocket sock( m_settings.String( PLCConnIPAddress ), m_settings.Int( PLCConnPort ) );
    //sock.Connect();
    //sock.WriteLine( cmd, 2000 );
    //QString fb = sock.readLine( 2000 );
    GetData( cmd );
    QStringList codeItems = fb.split( ' ' );
    if ( codeItems.size() < 1 ) 
        return "";
    std::vector<wchar_t> buffer( codeItems.size() + 1, 0 );
    for ( int i = 0; i < codeItems.size(); i++ )
    {
        int val = codeItems[ i ].toInt( 0 );
        buffer[ i ] = ( ( val >> 8 ) | ( val << 8 ) ) & 0xFFFF;
    }

    char* ptr = ( char* )buffer.data();

    return QString::fromStdString( ptr );
}

bool HxBarcode::SendFeedback( bool status )
{
    QString val1 = status == 1 ? "1" : "0";
    QString val2 = status == 1 ? "0" : "1";
    QString cmd = "WRS " + m_settings.String( RegBarcodeOK ) + " 2 " + val1 + " " + val2 + "\r";
    QString fb;
    try
    {
        HxTcpSocket sock( m_settings.String( PLCConnIPAddress ), m_settings.Int( PLCConnPort ) );
        sock.Connect();
        sock.WriteLine( cmd, 2000 );
        fb = sock.readLine( 2000 );
        cmd = "WR " + m_settings.String( RegBarcodeConfirm ) + " 1\r";
        sock.WriteLine( cmd, 2000 );
        fb = sock.readLine( 2000 );
    }
    catch ( HxException ex )
    {
        ex.SetWhere( "Barcode" );
        throw ex;
    }
    return fb == "OK";
}

//void HxBarcode::Save( const QString& code )
//{
//    QString dir = QCoreApplication::applicationDirPath() + "/data/BARCODE-LOGS";
//    QDir().mkdir( dir );
//
//    QString fileName = QDateTime::currentDateTime().toString( "yyyyMMdd" ) + ".csv";
//    QString filePath = dir + "/" + fileName;
//
//    QFile fileWriter( filePath );
//    if ( !fileWriter.exists() )
//    {
//        if ( fileWriter.open( QIODevice::WriteOnly ) )
//        {
//            fileWriter.write( "TIME,CODE," );
//            fileWriter.close();
//        }
//    }
//
//    QString data = "\n" + QDateTime::currentDateTime().toString( "HH:mm:ss" ) + "," + code + ",";
//    if ( fileWriter.open( QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text ) )
//    {
//        fileWriter.write( data.toStdString().c_str(), data.length() );
//        fileWriter.close();
//    }
//}

void HxBarcode::ReLoadSetting()
{
    m_settings.Load();
}

HxBarcode* Barcode()
{
    static HxBarcode instance;
    return &instance;
}
