#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>

class Settings{
public:
    static QString password;
    static QString laserPort;
    static QString plcIp;
    static int plcPort;

    static QString conveyerReg;// = "DM230";
    static QString stopperReg;// = "MR30103";
    static QString passModeReg;// = "MR30107";
    static QString softwareReady;// = "MR30100";
    static QString laserTriggerReg;// = "MR30000";
    static QString laserTriggerConfirmReg;// = "MR30110";
    static QString laserStopReg;// = "MR30001";
    static QString laserStopConfirmReg;// = "MR30111";
    static QString barcodeReg;// = "MR30002";
    static QString barcodeConfirmReg;// = "MR30111";
    static QString barcodeDataReg;// = "DM350";
    static QString barcodeOKReg;// = "MR30105";
    static QString barcodeNGReg;// = "MR30106";
    static QString markResultReg;// = "MR30101";


    static void save();
    static void load();
};

#endif // SETTINGS_H
