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

uint64_t HxObject::ModifyFlags() const
{
    return m_flags;
}

void HxObject::ClearModified()
{
    m_flags = 0;
}
