
#include "HxActuator.h"
#include "QString"
#include "HxTcpSocket.h"
#include "HxSettings.h"
#include "HxMessage.h"


bool HxActuator::SetEnable( bool en )
{
    QString val = en ? "1" : "0";
    QString cmd = "WR " + HxSettings::softwareReady + " " + val + "\r";
    HxTcpSocket sock( HxSettings::plcIp, HxSettings::plcPort );
    sock.Connect();
    sock.WriteLine( cmd );
    QString fb = sock.ReadLine().trimmed();
    return fb == "OK";
}

bool HxActuator::SetMarkResult( bool status )
{
    QString val = status == true ? "1 0" : "0 1";
    QString cmd = "WRS " + HxSettings::markResultReg + " 2 " + val + "\r";
    HxTcpSocket sock( HxSettings::plcIp, HxSettings::plcPort );
    sock.Connect();
    sock.WriteLine( cmd );
    QString fb = sock.ReadLine();
    return fb == "OK";
}

bool HxActuator::SetCvWidth( double width )
{
    int val = width * 100;
    QString cmd = "WR " + HxSettings::conveyerReg + " " + QString::number( val ) + "\r";
    HxTcpSocket sock( HxSettings::plcIp, HxSettings::plcPort );
    sock.Connect();
    sock.WriteLine( cmd );
    QString fb = sock.ReadLine();
    return fb == "OK";
}

bool HxActuator::SetTransferMode( bool on )
{
    QString cmd = "WR " + HxSettings::passModeReg + " " + QString::number( on ) + "\r";
    HxTcpSocket sock( HxSettings::plcIp, HxSettings::plcPort );
    sock.Connect();
    sock.WriteLine( cmd );
    QString fb = sock.ReadLine();
    return fb == "OK";
}

bool HxActuator::SetStopper( int stopper )
{
    QString val = stopper == 1 ? "1 0" : "0 1";
    QString cmd = "WRS " + HxSettings::stopperReg + " 2 " + val + "\r";
    HxTcpSocket sock( HxSettings::plcIp, HxSettings::plcPort );
    sock.Connect();
    sock.WriteLine( cmd );
    QString fb = sock.ReadLine();
    return fb == "OK";
}

bool HxActuator::IsHasTrigger()
{
    QString cmd = "RD " + HxSettings::laserTriggerReg + "\r";
    HxTcpSocket sock( HxSettings::plcIp, HxSettings::plcPort );
    sock.Connect();
    sock.WriteLine( cmd );
    QString fb = sock.ReadLine();
    return fb == "1";
}

bool HxActuator::ConfirmTrigger()
{
    QString cmd = "WR " + HxSettings::laserTriggerConfirmReg + " 1\r\n";
    HxTcpSocket sock( HxSettings::plcIp, HxSettings::plcPort );
    sock.Connect();
    sock.WriteLine( cmd );
    QString fb = sock.ReadLine();
    return fb == "OK";
}

bool HxActuator::SetCompleteBit()
{
    QString cmd = "WR MR30115 1\r\n";
    HxTcpSocket sock( HxSettings::plcIp, HxSettings::plcPort );
    sock.Connect();
    sock.WriteLine( cmd );
    QString fb = sock.ReadLine();
    return fb == "OK";
}
