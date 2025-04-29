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

bool HxProtector::Login( QString name, QString pass )
{
    name = name.trimmed().toLower();
    if ( name == "admin" && pass == HxSettings::password )
    {
        _currentUser = &admin;
        emit LoginChanged();
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
                HxUserProfile::Save();
                emit LoginChanged();
            }
            else if ( user->pass == pass )
            {
                _currentUser = user.get();
                emit LoginChanged();
                return true;
            }
        }
    }
    _currentUser = nullptr;
    emit LoginChanged();
    return false;
}

void HxProtector::Logout()
{
    _currentUser = nullptr;
    emit LoginChanged();
}

HxUserProfile* HxProtector::CurrentUser()
{
    return _currentUser;
}


/* -------------------------- */
HxProtector* HxProtector::Instance()
{
    static HxProtector p;
    return &p;
}
