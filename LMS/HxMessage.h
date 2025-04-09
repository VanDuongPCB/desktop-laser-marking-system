#pragma once
#include "QString"
#include "QMessageBox"
#include "QObject"


class Message
{
public:
    // static int show(QString content);
    // static int show(QString content,QString title);
    static int show( QString content, QString title, QMessageBox::Icon icon );
    static int show( QString content, QString title, QMessageBox::Icon icon, QMessageBox::StandardButtons buttons );
    // static int error(QString content);
    // static int error(QString content, QString title);
    static int warning( QString content, QString title );
};



int HxShowMsg( const QString& content );
int HxShowMsg( const QString& content, const QString& title );

int HxShowError( const QString& error );
int HxShowError( const QString& error, const QString& title );
int HxShowError( const QString& error, const QString& details, const QString& title );
