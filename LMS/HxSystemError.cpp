#include "HxSystemError.h"



HxSystemError::HxSystemError()
{

}


void HxSystemError::errorReport( HxException ex )
{
    emit reported( ex );
}

HxSystemError* HxSystemError::instance()
{
    static HxSystemError _ins;
    return &_ins;
}
