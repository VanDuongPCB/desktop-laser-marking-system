#pragma once
#include "QException"



class HxException : public QException
{
private:
public:
    HxException();
    HxException( const QString& what );
    HxException( const QString& where, const QString& what );

    QString Time() const;
    QString Where() const;
    QString Message() const;

    void SetWhere( const QString& where );
    void SetMessage( const QString& message );
    QByteArray toJson();

private:
    QString m_time = "0000/00/00 00:00:00";
    QString m_where = "";
    QString m_message = "";
};