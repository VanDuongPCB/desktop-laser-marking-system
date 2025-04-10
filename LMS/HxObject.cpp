#include "HxObject.h"

HxObject::HxObject()
{}

void HxObject::SetModified(int flags)
{
    m_flags |= flags;
}

bool HxObject::IsMofified(int flags)
{
    return (m_flags & flags) > 0;
}

void HxObject::ClearModified()
{
    m_flags = 0;
}
