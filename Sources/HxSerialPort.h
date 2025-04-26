
#ifndef SERIALPORT_H
#define SERIALPORT_H

#include <QSerialPort>



class HxSerialPort : public QSerialPort
{
public:
    explicit HxSerialPort( QObject* parent = nullptr );
    explicit HxSerialPort( QString port, QObject* parent = nullptr );
    explicit HxSerialPort( QString port, int baud, QObject* parent = nullptr );

    bool writeLine( QString data, int timeout = 30000 );
    QString readLine( int timeout = 30000 );
};

#endif // SERIALPORT_H
