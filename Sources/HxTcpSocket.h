#pragma once
#include <QTcpSocket>



class HxTcpSocket : private QTcpSocket
{
private:
    QString ip;
    int port = 0;
public:
    explicit HxTcpSocket( QObject* parent = nullptr );
    explicit HxTcpSocket( QString ip, int port, QObject* parent = nullptr );
    bool Connect( int timeout = 5000 );
    bool WriteLine( QString data, int timeout = 2000 );
    QString ReadLine( int timeout = 2000 );
};
