
#include "exception.h"
#include <QDateTime>

Exception::Exception(): QException(){
    time = QDateTime::currentDateTime().toString("yyyy/MM/dd HH:mm:ss");
}

Exception::Exception(QString what) : Exception(){
    this->message = what;
}

Exception::Exception(QString where, QString what) : Exception(what){
    this->where = where;
}
