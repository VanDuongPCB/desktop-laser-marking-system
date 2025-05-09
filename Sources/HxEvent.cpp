#pragma once
#include "HxEvent.h"

HxEvent::HxEvent( HxEvent::Type type ) :
    QEvent( QEvent::Type::User ),
    m_type( type )
{
}

HxEvent::HxEvent( Type type, QVariant data ) :
    QEvent( QEvent::Type::User ),
    m_type( type ),
    m_data( data )
{
}

HxEvent::~HxEvent()
{
}

HxEvent::Type HxEvent::GetType() const
{
    return m_type;
}

QVariant HxEvent::Data() const
{
    return m_data;
}

bool HxEvent::IsCustomEvent( QEvent* event, HxEvent*& hxEvent, HxEvent::Type& type )
{
    if ( !event || event->type() != HxEvent::User )
        return false;

    hxEvent = static_cast< HxEvent* >( event );
    if ( !hxEvent )
        return false;

    type = hxEvent->GetType();
    return true;
}