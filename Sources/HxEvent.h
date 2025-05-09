#pragma once
#include "QEvent"
#include "QVariant"

class HxEvent : public QEvent
{
public:
    enum Type
    {
        eCustomEvent,
        eLoginEvent,
        eLogoutEvent,

        eDesignAdded,
        eDesignDeleted,
        eDesignChanged,

        eModelAdded,
        eModelDeleted,
        eModelChanged,

        eLOTAdded,
        eLOTDeleted,
        eLOTChanged,

        eSettingChanged,

        eMarkerSetupChanged,
        eMarkerGoFree,
        eMarkerGoError,
        eMarkerGoTransfer,
        eMarkerGoMark,
        eMarkerGoTest,
        eMarkerMarked,
        eMarkerGoFinish,
        eMarkerStopped,
    };
    HxEvent( Type type );
    HxEvent( Type type, QVariant data );
    ~HxEvent();
    Type GetType() const;
    QVariant Data() const;
private:
    Type m_type = eCustomEvent;
    QVariant m_data;

public:
    static bool IsCustomEvent( QEvent* event, HxEvent*& hxEvent, HxEvent::Type& type );
};