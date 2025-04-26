#include "barcodesaver.h"
#include "settings.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QDateTime>

void BarcodeSaver::save(QString code){
    QString dir = QCoreApplication::applicationDirPath() + "/data/BARCODE-LOGS";
    QDir().mkdir(dir);

    QString fileName = QDateTime::currentDateTime().toString("yyyyMMdd") + ".csv";
    QString filePath = dir + "/" + fileName;

    QFile fileWriter(filePath);
    if(!fileWriter.exists()){
        if(fileWriter.open(QIODevice::WriteOnly)){
            fileWriter.write("TIME,CODE,");
            fileWriter.close();
        }
    }

    QString data = "\n" + QDateTime::currentDateTime().toString("HH:mm:ss") + "," + code + ",";
    if(fileWriter.open(QIODevice::WriteOnly| QIODevice::Append | QIODevice::Text)){
        fileWriter.write(data.toStdString().c_str(), data.length());
        fileWriter.close();
    }

}
