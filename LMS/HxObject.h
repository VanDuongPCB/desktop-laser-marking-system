#pragma once
#include "QObject"

class HxObject
{
public:
    HxObject();
    void SetModified(int flags);
    bool IsMofified(int flags = 0xFFFFFFFF);
    void ClearModified();
private:
    int m_flags = 0;
};
