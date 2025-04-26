#include "logsaver.h"
#include "model.h"
#include "lot.h"
#include "design.h"
#include "settings.h"
#include <QDir>
#include <QCoreApplication>

void LogSaver::save(std::shared_ptr<Lot> lot,std::shared_ptr<Model> model, std::shared_ptr<Design> design){
    if(lot == nullptr){
        return;
    }

    if(model == nullptr){
        return;
    }

    if(design == nullptr){
        return;
    }

    QString dir = QCoreApplication::applicationDirPath() + "/data/" + (lot->isRePrint ? "/REPRINT-LOGS" : "/PRINT-LOGS");
    QDir().mkdir(dir);

    QString fileName = QDateTime::currentDateTime().toString("yyyyMMdd") + ".csv";
    QString filePath = dir + "/" + fileName;

    QFile fileWriter(filePath);
    if(!fileWriter.exists()){
        if(fileWriter.open(QIODevice::WriteOnly)){
            fileWriter.write("TIME,LOT,MODEL,DESIGN,DATA-1,DATA-2,DATA-3,DATA-4,DATA-5,DATA-6,DATA-7,DATA-8,DATA-9,DATA-10");
            fileWriter.close();
        }
    }

    QStringList itemData;
    itemData.push_back("\n" + QDateTime::currentDateTime().toString("HH:mm:ss"));
    itemData.push_back(lot->name);
    itemData.push_back(model->name);
    itemData.push_back(design->name);



    for(int i=1;i<=10;i++){
        if(design->blocks.contains(i)){
            itemData.push_back(BlockInfo::gen(design->blocks[i].data, lot, model));
        }
        else{
            itemData.push_back("");
        }
    }

    QString data = itemData.join(",");
    if(fileWriter.open(QIODevice::WriteOnly| QIODevice::Append | QIODevice::Text)){
        fileWriter.write(data.toStdString().c_str(), data.length());
        fileWriter.close();
    }
}
