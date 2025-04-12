#include "HxProtector.h"
#include "HxSettings.h"

#include "HxFileManager.h"

namespace
{
HxProtector s_instance;
}

HxProtector::HxProtector( QObject* parent ) : QObject( parent )
{
    auto settings = GetFileManager()->GetSettings(HxFileManager::eSettingProtect);
    admin = std::make_shared<HxProfile>();
    admin->name = "Admin";
    admin->pass = settings->value("Password").toString();
    admin->isAdmin = true;
}

HxProtector::~HxProtector()
{

}

bool HxProtector::login( QString name, QString pass )
{
    auto settings = GetFileManager()->GetSettings(HxFileManager::eSettingProtect);
    QString adminPass = settings->value("Password").toString();
    if(adminPass.isEmpty())
    {
        adminPass = "Laser1";
        settings->setValue("Password", adminPass);
    }

    name = name.trimmed().toLower();
    if ( name == "admin" && pass == adminPass )
    {
        _currentUser = admin;
        emit loginChanged();
        return true;
    }

    HxProfilePtrArray items = GetProfileManager()->GetProfiles();
    for ( auto& user : items )
    {
        if ( user->name.trimmed().toLower() == name )
        {
            if ( user->pass == "" )
            {
                _currentUser = user;
                _currentUser->pass = pass;
                GetProfileManager()->Save(_currentUser);
                emit loginChanged();
            }
            else if ( user->pass == pass )
            {
                _currentUser = user;
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

HxProfilePtr HxProtector::currentUser()
{
    return _currentUser;
}

HxProtector* GetProtector()
{
    return &s_instance;
}
