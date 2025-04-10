#include "HxLicense.h"
#include <QSysInfo>
#include <QCoreApplication>
#include <QFile>
#include "HxMessage.h"
#include "QSettings"

namespace
{
HxLicensing s_instance;
}

QString HxLicensing::readKey()
{
    QSettings settings( "HKEY_CURRENT_USER\\Software\\Laser Marker System\\License", QSettings::NativeFormat );
    QString key = settings.value( "Key" ).toString();
    return key;
}

void HxLicensing::writeKey( QString key )
{
    QSettings settings( "HKEY_CURRENT_USER\\Software\\Laser Marker System\\License", QSettings::NativeFormat );
    settings.setValue( "Key", key );
}

bool HxLicensing::isRegistered()
{
    QString keyRef = keyFromId( id() );
    QString keyStore = readKey();
    return keyStore == keyRef;
}

QString HxLicensing::id()
{
    return QSysInfo::machineUniqueId().toUpper() + "-R2025";
}

QString HxLicensing::keyFromId( QString id )
{
    std::string sid = id.toStdString();
    std::vector<char> buff;
    for ( int i = 0; i < sid.length(); i++ )
    {
        if ( i > 0 && i % 5 == 0 )
        {
            buff.push_back( '-' );
        }

        int val = sid[ i ] * 69 + i;
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

bool HxLicensing::registerKey( QString key )
{
    QString keyRef = keyFromId( id() );
    if ( keyRef == key )
    {
        writeKey( key );
        return true;
    }
    else
    {
        return false;
    }
}


HxLicensing* GetLicensing()
{
    return &s_instance;
}
