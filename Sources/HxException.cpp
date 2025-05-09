
#include "HxException.h"
#include "QDateTime"
#include "QJsonObject"
#include "QJsonDocument"

HxException::HxException() : QException()
{
    m_time = QDateTime::currentDateTime().toString( "yyyy/MM/dd HH:mm:ss" );
}

HxException::HxException( const QString& what ) : HxException()
{
    this->m_message = what;
}

HxException::HxException( const QString& where, const QString& what ) : HxException( what )
{
    this->m_where = where;
}

QString HxException::Time() const
{
    return m_time;
}

QString HxException::Where() const
{
    return m_where;
}

QString HxException::Message() const
{
    return m_message;
}

void HxException::SetWhere( const QString& where )
{
    m_where = where;
}

void HxException::SetMessage( const QString& message )
{
    m_message = message;
}

QByteArray HxException::toJson()
{
    QJsonObject obj;
    obj.insert( "time", m_time );
    obj.insert( "where", m_where );
    obj.insert( "message", m_message );
    QJsonDocument doc;
    doc.setObject( obj );
    return doc.toJson();
}