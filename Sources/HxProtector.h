#pragma once
#include "QObject"
#include "QString"
#include "HxProfile.h"

class HxProtector : public QObject
{
    Q_OBJECT
public:
    explicit HxProtector();
    ~HxProtector();
    bool Login( QString name, QString pass );
    void Logout();
    HxProfilePtr Profile();
private:
    HxRegistrySetting m_settings;
    HxProfilePtr m_pProfile;
    bool eventFilter( QObject* watched, QEvent* event );
};


HxProtector* Protector();
