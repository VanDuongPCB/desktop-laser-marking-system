#include "HxProtector.h"

#include "QApplication"

#include "HxSettings.h"
#include "HxFileManager.h"
#include "HxMessage.h"
#include "HxEvent.h"

HxProtector::HxProtector() : QObject( nullptr )
{
    m_settings.Load();
    qApp->installEventFilter( this );
}

HxProtector::~HxProtector()
{}

bool HxProtector::Login( QString name, QString pass )
{
    if ( pass.isEmpty() || name.isEmpty() )
    {
        HxMsgError( tr( "Mật khẩu hoặc tên không được để trống!" ) );
        return false;
    }
    QString passwordKey = "Protect/" + name + "Password";
    QString userName = name.trimmed().toLower();
    QString setupPassword = m_settings.String( passwordKey );

    if ( pass == setupPassword )
    {
        m_pProfile = HxProfile::Create();
        m_pProfile->SetID( name.trimmed().toLower() );
        m_pProfile->SetName( name );
        m_pProfile->SetPassword( pass );

        if ( name == "Admin" )
        {
            m_pProfile->SetPermission( "ADMIN", true);
        }
        else if ( name == "Super" )
        {
            m_pProfile->SetPermission( "SUPER", true );
        }
        else if ( name == "Leader" )
        {
            m_pProfile->SetPermission( "LEADER", true );
        }

        qApp->postEvent( qApp, new HxEvent( HxEvent::eLoginEvent ) );
        return true;
    }
    else
    {
        HxMsgError( tr( "Mật khẩu không đúng!" ) );
        return false;
    }
    return false;
}

void HxProtector::Logout()
{
    m_pProfile.reset();
    qApp->postEvent( qApp, new HxEvent( HxEvent::eLogoutEvent ) );
}

HxProfilePtr HxProtector::Profile()
{
    return m_pProfile;
}

bool HxProtector::eventFilter( QObject* watched, QEvent* event )
{
    HxEvent* hxEvent( nullptr );
    HxEvent::Type type;
    if ( !HxEvent::IsCustomEvent( event, hxEvent, type ) )
        return QObject::eventFilter( watched, event );

    switch ( type )
    {
    case HxEvent::eSettingChanged:
        m_settings.Load();
        break;
    default:
        break;
    }
    return QObject::eventFilter( watched, event );
}

HxProtector* Protector()
{
    static HxProtector instance;
    return &instance;
}
