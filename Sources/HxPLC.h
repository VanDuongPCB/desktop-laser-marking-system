#pragma once
#include "QObject"
#include "HxSettings.h"


class HxPLC : private QObject
{
    Q_OBJECT
public:
    HxPLC();
    bool SetEnable( bool en );
    bool SetMarkResult( bool status );
    bool SetCvWidth( double width );
    bool SetTransferMode( bool on );
    bool SetStopper( int stopper );
    bool IsHasTrigger();
    bool ConfirmTrigger();
    bool SetCompleteBit();
    void ReLoadSetting();
private:
    HxRegistrySetting m_settings;
};

HxPLC* PLC();