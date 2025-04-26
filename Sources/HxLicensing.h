#pragma once
#include <QString>

class HxLicensing
{
public:
    static QString readKey();
    static void writeKey( QString key );
    static bool isRegistered();
    static QString id();
    static QString keyFromId( QString id );
    static bool registerKey( QString key );
};

