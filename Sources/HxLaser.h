#pragma once
#include "QString"
#include "QMap"
#include "QObject"

#include "HxPosition.h"
#include "HxDesign.h"
#include "HxStopper.h"
#include "HxSettings.h"

class HxLaser : public QObject
{
private:
    QString SendData( const QString& data, int timeout = 10000 );

public:
    HxLaser();
    bool SetProgram( const QString& name );
    bool SetupBlockData( const QString& program, std::map<int, QString> dataMap );
    bool SetupPosition( const QString& program, HxPosition pos, int stopper, HxDesignPtr pDesign );
    bool Burn();
    void ReLoadSetting();
private:
    HxRegistrySetting m_settings;
};

HxLaser* Laser();
