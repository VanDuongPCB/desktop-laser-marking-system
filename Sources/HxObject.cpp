#include "HxObject.h"

HxObject::HxObject()
{
}

void HxObject::SetModified( uint64_t flags )
{
    m_flags |= flags;
}

bool HxObject::IsMofified( uint64_t flags ) const
{
    return ( m_flags & flags ) > 0;
}

void HxObject::ClearModified()
{
    m_flags = 0;
}
