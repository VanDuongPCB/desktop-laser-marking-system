#include "HxProtector.h"
#include "HxSettings.h"

HxProtector::HxProtector( QObject* parent ) : QObject( parent )
{
    admin.name = "Admin";
    admin.pass = HxSettings::password;
    admin.isAdmin = true;
}

HxProtector::~HxProtector()
{

}

bool HxProtector::login( QString name, QString pass )
{
    name = name.trimmed().toLower();
    if ( name == "admin" && pass == HxSettings::password )
    {
        _currentUser = &admin;
        emit loginChanged();
        return true;
    }

    for ( auto& user : HxUserProfile::items )
    {
        if ( user->name.trimmed().toLower() == name )
        {
            if ( user->pass == "" )
            {
                _currentUser = user.get();
                _currentUser->pass = pass;
                HxUserProfile::save();
                emit loginChanged();
            }
            else if ( user->pass == pass )
            {
                _currentUser = user.get();
                emit loginChanged();
                return true;
            }
        }
    }
    _currentUser = nullptr;
    emit loginChanged();
    return false;
}

void HxProtector::logout()
{
    _currentUser = nullptr;
    emit loginChanged();
}

HxUserProfile* HxProtector::currentUser()
{
    return _currentUser;
}


/* -------------------------- */
HxProtector* HxProtector::instance()
{
    static HxProtector p;
    return &p;
}
