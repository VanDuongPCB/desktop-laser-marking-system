
#include "actuator.h"
#include <QString>
#include "tcpsocket.h"
#include "settings.h"
#include "message.h"
#include "debugger.h"

bool Actuator::setEnable(bool en){
    QString val = en ? "1" : "0";
    QString cmd = "WR " + Settings::softwareReady + " " + val + "\r";
    TcpSocket sock(Settings::plcIp, Settings::plcPort);
    sock.connect();
    sock.writeLine(cmd);
    QString fb = sock.readLine().trimmed();
    return fb == "OK";
}

bool Actuator::setMarkResult(bool status){
    QString val = status == true ? "1 0" : "0 1";
    QString cmd = "WRS " + Settings::markResultReg + " 2 " + val + "\r";
    TcpSocket sock(Settings::plcIp, Settings::plcPort);
    sock.connect();
    sock.writeLine(cmd);
    QString fb = sock.readLine();
    return fb == "OK";
}

bool Actuator::setCvWidth(double width){
    int val = width * 100;
    QString cmd = "WR " + Settings::conveyerReg + " " + QString::number(val) + "\r";
    TcpSocket sock(Settings::plcIp, Settings::plcPort);
    sock.connect();
    sock.writeLine(cmd);
    QString fb = sock.readLine();
    return fb == "OK";
}

bool Actuator::setTransferMode(bool on){
    QString cmd = "WR " + Settings::passModeReg + " " + QString::number(on) + "\r";
    TcpSocket sock(Settings::plcIp, Settings::plcPort);
    sock.connect();
    sock.writeLine(cmd);
    QString fb = sock.readLine();
    return fb == "OK";
}

bool Actuator::setStopper(int stopper){
    QString val = stopper == 1 ? "1 0" : "0 1";
    QString cmd = "WRS " + Settings::stopperReg + " 2 " + val + "\r";
    TcpSocket sock(Settings::plcIp, Settings::plcPort);
    sock.connect();
    sock.writeLine(cmd);
    QString fb = sock.readLine();
    return fb == "OK";
}

bool Actuator::hasTrigger(){
    QString cmd = "RD " + Settings::laserTriggerReg + "\r";
    TcpSocket sock(Settings::plcIp, Settings::plcPort);
    sock.connect();
    sock.writeLine(cmd);
    QString fb = sock.readLine();
    return fb == "1";
}

bool Actuator::confirmTrigger(){
    QString cmd = "WR " + Settings::laserTriggerConfirmReg + " 1\r\n";
    TcpSocket sock(Settings::plcIp, Settings::plcPort);
    sock.connect();
    sock.writeLine(cmd);
    QString fb = sock.readLine();
    return fb == "OK";
}

bool Actuator::setCompleteBit(){
    QString cmd = "WR MR30115 1\r\n";
    TcpSocket sock(Settings::plcIp, Settings::plcPort);
    sock.connect();
    sock.writeLine(cmd);
    QString fb = sock.readLine();
    return fb == "OK";
}
