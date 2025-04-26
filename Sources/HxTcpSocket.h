#pragma once
#include <QTcpSocket>



class HxTcpSocket : public QTcpSocket
{
private:
    QString ip;
    int port = 0;
public:
    explicit HxTcpSocket( QObject* parent = nullptr );
    explicit HxTcpSocket( QString ip, int port, QObject* parent = nullptr );
    bool connect( int timeout = 5000 );
    bool writeLine( QString data, int timeout = 2000 );
    QString readLine( int timeout = 2000 );
};
