#include "passwindow.h"
#include "ui_passwindow.h"
#include "model.h"
#include "marker.h"
#include "actuator.h"
#include "message.h"
#include "exception.h"
#include "systemerror.h"

PassWindow::PassWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::PassWindow){
    ui->setupUi(this);
    mainWindow = (MainWindow*) parent;
    connect(Marker::instance(), &Marker::started, this, &PassWindow::passStarted);
    connect(Marker::instance(), &Marker::stopped, this, &PassWindow::passStopped);
    on_btnReload_clicked();
}

PassWindow::~PassWindow(){
    delete ui;
}

void PassWindow::passStarted(){
    ui->cbxModel->setEnabled(false);
    ui->spxCvWidth->setEnabled(false);
    ui->btnPass->setEnabled(false);
    ui->btnStop->setEnabled(true);
}

void PassWindow::passStopped(){
    ui->cbxModel->setEnabled(true);
    ui->spxCvWidth->setEnabled(true);
    ui->btnPass->setEnabled(true);
    ui->btnStop->setEnabled(false);
}


void PassWindow::on_btnReload_clicked(){
    QString old = ui->cbxModel->currentText();
    ui->cbxModel->setEnabled(false);
    ui->cbxModel->clear();
    for(auto &md : Model::items){
        ui->cbxModel->addItem(md->name);
    }
    ui->cbxModel->setCurrentText(old);
    ui->cbxModel->setEnabled(true);
}


void PassWindow::on_spxCvWidth_valueChanged(double arg1){
    try{
        Actuator::setCvWidth(ui->spxCvWidth->value());
    }
    catch(Exception ex){
        SystemError::instance()->errorReport(ex);
    }
}


void PassWindow::on_cbxModel_currentTextChanged(const QString &arg1){
    if(ui->cbxModel->isEnabled()==false) return;
    auto model = Model::find(arg1);
    if(model!=nullptr){
        ui->spxCvWidth->setValue(model->cvWidth);
    }
}

void PassWindow::on_btnPass_clicked(){
    try{
        Actuator::setCvWidth(ui->spxCvWidth->value());
//        Actuator::setEnable(true);
        Actuator::setTransferMode(true);
        ui->btnPass->setEnabled(false);
        ui->btnStop->setEnabled(true);
        ui->cbxModel->setEnabled(false);
        ui->spxCvWidth->setEnabled(false);
        ui->btnReload->setEnabled(false);
        if(mainWindow != nullptr){
            mainWindow->setNavEnable(false);
        }
    }
    catch(Exception ex){
        SystemError::instance()->errorReport(ex);
    }
}


void PassWindow::on_btnStop_clicked(){
    try{
//        Actuator::setEnable(false);
        Actuator::setTransferMode(false);
        ui->btnPass->setEnabled(true);
        ui->btnStop->setEnabled(false);
        ui->cbxModel->setEnabled(true);
        ui->spxCvWidth->setEnabled(true);
        ui->btnReload->setEnabled(true);
        if(mainWindow != nullptr){
            mainWindow->setNavEnable(true);
        }
    }
    catch(Exception ex){
        SystemError::instance()->errorReport(ex);
    }
}

