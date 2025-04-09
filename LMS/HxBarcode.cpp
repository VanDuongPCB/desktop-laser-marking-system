
#include "HxBarcode.h"
#include "HxSettings.h"
#include "HxTcpSocket.h"
#include "HxMessage.h"
#include "QCoreApplication"
#include "QTime"
#include "QDir"
#include "QFile"

#include "HxFileManager.h"

namespace
{
    HxBarcode s_instance;
}

bool HxBarcode::hasData()
{
    QString cmd = "RD " + NxSettings::barcodeReg + "\r";
    TcpSocket sock( NxSettings::plcIp, NxSettings::plcPort );
    sock.connect();
    sock.writeLine( cmd, 2000 );
    QString fb = sock.readLine( 2000 );
    return fb == "1";
}

bool HxBarcode::clear()
{
    QString cmd = "WR " + NxSettings::laserTriggerConfirmReg + " 1\r";
    TcpSocket sock( NxSettings::plcIp, NxSettings::plcPort );
    sock.connect();
    sock.writeLine( cmd, 2000 );
    QString fb = sock.readLine( 2000 );
    return fb == "OK";
}

QString HxBarcode::read()
{
    QString cmd = "RDS " + NxSettings::barcodeDataReg + " 40\r";
    TcpSocket sock( NxSettings::plcIp, NxSettings::plcPort );
    sock.connect();
    sock.writeLine( cmd, 2000 );
    QString fb = sock.readLine( 2000 );
    QStringList codeItems = fb.split( ' ' );
    if ( codeItems.size() < 1 ) return "";
    std::vector<wchar_t> buffer( codeItems.size() + 1, 0 );
    for ( int i = 0; i < codeItems.size(); i++ )
    {
        int val = codeItems[ i ].toInt( 0 );
        buffer[ i ] = ( ( val >> 8 ) | ( val << 8 ) ) & 0xFFFF;
    }

    char* ptr = ( char* )buffer.data();

    return QString::fromStdString( ptr );
}

bool HxBarcode::sendFeedback( bool status )
{
    QString val1 = status == 1 ? "1" : "0";
    QString val2 = status == 1 ? "0" : "1";
    QString cmd = "WRS " + NxSettings::barcodeOKReg + " 2 " + val1 + " " + val2 + "\r";
    TcpSocket sock( NxSettings::plcIp, NxSettings::plcPort );
    sock.connect();
    sock.writeLine( cmd, 2000 );
    QString fb = sock.readLine( 2000 );
    cmd = "WR " + NxSettings::barcodeConfirmReg + " 1\r";
    sock.writeLine( cmd, 2000 );
    fb = sock.readLine( 2000 );
    return fb == "OK";
}

void HxBarcode::save( QString code )
{
    QString dir = GetFileManager()->GetPath(HxFileManager::eDBBarcodeDir);
    QDir().mkdir( dir );

    QString fileName = QDateTime::currentDateTime().toString( "yyyyMMdd" ) + ".csv";
    QString filePath = dir + "/" + fileName;

    QFile fileWriter( filePath );
    if ( !fileWriter.exists() )
    {
        if ( fileWriter.open( QIODevice::WriteOnly ) )
        {
            fileWriter.write( "TIME,CODE," );
            fileWriter.close();
        }
    }

    QString data = "\n" + QDateTime::currentDateTime().toString( "HH:mm:ss" ) + "," + code + ",";
    if ( fileWriter.open( QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text ) )
    {
        fileWriter.write( data.toStdString().c_str(), data.length() );
        fileWriter.close();
    }
}


HxBarcode* GetBarcode()
{
    return &s_instance;
}
