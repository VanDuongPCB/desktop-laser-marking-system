#pragma once
#include "QString"

class HxLicensing
{
public:
     QString readKey();
     void writeKey( QString key );
     bool isRegistered();
     QString id();
     QString keyFromId( QString id );
     bool registerKey( QString key );
     QString GetVersion();
};

HxLicensing* GetLicensing();
