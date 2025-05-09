#pragma once
#include <vector>
#include <memory>
#include <set>
#include "map"

#include "QString"

#include "HxObject.h"
#include "HxSettings.h"

class HxProfile;
using HxProfilePtr = std::shared_ptr<HxProfile>;
using HxProfilePtrArray = std::vector<HxProfilePtr>;
using HxProfilePtrMap = std::map<QString, HxProfilePtr>;

class HxProfile : public HxObject
{
public:
    enum ModifyType
    {
        eNew = 0x01,
        eName = 0x02,
        ePassword = 0x04,
        ePermission = 0x08
    };

    HxProfile();
    ~HxProfile();

    QString ID() const;
    QString Name() const;
    QString Password() const;
    QString Permissions() const;
    bool Permission( const QString& permission ) const;

    void SetID( const QString& ID );
    void SetName( const QString& name );
    void SetPassword( const QString& password );
    void SetPermission( const QString& permission, bool isOn );

private:
    QString m_id;
    QString m_name;
    QString m_password;
    std::set<QString> m_permissions;

public:
    static HxProfilePtr Create();
};