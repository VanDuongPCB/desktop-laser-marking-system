#include "controlwindow.h"
#include "ui_controlwindow.h"
#include "marker.h"
#include "laser.h"
#include "exception.h"
#include "systemerror.h"
#include "message.h"
#include <QFile>
#include <QMap>
#include "actuator.h"
#include "barcodereader.h"

ControlWindow::ControlWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::ControlWindow){
    ui->setupUi(this);
    on_btnBlockClear_clicked();
}

ControlWindow::~ControlWindow(){
    delete ui;
}

void ControlWindow::on_btnBarcodeRead_clicked(){
    ui->txtBarcodeData->setText("");
    try{
        bool bit = BarcodeReader::hasData();
        if(bit){
            BarcodeReader::clear();
            QString barcode = BarcodeReader::read();
            ui->txtBarcodeData->setText(barcode);
        }
    }
    catch(Exception ex){
        Message::error(ex.message);
    }
}

void ControlWindow::on_spxCvWidth_valueChanged(double arg1){
    try{
        Actuator::setCvWidth(arg1);
    }
    catch(Exception ex){
        Message::error(ex.message);
    }
}

void ControlWindow::on_cbxStopper_currentIndexChanged(int index){
    try{
        Actuator::setStopper(index);
    }
    catch(Exception ex){
        Message::error(ex.message);
    }
}

void ControlWindow::on_btnBlockClear_clicked(){
    if(ui->tbvMarkBlocks->headers.empty()){
        ui->tbvMarkBlocks->setHeaders({"Block","Giá trị"});
        int rows = 16;
        ui->tbvMarkBlocks->setRowCount(rows);
        for(int row = 0; row < rows; row++){
            ui->tbvMarkBlocks->setText(row,0, QString::number(row).rightJustified(3,'0'));
            ui->tbvMarkBlocks->item(row, 0)->setFlags(ui->tbvMarkBlocks->item(row,0)->flags() & ~Qt::ItemIsEditable);
            ui->tbvMarkBlocks->setText(row,1, "");
        }
    }
}

void ControlWindow::on_btnMark_clicked(){
//    QString program = QString::number(ui->spxProgram->value()).rightJustified(4,'0');
//    int stopper = ui->cbxStopper->currentIndex() + 1;
//    QMap<QString, QString> data;
//    int rows = 16;
//    for(int row = 0; row < rows; row++){
//        if(ui->tbvMarkBlocks->item(row,1) == nullptr) continue;
//        QString text = ui->tbvMarkBlocks->item(row,1)->text().trimmed();
//        if(text.length() < 1) continue;
//        QString block = ui->tbvMarkBlocks->item(row,0)->text();
//        data.insert(block, text);
//    }

//    try{
//        if(ui->chxChangeProgram->isChecked()){
//            Laser::setProgram(program);
//        }
//    }
//    catch(Exception ex){
//        Message::error(ex.message());
//    }




//    if(ui->chxChangeProgram->isChecked()){
////        Controller::instance()->setupLaserProgram(program);
//        try{
//            Laser::setProgram(program);
//        }
//        catch(std::exception ex){
//            std::cout << ex.what() << std::endl;
//        }
//    }

//    if(ui->chxChangePos->isChecked()){
//        Position pos;
//        pos.x = ui->spxMarkX->value();
//        pos.y = ui->spxMarkY->value();
//        pos.angle = ui->spxMarkAngle->value();
//        Marker::instance()->setupLaserPos(program,stopper, pos,26,7);
//    }

//    if(ui->chxChangeContent->isChecked()){
//        QMap<QString, QString> data;
//        int rows = 16;
//        for(int row = 0; row < rows; row++){
//            if(ui->tbvMarkBlocks->item(row,1) == nullptr) continue;
//            QString text = ui->tbvMarkBlocks->item(row,1)->text().trimmed();
//            if(text.length() < 1) continue;
//            QString block = ui->tbvMarkBlocks->item(row,0)->text();
//            data.insert(block, text);
//        }
//        Marker::instance()->setupLaserBlocks(program, data);
//    }

//    if(ui->chxPrint->isChecked()){
//        Marker::instance()->setupLaserOn();
//    }

}
