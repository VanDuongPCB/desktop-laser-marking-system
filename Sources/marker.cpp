#include "marker.h"
#include "stopper.h"
#include <QTimer>
#include <QSerialPort>
#include <QTcpSocket>
#include <iostream>
#include "barcodesaver.h"
#include "logsaver.h"
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>



#include "message.h"


#include "laser.h"
#include "actuator.h"
#include "stopper.h"
#include "barcodereader.h"

#include "exception.h"
#include "systemerror.h"


Marker::Marker(QObject *parent) : QObject(parent){
    runFlag = false;
}

Marker::~Marker(){

}

void Marker::clear(){
    lot.reset();
    model.reset();
    design.reset();
    stopper.reset();
}

bool Marker::select(std::shared_ptr<Lot> lotinf){
    if(lotinf == nullptr){
        Message::error("Dữ liệu lot không hợp lệ !");
        clear();
        return false;
    }

    if(lotinf->isCompleted()){
        Message::error("Lot này đã hoàn thành !");
        clear();
        return false;
    }

    auto _model = Model::find(lotinf->modelName);
    if(_model == nullptr){
        Message::error("Không tìm thấy thông tin model : " + lotinf->modelName);
        clear();
        return false;
    }

    auto _design = Design::find(_model->design);
    if(_design == nullptr){
        Message::error("Không tìm thấy thông tin thiết kế : " + _model->design);
        clear();
        return false;
    }

    auto _stopper = Stopper::find(_model->stopper);
    if(_stopper == nullptr){
        Message::error("Không tìm thấy thông tin stopper : " + QString::number(_model->stopper));
        clear();
        return false;
    }

    // try setup
    try{
        Laser::setProgram(_design->name);
        Actuator::setCvWidth(_model->cvWidth);
        Actuator::setStopper(_model->stopper);

        lot = lotinf;
        model = _model;
        design = _design;
        stopper = _stopper;
        return true;
    }
    catch(Exception ex){
        SystemError::instance()->errorReport(ex);
//        Message::error(ex.message);
        clear();
        return false;
    }
}

bool Marker::mark(bool test){
    auto tempLot = std::make_shared<Lot>(Lot());
    tempLot.get()[0] = lot.get()[0];
    int patternCnt = model->positions.size();

    if(test){
        for(int i = 0; i < patternCnt;i++){
            Position pos = model->positions[i];
            QMap<int, QString> blockDatas = BlockInfo::gen(design, tempLot, model);


            Laser::setupPosition(design->name, pos, model->stopper, design.get()[0]);
            Laser::setupBlockData(design->name, blockDatas);
            Laser::burn();
            tempLot->progress++;
        }
    }
    else{
        for(int i = 0; i < patternCnt;i++){
            if(tempLot->isCompleted()) continue;
            Position pos = model->positions[i];
            QMap<int, QString> blockDatas = BlockInfo::gen(design, tempLot, model);

            Laser::setupPosition(design->name, pos, model->stopper, design.get()[0]);
            Laser::setupBlockData(design->name, blockDatas);
            Laser::burn();
            LogSaver::save(tempLot, model, design);
            tempLot->progress++;
        }
        lot.get()[0] = tempLot.get()[0];
        Lot::saveLot(lot);
    }
    return true;
}


bool Marker::isBusy(){
    return runFlag;
}

void Marker::start(){
    if(runFlag){
        return;
    }

    QMetaObject::invokeMethod(this,"task");
}

void Marker::stop(){
    runFlag = false;
}

void Marker::task(){
    emit started();
    runFlag = true;
    QThread::msleep(500);
    while(true){
        if(!runFlag)
            break;
        QThread::msleep(50);

        try{
            if(BarcodeReader::hasData()){
                BarcodeReader::clear();
                QString code = BarcodeReader::read().trimmed();
                if(code.length() > 0 && code.startsWith("ERROR") == false){
                    BarcodeSaver::save(code);
                    BarcodeReader::sendFeedback(true);
                }
                else{
                    BarcodeReader::sendFeedback(false);
                }
            }
        }
        catch(Exception ex){
            ex.where = "Đọc barcode";
            SystemError::instance()->errorReport(ex);

            Exception ex2;
            ex2.where = "Khắc";
            ex2.message = "Quá trình khắc đã dừng lại do phát hiện lỗi!";
            SystemError::instance()->errorReport(ex2);
            runFlag = false;
            break;
        }

        try{
            if(Actuator::hasTrigger()){
                Actuator::confirmTrigger();
                bool status = mark(false);
                Actuator::setMarkResult(status);
                if(lot->isCompleted()){
                    Actuator::setCompleteBit();
                }
                emit printed(lot);
            }
        }
        catch(Exception ex){
            ex.where = "Khắc";
            SystemError::instance()->errorReport(ex);

            Exception ex2;
            ex2.where = "Khắc";
            ex2.message = "Quá trình khắc đã dừng lại do phát hiện lỗi!";
            SystemError::instance()->errorReport(ex2);

            runFlag = false;
            break;
        }
    }
    runFlag = false;
    emit stopped();
}

/* ------------------------------------------------------ */
QThread Marker::worker;
Marker *Marker::_instance = nullptr;

void Marker::initialize(){
    worker.start();
}
void Marker::terminate(){
    worker.quit();
}

Marker *Marker::instance(){
    if(_instance == nullptr){
        _instance = new Marker();
        _instance->moveToThread(&worker);
    }
    return _instance;
}
