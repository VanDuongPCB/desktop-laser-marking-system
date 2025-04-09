#pragma once
class HxPLC
{
public:
    bool setEnable( bool en );
    bool setMarkResult( bool status );
    bool setCvWidth( double width );
    bool setTransferMode( bool on );
    bool setStopper( int stopper );
    bool hasTrigger();
    bool confirmTrigger();
    bool setCompleteBit();
};

HxPLC* GetPLC();