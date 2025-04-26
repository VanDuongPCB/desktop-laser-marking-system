
#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <QException>



class Exception : public QException{
private:
public:
    Exception();
    Exception(QString what);
    Exception(QString where, QString what);
    QString time = "0000/00/00 00:00:00";
    QString where = "";
    QString message = "";
};

#endif // EXCEPTION_H
