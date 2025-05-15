
#include "HxPLC.h"
#include "QString"
#include "QApplication"
#include "HxTcpSocket.h"
#include "HxSettings.h"
#include "HxMessage.h"
#include "HxEvent.h"
#include "HxException.h"

#define GetData(command) \
QString fb;\
try\
{\
HxTcpSocket sock( m_settings.String( PLCConnIPAddress ),m_settings.Int( PLCConnPort ) );\
sock.Connect();\
sock.WriteLine( command );\
fb = sock.ReadLine().trimmed();\
}\
catch(HxException ex)\
{\
ex.SetWhere("PLC");\
throw ex;\
}\

HxPLC::HxPLC()
{
    m_settings.Load();
}

bool HxPLC::SetEnable( bool en )
{
    QString val = en ? "1" : "0";
    QString cmd = "WR " +  m_settings.String( RegSoftwareReady ) + " " + val + "\r";
    //HxTcpSocket sock( m_settings.String( PLCConnIPAddress ),m_settings.Int( PLCConnPort ) );
    //sock.Connect();
    //sock.WriteLine( cmd );
    //QString fb = sock.ReadLine().trimmed();
    GetData( cmd );
    return fb == "OK";
}

bool HxPLC::SetMarkResult( bool status )
{
    QString val = status == true ? "1 0" : "0 1";
    QString cmd = "WRS " + m_settings.String( RegMarkingResult ) + " 2 " + val + "\r";
    //HxTcpSocket sock( m_settings.String( PLCConnIPAddress ), m_settings.Int( PLCConnPort ) );
    //sock.Connect();
    //sock.WriteLine( cmd );
    //QString fb = sock.ReadLine();
    GetData( cmd );
    return fb == "OK";
}

bool HxPLC::SetCvWidth( double width )
{
    int val = width * 100;
    QString cmd = "WR " + m_settings.String( RegCvWidth ) + " " + QString::number( val ) + "\r";
    //HxTcpSocket sock( m_settings.String( PLCConnIPAddress ), m_settings.Int( PLCConnPort ) );
    //sock.Connect();
    //sock.WriteLine( cmd );
    //QString fb = sock.ReadLine();
    GetData( cmd );
    return fb == "OK";
}

bool HxPLC::SetTransferMode( bool on )
{
    QString cmd = "WR " + m_settings.String( RegPassMode ) + " " + QString::number( on ) + "\r";
    //HxTcpSocket sock( m_settings.String( PLCConnIPAddress ), m_settings.Int( PLCConnPort ) );
    //sock.Connect();
    //sock.WriteLine( cmd );
    //QString fb = sock.ReadLine();
    GetData( cmd );
    return fb == "OK";
}

bool HxPLC::SetStopper( int stopper )
{
    QString val = stopper == 1 ? "1 0" : "0 1";
    QString cmd = "WRS " + m_settings.String( RegStopper ) + " 2 " + val + "\r";
    //HxTcpSocket sock( m_settings.String( PLCConnIPAddress ), m_settings.Int( PLCConnPort ) );
    //sock.Connect();
    //sock.WriteLine( cmd );
    //QString fb = sock.ReadLine();
    GetData( cmd );
    return fb == "OK";
}

bool HxPLC::IsHasTrigger()
{
    QString cmd = "RD " + m_settings.String( RegLaserTrigger ) + "\r";
    //HxTcpSocket sock( m_settings.String( PLCConnIPAddress ), m_settings.Int( PLCConnPort ) );
    //sock.Connect();
    //sock.WriteLine( cmd );
    //QString fb = sock.ReadLine();
    GetData( cmd );
    return fb == "1";
}

bool HxPLC::ConfirmTrigger()
{
    QString cmd = "WR " + m_settings.String( RegLaserTriggerConfirm ) + " 1\r\n";
    //HxTcpSocket sock( m_settings.String( PLCConnIPAddress ), m_settings.Int( PLCConnPort ) );
    //sock.Connect();
    //sock.WriteLine( cmd );
    //QString fb = sock.ReadLine();
    GetData( cmd );
    return fb == "OK";
}

bool HxPLC::SetCompleteBit()
{
    QString cmd = "WR MR30115 1\r\n";
    //HxTcpSocket sock( m_settings.String( PLCConnIPAddress ), m_settings.Int( PLCConnPort ) );
    //sock.Connect();
    //sock.WriteLine( cmd );
    //QString fb = sock.ReadLine();
    GetData( cmd );
    return fb == "OK";
}

void HxPLC::ReLoadSetting()
{
    m_settings.Load();
}

HxPLC* PLC()
{
    static HxPLC instance;
    return &instance;
}
