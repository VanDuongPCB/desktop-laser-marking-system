
#include "HxPLC.h"
#include "QString"
#include "HxTcpSocket.h"
#include "HxSettings.h"
#include "HxMessage.h"
#include "HxDebugger.h"

namespace
{
    HxPLC s_instance;
}

bool HxPLC::setEnable( bool en )
{
    QString val = en ? "1" : "0";
    QString cmd = "WR " + NxSettings::softwareReady + " " + val + "\r";
    TcpSocket sock( NxSettings::plcIp, NxSettings::plcPort );
    sock.connect();
    sock.writeLine( cmd );
    QString fb = sock.readLine().trimmed();
    return fb == "OK";
}

bool HxPLC::setMarkResult( bool status )
{
    QString val = status == true ? "1 0" : "0 1";
    QString cmd = "WRS " + NxSettings::markResultReg + " 2 " + val + "\r";
    TcpSocket sock( NxSettings::plcIp, NxSettings::plcPort );
    sock.connect();
    sock.writeLine( cmd );
    QString fb = sock.readLine();
    return fb == "OK";
}

bool HxPLC::setCvWidth( double width )
{
    int val = width * 100;
    QString cmd = "WR " + NxSettings::conveyerReg + " " + QString::number( val ) + "\r";
    TcpSocket sock( NxSettings::plcIp, NxSettings::plcPort );
    sock.connect();
    sock.writeLine( cmd );
    QString fb = sock.readLine();
    return fb == "OK";
}

bool HxPLC::setTransferMode( bool on )
{
    QString cmd = "WR " + NxSettings::passModeReg + " " + QString::number( on ) + "\r";
    TcpSocket sock( NxSettings::plcIp, NxSettings::plcPort );
    sock.connect();
    sock.writeLine( cmd );
    QString fb = sock.readLine();
    return fb == "OK";
}

bool HxPLC::setStopper( int stopper )
{
    QString val = stopper == 1 ? "1 0" : "0 1";
    QString cmd = "WRS " + NxSettings::stopperReg + " 2 " + val + "\r";
    TcpSocket sock( NxSettings::plcIp, NxSettings::plcPort );
    sock.connect();
    sock.writeLine( cmd );
    QString fb = sock.readLine();
    return fb == "OK";
}

bool HxPLC::hasTrigger()
{
    QString cmd = "RD " + NxSettings::laserTriggerReg + "\r";
    TcpSocket sock( NxSettings::plcIp, NxSettings::plcPort );
    sock.connect();
    sock.writeLine( cmd );
    QString fb = sock.readLine();
    return fb == "1";
}

bool HxPLC::confirmTrigger()
{
    QString cmd = "WR " + NxSettings::laserTriggerConfirmReg + " 1\r\n";
    TcpSocket sock( NxSettings::plcIp, NxSettings::plcPort );
    sock.connect();
    sock.writeLine( cmd );
    QString fb = sock.readLine();
    return fb == "OK";
}

bool HxPLC::setCompleteBit()
{
    QString cmd = "WR MR30115 1\r\n";
    TcpSocket sock( NxSettings::plcIp, NxSettings::plcPort );
    sock.connect();
    sock.writeLine( cmd );
    QString fb = sock.readLine();
    return fb == "OK";
}


HxPLC* GetPLC()
{
    return &s_instance;
}
