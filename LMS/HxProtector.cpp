#include "HxProtector.h"
#include "HxSettings.h"

#include "HxFileManager.h"

namespace
{
HxProtector s_instance;
}

HxProtector::HxProtector( QObject* parent ) : QObject( parent )
{
    admin.name = "Admin";
    admin.pass = "Laser1";
    admin.isAdmin = true;
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
        _currentUser = &admin;
        emit loginChanged();
        return true;
    }

    for ( auto& user : HxProfile::items )
    {
        if ( user->name.trimmed().toLower() == name )
        {
            if ( user->pass == "" )
            {
                _currentUser = user.get();
                _currentUser->pass = pass;
                HxProfile::save();
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

HxProfile* HxProtector::currentUser()
{
    return _currentUser;
}

HxProtector* GetProtector()
{
    return &s_instance;
}
