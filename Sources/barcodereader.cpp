
#include "barcodereader.h"
#include "settings.h"
#include "tcpsocket.h"
#include "message.h"

bool BarcodeReader::hasData(){
    QString cmd = "RD " + Settings::barcodeReg + "\r";
    TcpSocket sock(Settings::plcIp, Settings::plcPort);
    sock.connect();
    sock.writeLine(cmd,2000);
    QString fb = sock.readLine(2000);
    return fb == "1";
}

bool BarcodeReader::clear(){
    QString cmd = "WR " + Settings::laserTriggerConfirmReg + " 1\r";
    TcpSocket sock(Settings::plcIp, Settings::plcPort);
    sock.connect();
    sock.writeLine(cmd,2000);
    QString fb = sock.readLine(2000);
    return fb == "OK";
}

QString BarcodeReader::read(){
    QString cmd = "RDS " + Settings::barcodeDataReg + " 40\r";
    TcpSocket sock(Settings::plcIp, Settings::plcPort);
    sock.connect();
    sock.writeLine(cmd,2000);
    QString fb = sock.readLine(2000);
    QStringList codeItems = fb.split(' ');
    if(codeItems.size() < 1) return "";
    std::vector<wchar_t> buffer(codeItems.size()+1,0);
    for(int i=0;i<codeItems.size();i++){
        int val = codeItems[i].toInt(0);
        buffer[i] = ((val >> 8) | (val << 8)) & 0xFFFF;
    }

    char *ptr = (char*)buffer.data();

    return QString::fromStdString(ptr);
}

bool BarcodeReader::sendFeedback(bool status){
    QString val1 = status == 1 ? "1" : "0";
    QString val2 = status == 1 ? "0" : "1";
    QString cmd = "WRS " + Settings::barcodeOKReg + " 2 " + val1 + " " + val2 + "\r";
    TcpSocket sock(Settings::plcIp, Settings::plcPort);
    sock.connect();
    sock.writeLine(cmd,2000);
    QString fb = sock.readLine(2000);
    cmd = "WR " + Settings::barcodeConfirmReg + " 1\r";
    sock.writeLine(cmd,2000);
    fb = sock.readLine(2000);
    return fb == "OK";
}
