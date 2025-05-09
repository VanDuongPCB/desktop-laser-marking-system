#pragma once
#include "QSerialPort"



class HxSerialPort : public QSerialPort
{
public:
    explicit HxSerialPort( QObject* parent = nullptr );
    explicit HxSerialPort( const QString& port, QObject* parent = nullptr );
    explicit HxSerialPort( const QString& port, int baud, QObject* parent = nullptr );

    bool WriteLine( const QString& data, int timeout = 30000 );
    QString ReadLine( int timeout = 30000 );
};

