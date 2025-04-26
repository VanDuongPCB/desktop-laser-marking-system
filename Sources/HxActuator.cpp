
#include "HxActuator.h"
#include <QString>
#include "HxTcpSocket.h"
#include "HxSettings.h"
#include "HxMessage.h"


bool HxActuator::setEnable( bool en )
{
    QString val = en ? "1" : "0";
    QString cmd = "WR " + HxSettings::softwareReady + " " + val + "\r";
    HxTcpSocket sock( HxSettings::plcIp, HxSettings::plcPort );
    sock.connect();
    sock.writeLine( cmd );
    QString fb = sock.readLine().trimmed();
    return fb == "OK";
}

bool HxActuator::setMarkResult( bool status )
{
    QString val = status == true ? "1 0" : "0 1";
    QString cmd = "WRS " + HxSettings::markResultReg + " 2 " + val + "\r";
    HxTcpSocket sock( HxSettings::plcIp, HxSettings::plcPort );
    sock.connect();
    sock.writeLine( cmd );
    QString fb = sock.readLine();
    return fb == "OK";
}

bool HxActuator::setCvWidth( double width )
{
    int val = width * 100;
    QString cmd = "WR " + HxSettings::conveyerReg + " " + QString::number( val ) + "\r";
    HxTcpSocket sock( HxSettings::plcIp, HxSettings::plcPort );
    sock.connect();
    sock.writeLine( cmd );
    QString fb = sock.readLine();
    return fb == "OK";
}

bool HxActuator::setTransferMode( bool on )
{
    QString cmd = "WR " + HxSettings::passModeReg + " " + QString::number( on ) + "\r";
    HxTcpSocket sock( HxSettings::plcIp, HxSettings::plcPort );
    sock.connect();
    sock.writeLine( cmd );
    QString fb = sock.readLine();
    return fb == "OK";
}

bool HxActuator::setStopper( int stopper )
{
    QString val = stopper == 1 ? "1 0" : "0 1";
    QString cmd = "WRS " + HxSettings::stopperReg + " 2 " + val + "\r";
    HxTcpSocket sock( HxSettings::plcIp, HxSettings::plcPort );
    sock.connect();
    sock.writeLine( cmd );
    QString fb = sock.readLine();
    return fb == "OK";
}

bool HxActuator::hasTrigger()
{
    QString cmd = "RD " + HxSettings::laserTriggerReg + "\r";
    HxTcpSocket sock( HxSettings::plcIp, HxSettings::plcPort );
    sock.connect();
    sock.writeLine( cmd );
    QString fb = sock.readLine();
    return fb == "1";
}

bool HxActuator::confirmTrigger()
{
    QString cmd = "WR " + HxSettings::laserTriggerConfirmReg + " 1\r\n";
    HxTcpSocket sock( HxSettings::plcIp, HxSettings::plcPort );
    sock.connect();
    sock.writeLine( cmd );
    QString fb = sock.readLine();
    return fb == "OK";
}

bool HxActuator::setCompleteBit()
{
    QString cmd = "WR MR30115 1\r\n";
    HxTcpSocket sock( HxSettings::plcIp, HxSettings::plcPort );
    sock.connect();
    sock.writeLine( cmd );
    QString fb = sock.readLine();
    return fb == "OK";
}
