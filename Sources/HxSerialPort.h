#pragma once
#include <QSerialPort>



class HxSerialPort : public QSerialPort
{
public:
    explicit HxSerialPort( QObject* parent = nullptr );
    explicit HxSerialPort( QString port, QObject* parent = nullptr );
    explicit HxSerialPort( QString port, int baud, QObject* parent = nullptr );

    bool WriteLine( QString data, int timeout = 5000 );
    QString ReadLine( int timeout = 5000 );
};
