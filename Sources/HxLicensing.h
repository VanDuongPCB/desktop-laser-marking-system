#pragma once
#include "QString"

class HxLicensing
{
public:
    static QString ReadKey();
    static void WriteKey( QString key );
    static bool IsRegistered();
    static QString ID();
    static QString KeyFromId( QString id );
    static bool RegisterKey( QString key );
};

