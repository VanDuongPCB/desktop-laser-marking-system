
#include "serialport.h"
#include "exception.h"
#include "systemerror.h"

SerialPort::SerialPort(QObject *parent) : QSerialPort{parent}{

}

SerialPort::SerialPort(QString port, QObject *parent) : QSerialPort{parent}{
    this->setPortName(port);
}

SerialPort::SerialPort(QString port, int baud, QObject *parent) : QSerialPort{parent}{
    this->setPortName(port);
    this->setBaudRate(baud);
}

bool SerialPort::writeLine(QString data, int timeout){
    data += "\n";
    this->write(data.toStdString().c_str());
    if(!waitForBytesWritten(timeout)){
        QStringList items = {
            "Không gửi được dữ liệu trên cổng serial !",
            "Cổng : " + portName()
        };
        throw Exception(items.join("\n"));
    }
    return true;
}

QString SerialPort::readLine(int timeout){
    if(!isOpen()){
        QStringList items = {
            "Cổng dữ liệu đang đóng !",
            "Cổng : " + portName()
        };
        throw Exception(items.join("\n"));
    }

    if(!waitForReadyRead(timeout)){
        QStringList items = {
            "Không nhận được phản hồi trên cổng serial !",
            "Cổng : " + portName()
        };
        throw Exception(items.join("\n"));
    }

    std::vector<char> buffer;
    char oneBuff[1];
    do{
        if(bytesAvailable()){
            read(oneBuff,1);
            if(oneBuff[0] != 13) buffer.push_back(oneBuff[0]);
            else break;
        }
        else if(!waitForReadyRead(20)){
            break;
        }
    }while(true);
    close();

    buffer.push_back(0);
    QString data = QString::fromStdString(buffer.data());
    return data;
}
