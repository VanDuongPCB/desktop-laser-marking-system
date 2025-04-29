#pragma once
#include <QException>

class HxException : public QException
{
private:
public:
    HxException();
    HxException( QString what );
    HxException( QString where, QString what );
    QString time = "0000/00/00 00:00:00";
    QString where = "";
    QString message = "";
};

