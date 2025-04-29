#include "HxLicensing.h"
#include <QSysInfo>
#include <QCoreApplication>
#include <QFile>
#include "HxMessage.h"
#include "QSettings"

QString HxLicensing::ReadKey()
{
    QSettings settings( "HKEY_CURRENT_USER\\Software\\Laser Marker System\\License", QSettings::NativeFormat );
    QString key = settings.value( "Key" ).toString();
    return key;
    // QFile fileReader(QCoreApplication::applicationDirPath() + "/license.lic");
    // if(fileReader.open(QIODevice::ReadOnly)){
    //     QString key = fileReader.readAll();
    //     fileReader.close();
    //     return key;
    // }
    // return "";
}

void HxLicensing::WriteKey( QString key )
{
    QSettings settings( "HKEY_CURRENT_USER\\Software\\Laser Marker System\\License", QSettings::NativeFormat );
    settings.setValue( "Key", key );
    // QString licPath = QCoreApplication::applicationDirPath() + "/license.lic";
    // QFile fileWriter(licPath);
    // if(fileWriter.open(QIODevice::WriteOnly))
    // {
    //     fileWriter.write(key.toStdString().c_str());
    //     fileWriter.close();
    // }
    // else
    // {
    //     Message::show(licPath);
    //     // Message::show(QString::number(GetLastError()));
    // }
}

bool HxLicensing::IsRegistered()
{
    QString keyRef = KeyFromId( ID() );
    QString keyStore = ReadKey();
    return keyStore == keyRef;
}

QString HxLicensing::ID()
{
    return QSysInfo::machineUniqueId().toUpper() + "-R2024";
}

QString HxLicensing::KeyFromId( QString id )
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

bool HxLicensing::RegisterKey( QString key )
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
