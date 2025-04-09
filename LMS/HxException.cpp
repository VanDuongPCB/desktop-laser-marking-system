
#include "HxException.h"
#include <QDateTime>

HxException::HxException() : QException()
{
    time = QDateTime::currentDateTime().toString( "yyyy/MM/dd HH:mm:ss" );
}

HxException::HxException( QString what ) : HxException()
{
    this->message = what;
}

HxException::HxException( QString where, QString what ) : HxException( what )
{
    this->where = where;
}
