#include "HxTcpSocket.h"
#include "HxException.h"
#include "HxSystemError.h"

HxTcpSocket::HxTcpSocket( QObject* parent ) : QTcpSocket{ parent }
{

}

HxTcpSocket::HxTcpSocket( QString ip, int port, QObject* parent )
{
    this->ip = ip;
    this->port = port;
}

bool HxTcpSocket::Connect( int timeout )
{
    connectToHost( ip, port );
    if ( !waitForConnected( timeout ) )
    {
        QStringList items = {
            "Kết nối tới plc hết hạn !",
            "IP : " + ip,
            "Port : " + QString::number( port )
        };
        throw HxException( items.join( "\n" ) );
    }
    return true;
}

bool HxTcpSocket::WriteLine( QString data, int timeout )
{
    std::string s = data.toStdString();
    this->write( s.data(), s.length() );
    if ( !waitForBytesWritten( timeout ) )
    {
        QStringList items = {
            "Gửi dữ liệu đến plc hết hạn !",
            "IP : " + ip,
            "Port : " + QString::number( port )
        };
        throw HxException( items.join( "\n" ) );
    }
    return true;
}

QString HxTcpSocket::ReadLine( int timeout )
{
    if ( !waitForReadyRead( timeout ) )
    {
        QStringList items = {
            "Plc không phản hồi !",
            "IP : " + ip,
            "Port : " + QString::number( port )
        };
        throw HxException( items.join( "\n" ) );
    }
    char buffer[ 4096 ]{ 0 };
    this->read( buffer, 4095 );
    return QString::fromStdString( buffer ).trimmed();
}
