#pragma once
#include "QString"
#include <QMap>
#include "HxPosition.h"
#include "HxDesign.h"
#include "HxStopper.h"

class HxLaserDevice
{
private:
    static bool DetectPortExisting();
    static QString SendData( QString data, int timeout = 30000 );

public:
    static bool SetProgram( QString name );
    static bool SetupBlockData( QString program, std::map<int, QString> blocks );
    static bool SetupPosition( QString program, HxPosition pos, int stopper, HxDesign design );
    static bool Burn();
};
