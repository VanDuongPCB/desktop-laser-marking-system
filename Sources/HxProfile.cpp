#include "HxProfile.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include "QStringBuilder"

#include "HxDatabase.h"
#include "HxFileManager.h"
#include "HxSettings.h"

HxProfile::HxProfile()
{

}

HxProfile::~HxProfile()
{

}

QString HxProfile::ID() const
{
    return m_id.trimmed().toLower();
}
QString HxProfile::Name() const
{
    return m_name;
}

QString HxProfile::Password() const
{
    return m_password;
}

QString HxProfile::Permissions() const
{
    QStringList items;
    for ( auto& permission : m_permissions )
    {
        items.push_back( permission.trimmed().toUpper() );
    }
    return items.join( "," );
}

bool HxProfile::Permission( const QString& permission ) const
{
    QString permisionName = permission.trimmed().toUpper();
    auto it = m_permissions.find( permisionName );
    return it != m_permissions.end();
}

void HxProfile::SetID( const QString& ID )
{
    Modify( m_id, ID, eNew );
}
void HxProfile::SetName( const QString& name )
{
    Modify( m_name, name, eName );
}

void HxProfile::SetPassword( const QString& password )
{
    Modify( m_password, password, ePassword );
}

void HxProfile::SetPermission( const QString& permission, bool isOn )
{
    QString permisionName = permission.trimmed().toUpper();
    if ( permisionName.isEmpty() )
        return;
    if ( isOn )
    {
        auto it = m_permissions.find( permisionName );
        if ( it == m_permissions.end() )
        {
            m_permissions.insert( permisionName );
            SetModified( ePermission );
        }
    }
    else
    {
        auto it = m_permissions.find( permisionName );
        if ( it != m_permissions.end() )
        {
            m_permissions.erase( permisionName );
            SetModified( ePermission );
        }
    }
}

HxProfilePtr HxProfile::Create()
{
    return std::make_shared<HxProfile>();
}