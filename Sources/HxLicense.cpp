#include "HxLicense.h"
#include "QSysInfo"
#include "QCoreApplication"
#include "QFile"
#include "HxMessage.h"
#include "HxSettings.h"

QString HxLicensing::ReadKey() const
{
    HxRegistrySetting setting;
    QString keyVersion = "License/" + m_Version;
    return setting.String( keyVersion );
}

void HxLicensing::WriteKey( const QString& key ) const
{
    HxRegistrySetting setting;
    QString keyVersion = "License/" + m_Version;
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
    std::string sid = ( id ).toStdString();
    std::vector<char> buff;
    for ( int i = 0; i < sid.length(); i++ )
    {
        if ( i > 0 && i % 5 == 0 )
        {
            buff.push_back( '-' );
        }

        int val = sid[ i ] * 69 + i;
        val *= m_Kernel;
        val = val % 36;
        if ( val < 10 )
        {
            buff.push_back( '0' + val );
        }
        else
        {
            buff.push_back( val - 10 + 'A' );
        }
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
    return m_Version;
}

HxLicensing* Licensing()
{
    static HxLicensing instance;
    return &instance;
}
