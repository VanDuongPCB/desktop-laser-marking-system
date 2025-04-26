#include "license.h"
#include <QSysInfo>
#include <QCoreApplication>
#include <QFile>
#include "message.h"
#include "QSettings"

QString License::readKey(){
    QSettings settings("HKEY_CURRENT_USER\\Software\\Laser Marker System\\License", QSettings::NativeFormat);
    QString key = settings.value("Key").toString();
    return key;
    // QFile fileReader(QCoreApplication::applicationDirPath() + "/license.lic");
    // if(fileReader.open(QIODevice::ReadOnly)){
    //     QString key = fileReader.readAll();
    //     fileReader.close();
    //     return key;
    // }
    // return "";
}

void License::writeKey(QString key)
{
    QSettings settings("HKEY_CURRENT_USER\\Software\\Laser Marker System\\License", QSettings::NativeFormat);
    settings.setValue("Key",key);
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

bool License::isRegistered(){
    QString keyRef = keyFromId(id());
    QString keyStore = readKey();
    return keyStore == keyRef;
}

QString License::id(){
    return QSysInfo::machineUniqueId().toUpper() + "-R2024";
}

QString License::keyFromId(QString id){
    std::string sid = id.toStdString();
    std::vector<char> buff;
    for(int i=0;i<sid.length();i++){
        if(i>0 && i%5==0){
            buff.push_back('-');
        }

        int val = sid[i] * 69 + i;
        val = val % 36;
        if(val < 10){
            buff.push_back('0' + val);
        }
        else{
            buff.push_back(val - 10 + 'A');
        }
    }
    buff.push_back(0);
    return QString::fromStdString(buff.data());
}

bool License::registerKey(QString key){
    QString keyRef = keyFromId(id());
    if(keyRef == key){
        writeKey(key);
        return true;
    }
    else{
        return false;
    }
}
