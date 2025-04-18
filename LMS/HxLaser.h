#pragma once
#include "QString"
#include "QMap"

#include "HxPosition.h"
#include "HxDesign.h"
#include "HxStopper.h"

class HxLaserMachine
{
private:
    static bool detectPortExisting();
    static QString sendData( QString data, int timeout = 30000 );
    static QString detectError( QString data );

public:
    static bool setProgram( QString name );
    static bool setupBlockData( QString program, QMap<int, QString> data );
    static bool setupPosition( QString program, HxPosition pos, int stopper, HxDesign design );
    static bool burn();
};

HxLaserMachine* GetLaserMachine();
