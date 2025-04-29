#include "HxSystemError.h"



HxSystemError::HxSystemError()
{

}


void HxSystemError::ErrorReport( HxException ex )
{
    emit Reported( ex );
}

HxSystemError* HxSystemError::Instance()
{
    static HxSystemError _ins;
    return &_ins;
}
