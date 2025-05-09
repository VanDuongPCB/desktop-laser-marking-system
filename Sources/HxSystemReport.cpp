#include "HxSystemReport.h"

namespace
{
    HxSystemReport s_instance;
}

HxSystemReport::HxSystemReport()
{

}

void HxSystemReport::Report( HxException ex )
{
    emit reported( ex );
}

HxSystemReport* GetSystemReport()
{
    return &s_instance;
}