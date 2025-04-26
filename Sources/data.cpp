#include "data.h"
#include "settings.h"

#include <QString>
#include <QStringList>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <iostream>


Lot* Data::_lot = nullptr;
Model* Data::_model = nullptr;

QMutex Data::mutex;


Lot* Data::lot(){
    return _lot;
}

Model* Data::model(){
    return _model;
}


void Data::clear(){
    mutex.lock();
    _lot = nullptr;
    _model = nullptr;
    mutex.unlock();
}

void Data::select(QString lotName){

}

void Data::lock(){
    mutex.lock();
}

void Data::unlock(){
    mutex.unlock();
}
