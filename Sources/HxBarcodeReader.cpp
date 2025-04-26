
#include "HxBarcodeReader.h"
#include "HxSettings.h"
#include "HxTcpSocket.h"
#include "HxMessage.h"

bool HxBarcodeReader::hasData()
{
    QString cmd = "RD " + HxSettings::barcodeReg + "\r";
    HxTcpSocket sock( HxSettings::plcIp, HxSettings::plcPort );
    sock.connect();
    sock.writeLine( cmd, 2000 );
    QString fb = sock.readLine( 2000 );
    return fb == "1";
}

bool HxBarcodeReader::clear()
{
    QString cmd = "WR " + HxSettings::laserTriggerConfirmReg + " 1\r";
    HxTcpSocket sock( HxSettings::plcIp, HxSettings::plcPort );
    sock.connect();
    sock.writeLine( cmd, 2000 );
    QString fb = sock.readLine( 2000 );
    return fb == "OK";
}

QString HxBarcodeReader::read()
{
    QString cmd = "RDS " + HxSettings::barcodeDataReg + " 40\r";
    HxTcpSocket sock( HxSettings::plcIp, HxSettings::plcPort );
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

bool HxBarcodeReader::sendFeedback( bool status )
{
    QString val1 = status == 1 ? "1" : "0";
    QString val2 = status == 1 ? "0" : "1";
    QString cmd = "WRS " + HxSettings::barcodeOKReg + " 2 " + val1 + " " + val2 + "\r";
    HxTcpSocket sock( HxSettings::plcIp, HxSettings::plcPort );
    sock.connect();
    sock.writeLine( cmd, 2000 );
    QString fb = sock.readLine( 2000 );
    cmd = "WR " + HxSettings::barcodeConfirmReg + " 1\r";
    sock.writeLine( cmd, 2000 );
    fb = sock.readLine( 2000 );
    return fb == "OK";
}
