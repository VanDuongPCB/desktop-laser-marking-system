#pragma once
#include "QTcpSocket"

class HxTcpSocket : public QTcpSocket
{
public:
    explicit HxTcpSocket( QObject* parent = nullptr );
    explicit HxTcpSocket( const QString& ip, int port, QObject* parent = nullptr );
    bool Connect( int timeout = 5000 );
    bool WriteLine( const QString& data, int timeout = 2000 );
    QString ReadLine( int timeout = 2000 );
private:
    QString m_ip;
    int m_port = 0;
};

