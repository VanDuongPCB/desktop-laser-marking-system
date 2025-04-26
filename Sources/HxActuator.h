#pragma once
class HxActuator
{
public:
    static bool SetEnable( bool en );
    static bool SetMarkResult( bool status );
    static bool SetCvWidth( double width );
    static bool SetTransferMode( bool on );
    static bool SetStopper( int stopper );
    static bool IsHasTrigger();
    static bool ConfirmTrigger();
    static bool SetCompleteBit();
};
