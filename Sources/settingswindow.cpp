#include "settingswindow.h"
#include "ui_settingswindow.h"
#include "adduserdialog.h"

#include "settings.h"
#include "stopper.h"
#include "marker.h"
#include "user.h"

#include "message.h"

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QFileDialog>

SettingsWindow::SettingsWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::SettingsWindow){
    ui->setupUi(this);
}

SettingsWindow::~SettingsWindow(){
    delete ui;
}

void SettingsWindow::showEvent(QShowEvent*){
    showProtectSetting();
    showActuatorSettings();
    showLaserSettings();
    showStoppers();
    showUsers();
}

void SettingsWindow::showProtectSetting(){
    ui->txtPassword->setText(Settings::password);
}

void SettingsWindow::showActuatorSettings(){
    ui->txtDataDir->setText(QCoreApplication::applicationDirPath() + "/data");
    ui->txtPLCIP->setText(Settings::plcIp);
    ui->spxPLCPort->setValue(Settings::plcPort);

    ui->txtRegCvWidth->setText(Settings::conveyerReg);
    ui->txtRegStopper->setText(Settings::stopperReg);
    ui->txtRegPassMode->setText(Settings::passModeReg);
    ui->txtRegSWReady->setText(Settings::softwareReady);
    ui->txtRegTrigger->setText(Settings::laserTriggerReg);
    ui->txtRegTriggerConfirm->setText(Settings::laserTriggerConfirmReg);
    ui->txtRegStop->setText(Settings::laserStopReg);
    ui->txtRegStopConfirm->setText(Settings::laserStopConfirmReg);
    ui->txtRegBarCode->setText(Settings::barcodeReg);
    ui->txtRegBarcodeConfirm->setText(Settings::barcodeConfirmReg);
    ui->txtRegBarcodeData->setText(Settings::barcodeDataReg);
    ui->txtRegBarcodeOK->setText(Settings::barcodeOKReg);
    ui->txtRegBarcodeNG->setText(Settings::barcodeNGReg);
    ui->txtRegPrintRes->setText(Settings::markResultReg);
}

void SettingsWindow::showLaserSettings(){
    on_btnEnum_clicked();
}

void SettingsWindow::showStoppers(){
    if(ui->tbvStoppers->headers.empty()){
        ui->tbvStoppers->setHeaders({"X","Y"});
    }

    auto keys = Stopper::items.keys();
    ui->tbvStoppers->setRowCount(keys.size());
    for(int row = 0; row < keys.size(); row++){
        ui->tbvStoppers->setText(row,"X", QString::number(Stopper::items[keys[row]]->x,'f',2));
        ui->tbvStoppers->setText(row,"Y", QString::number(Stopper::items[keys[row]]->y,'f',2));
    }
}

void SettingsWindow::showUsers(){
    if(ui->tbvUsers->headers.empty()){
        ui->tbvUsers->setHeaders({"ID"});
    }
    int rows = User::items.size();
    ui->tbvUsers->setRowCount(rows);
    for(int row = 0; row < rows; row ++){
        ui->tbvUsers->setText(row,0, User::items[row]->name);
        ui->tbvUsers->setText(row,1, "");
        ui->tbvUsers->setText(row,2, "");
    }
}

void SettingsWindow::on_btnBrowseDataDir_clicked(){
    QString dir = QFileDialog::getExistingDirectory();
    if(dir.length()>1){
        ui->txtDataDir->setText(dir);
    }
}

void SettingsWindow::on_actionSave_triggered(){
    Settings::password = ui->txtPassword->text();


    Settings::plcIp = ui->txtPLCIP->text();
    Settings::plcPort = ui->spxPLCPort->value();
    Settings::laserPort = ui->cbxLaserPort->lineEdit()->text();

    Settings::conveyerReg=ui->txtRegCvWidth->text();
    Settings::stopperReg=ui->txtRegStopper->text();
    Settings::passModeReg=ui->txtRegPassMode->text();
    Settings::softwareReady=ui->txtRegSWReady->text();
    Settings::laserTriggerReg=ui->txtRegTrigger->text();
    Settings::laserTriggerConfirmReg=ui->txtRegTriggerConfirm->text();
    Settings::laserStopReg=ui->txtRegStop->text();
    Settings::laserStopConfirmReg=ui->txtRegStopConfirm->text();
    Settings::barcodeReg=ui->txtRegBarCode->text();
    Settings::barcodeConfirmReg=ui->txtRegBarcodeConfirm->text();
    Settings::barcodeDataReg=ui->txtRegBarcodeData->text();
    Settings::barcodeOKReg=ui->txtRegBarcodeOK->text();
    Settings::barcodeNGReg=ui->txtRegBarcodeNG->text();
    Settings::markResultReg=ui->txtRegPrintRes->text();
    Settings::save();

    int rows = ui->tbvStoppers->dataTable()->rowCount();
    for(int row = 0; row < rows; row++){
        int index = row + 1;
        double x = ui->tbvStoppers->item(row,0)->text().toDouble();
        double y = ui->tbvStoppers->item(row,1)->text().toDouble();
        auto stp = Stopper::find(index);
        if(stp != nullptr){
            stp->x = x;
            stp->y = y;
        }
    }
    Stopper::save();

    User::save();

    Message::show("Đã lưu cài đặt !","Thông báo");
}

void SettingsWindow::on_actionLoad_triggered(){
    Settings::load();
    Stopper::load();
    User::load();
    showActuatorSettings();
    showLaserSettings();
    showStoppers();
}

void SettingsWindow::on_btnAddUser_clicked(){
    if(AddUserDialog(this).exec()){
        showUsers();
    }
}


void SettingsWindow::on_tbnRemoveUser_clicked(){
    int row= ui->tbvUsers->currentIndex().row();
    if(row<0) return;
    User::items.erase(User::items.begin() + row);
    showUsers();
}


void SettingsWindow::on_btnResetUser_clicked(){
    int row= ui->tbvUsers->currentIndex().row();
    if(row<0) return;
    User::items[row]->pass="";
}


void SettingsWindow::on_btnEnum_clicked(){
    QList<QSerialPortInfo> portInfos = QSerialPortInfo::availablePorts();
    QStringList portnames;
    for(auto &portinfo : portInfos){
        portnames.push_back(portinfo.portName());
    }
    if(!portnames.contains(Settings::laserPort)){
        portnames.push_back(Settings::laserPort);
    }

    std::sort(portnames.begin(), portnames.end());
    ui->cbxLaserPort->clear();
    ui->cbxLaserPort->addItem("");
    ui->cbxLaserPort->addItems(portnames);

    ui->cbxLaserPort->setCurrentText(Settings::laserPort);
}

