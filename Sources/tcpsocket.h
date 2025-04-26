
#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QTcpSocket>



class TcpSocket : public QTcpSocket{
private:
    QString ip;
    int port = 0;
public:
    explicit TcpSocket(QObject *parent = nullptr);
    explicit TcpSocket(QString ip, int port, QObject *parent = nullptr);
    bool connect(int timeout = 5000);
    bool writeLine(QString data, int timeout = 2000);
    QString readLine(int timeout = 2000);
};

#endif // TCPSOCKET_H
