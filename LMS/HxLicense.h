#pragma once
#include "QString"

class HxLicensing
{
public:
    QString ReadKey();
    void WriteKey( const QString& key );
    bool IsRegistered();
    QString ID();
    QString KeyFromId( const QString& id );
    bool RegisterKey( const QString& key );
    QString GetVersion();
};

HxLicensing* GetLicensing();
