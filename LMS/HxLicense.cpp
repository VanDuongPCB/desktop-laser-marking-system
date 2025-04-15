#include "HxLicense.h"
#include "QSysInfo"
#include "QCoreApplication"
#include "QFile"
#include "HxMessage.h"
#include "QSettings"

namespace
{
    QString s_Version = "2025.SP0";
    double s_Kernel = 0.202500;
    HxLicensing s_instance;
}

QString HxLicensing::ReadKey()
{
    QSettings settings( "HKEY_CURRENT_USER\\Software\\Laser Marker System\\License", QSettings::NativeFormat );
    QString key = settings.value( s_Version ).toString();
    return key;
}

void HxLicensing::WriteKey( const QString& key )
{
    QSettings settings( "HKEY_CURRENT_USER\\Software\\Laser Marker System\\License", QSettings::NativeFormat );
    settings.setValue( s_Version, key );
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

QString HxLicensing::KeyFromId( const QString& id )
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
        val *= s_Kernel;
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
    return s_Version;
}

HxLicensing* GetLicensing()
{
    return &s_instance;
}
