#include "settings.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>


QString Settings::password="Laser1";

QString Settings::laserPort = "COM1";
QString Settings::plcIp="192.168.0.11";
int Settings::plcPort = 8501;

QString Settings::conveyerReg = "DM230";
QString Settings::stopperReg = "MR30103";
QString Settings::passModeReg = "MR30107";


QString Settings::softwareReady = "MR30100";

QString Settings::laserTriggerReg = "MR30000";
QString Settings::laserTriggerConfirmReg = "MR30110";

QString Settings::laserStopReg = "MR30001";
QString Settings::laserStopConfirmReg = "MR30111";

QString Settings::barcodeReg = "MR30002";
QString Settings::barcodeConfirmReg = "MR30111";

QString Settings::barcodeDataReg = "DM350";
QString Settings::barcodeOKReg = "MR30105";
QString Settings::barcodeNGReg = "MR30106";

QString Settings::markResultReg = "MR30101";



void Settings::save(){
    QJsonObject obj;
    obj.insert("password", password);
    obj.insert("laser-dir", laserPort);
    obj.insert("plc-tcp-ip", plcIp);
    obj.insert("plc-tcp-port", plcPort);
    obj.insert("reg-cv-width", conveyerReg);
    obj.insert("reg-stopper", stopperReg);
    obj.insert("reg-pass-mode", passModeReg);
    obj.insert("reg-sw-ready", softwareReady);
    obj.insert("reg-trigger", laserTriggerReg);
    obj.insert("reg-trigger-confirm", laserTriggerConfirmReg);
    obj.insert("reg-laser-stop", laserStopReg);
    obj.insert("reg-laser-stop-confirm", laserStopConfirmReg);
    obj.insert("reg-barcode-trigger", barcodeReg);
    obj.insert("reg-barcode-confirm", barcodeConfirmReg);
    obj.insert("reg-barcode-data", barcodeDataReg);
    obj.insert("reg-barcode-status-ok", barcodeOKReg);
    obj.insert("reg-barcode-status-ng", barcodeNGReg);
    obj.insert("reg-mark-result", markResultReg);

    QJsonDocument doc;
    doc.setObject(obj);


    QString dir = QCoreApplication::applicationDirPath() + "/settings";
    QDir().mkdir(dir);
    QString path = dir + "/settings.json";
    QFile writer(path);
    if(writer.open(QIODevice::WriteOnly)){
        writer.write(doc.toJson());
        writer.close();
    }
}

void Settings::load(){
    QString dir = QCoreApplication::applicationDirPath() + "/settings";
    QDir().mkdir(dir);
    QString path = dir + "/settings.json";
    QFile reader(path);
    if(reader.open(QIODevice::ReadOnly)){
        QByteArray json = reader.readAll();
        reader.close();
        QJsonDocument doc = QJsonDocument::fromJson(json);
        QJsonObject obj = doc.object();
        password = obj.value("password").toString();
        laserPort = obj.value("laser-dir").toString();
        plcIp= obj.value("plc-tcp-ip").toString();
        plcPort = obj.value("plc-tcp-port").toInt(8501);
        conveyerReg= obj.value("reg-cv-width").toString();
        stopperReg= obj.value("reg-stopper").toString();
        passModeReg= obj.value("reg-pass-mode").toString();
        softwareReady= obj.value("reg-sw-ready").toString();
        laserTriggerReg= obj.value("reg-trigger").toString();
        laserTriggerConfirmReg= obj.value("reg-trigger-confirm").toString();
        laserStopReg= obj.value("reg-laser-stop").toString();
        laserStopConfirmReg= obj.value("reg-laser-stop-confirm").toString();
        barcodeReg= obj.value("reg-barcode-trigger").toString();
        barcodeConfirmReg= obj.value("reg-barcode-confirm").toString();
        barcodeDataReg= obj.value("reg-barcode-data").toString();
        barcodeOKReg= obj.value("reg-barcode-status-ok").toString();
        barcodeNGReg= obj.value("reg-barcode-status-ng").toString();
        markResultReg= obj.value("reg-mark-result").toString();
    }
    if(password.length() < 1) password = "Laser1";
    QDir().mkdir(QCoreApplication::applicationDirPath() + "/data");
}
