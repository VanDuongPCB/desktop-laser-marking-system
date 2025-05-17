#include "HxLicense.h"
#include "QSysInfo"
#include "QCoreApplication"
#include "QFile"
#include "HxMessage.h"
#include "HxSettings.h"

namespace
{
    const QString s_majorVersion = "2025";
    const QString s_minorVersion = "1";
    const QString s_buildVersion = "1";
    const int s_appTypeID = 1234;
    const std::map<QString, QString> s_kernels =
    {
        { "2024.4", "A2A460D7-3187-477B-A585-7FA824EBD68B" },
        { "2025.0", "3C5262C6-726B-4434-8E35-A3EBEBD042F4" },
        { "2025.1", "D7C88F93-CDB8-4E6E-8DB2-DBDBA070E893" },
        { "2025.2", "4B888ABB-74EF-4251-8E9E-3AD0FEB91640" },
        { "2025.3", "C9F7FA1A-2137-4B41-9727-18D9242F7DEE" },
        { "2025.4", "2C1A1A71-7E4A-44FF-B793-E334BE9C076B" },
        { "2026.0", "3CF1C417-DCB4-4E85-9C92-BCAC1B13930E" },
        { "2026.1", "C69039AD-834E-4610-A467-9F259AA4E995" },
        { "2026.2", "14CF1458-27C0-406C-BDEB-2EA4E8B4255E" },
        { "2026.3", "2103EC17-3456-4E40-B8DA-95B33506AECF" },
        { "2026.4", "CCB5D2E1-2977-45E0-8600-113C31E70134" }
    };
}

QString HxLicensing::ReadKey() const
{
    HxRegistrySetting setting;
    QString keyVersion = QString( "License/%1.%2" ).arg( s_majorVersion ).arg( s_minorVersion );
    return setting.String( keyVersion );
}

void HxLicensing::WriteKey( const QString& key ) const
{
    HxRegistrySetting setting;
    QString keyVersion = QString( "License/%1.%2" ).arg( s_majorVersion ).arg( s_minorVersion );
    setting.Clear();
    setting.Set( keyVersion, key );
    setting.Save();
}

bool HxLicensing::IsRegistered()
{
    QString keyRef = KeyFromId( ID() );
    QString keyStore = ReadKey();
    return keyStore == keyRef;
}

QString HxLicensing::ID()
{
    return QSysInfo::machineUniqueId().toUpper();
}

QString HxLicensing::KeyFromId( const QString& id ) const
{
    const std::string c = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string kernel = "C2D62944-5508-42E7-9933-A0AAF8E65BFE";
    auto it = s_kernels.find( QString( "%1.%2" ).arg( s_majorVersion ).arg( s_minorVersion ) );
    if ( it != s_kernels.end() )
        kernel = it->second.toStdString();

    std::string sid = id.toStdString();
    int size = sid.length();
    std::vector<char> buff;
    for ( int i = 0; i < size; i++ )
    {
        if ( kernel[ i ] == '-' )
        {
            buff.push_back( sid[ i ] );
            continue;
        }

        int val = abs( ( int )sid[ i ] * ( int )kernel[ i ] * i * s_appTypeID );
        val = val % 36;
        buff.push_back( c[ val ] );
    }
    buff.push_back( 0 );
    return QString::fromStdString( buff.data() );
}

bool HxLicensing::RegisterKey( const QString& key )
{
    QString keyRef = KeyFromId( ID() );
    if ( keyRef == key )
    {
        WriteKey( key );
        return true;
    }
    else
    {
        return false;
    }
}

QString HxLicensing::GetVersion()
{
    return QString( "%1.%2.%3" )
        .arg( s_majorVersion )
        .arg( s_minorVersion )
        .arg( s_buildVersion );
}

HxLicensing* Licensing()
{
    static HxLicensing instance;
    return &instance;
}
