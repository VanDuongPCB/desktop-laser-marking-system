
#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QSerialPort>



class SerialPort : public QSerialPort{
public:
    explicit SerialPort(QObject *parent = nullptr);
    explicit SerialPort(QString port, QObject *parent = nullptr);
    explicit SerialPort(QString port, int baud, QObject *parent = nullptr);

    bool writeLine(QString data, int timeout = 30000);
    QString readLine(int timeout = 30000);
};

#endif // SERIALPORT_H
