
#include "tcpsocket.h"
#include "exception.h"
#include "systemerror.h"

TcpSocket::TcpSocket(QObject *parent) : QTcpSocket{parent}
{

}

TcpSocket::TcpSocket(QString ip, int port, QObject *parent){
    this->ip = ip;
    this->port = port;
}

bool TcpSocket::connect(int timeout){
    connectToHost(ip, port);
    if(!waitForConnected(timeout)){
        QStringList items = {
            "Kết nối tới plc hết hạn !",
            "IP : " + ip,
            "Port : " + QString::number(port)
        };
        throw Exception(items.join("\n"));
    }
    return true;
}

bool TcpSocket::writeLine(QString data, int timeout){
    std::string s = data.toStdString();
    this->write(s.data(), s.length());
    if(!waitForBytesWritten(timeout)){
        QStringList items = {
            "Gửi dữ liệu đến plc hết hạn !",
            "IP : " + ip,
            "Port : " + QString::number(port)
        };
        throw Exception(items.join("\n"));
    }
    return true;
}

QString TcpSocket::readLine(int timeout){
    if(!waitForReadyRead(timeout)){
        QStringList items = {
            "Plc không phản hồi !",
            "IP : " + ip,
            "Port : " + QString::number(port)
        };
        throw Exception(items.join("\n"));
    }
    char buffer[4096] {0};
    this->read(buffer, 4095);
    return QString::fromStdString(buffer).trimmed();
}
