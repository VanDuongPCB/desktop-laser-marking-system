#pragma once
#include "QObject"

class HxObject
{
public:
    HxObject();
    void SetModified( uint64_t flags );
    bool IsMofified( uint64_t flags = UINT64_MAX ) const;
    void ClearModified();
    template<typename T>
    void Modify( T& field, const T& value, uint64_t flag = UINT64_MAX )
    {
        if ( field != value )
        {
            field = value;
            m_flags |= flag;
        }
    }
private:
    uint64_t m_flags = 0;
};
