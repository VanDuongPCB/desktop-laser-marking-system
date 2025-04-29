#include "HxSettingWindow.h"
#include "ui_hxsettingwindow.h"
#include "HxAddUserDialog.h"

#include "HxSettings.h"
#include "HxStopper.h"
#include "HxMarker.h"
#include "HxUserProfile.h"

#include "HxMessage.h"

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QFileDialog>

HxSettingsWindow::HxSettingsWindow( QWidget* parent ) : QMainWindow( parent ), ui( new Ui::SettingWindow )
{
    ui->setupUi( this );
}

HxSettingsWindow::~HxSettingsWindow()
{
    delete ui;
}

void HxSettingsWindow::showEvent( QShowEvent* )
{
    ShowProtectSetting();
    ShowActuatorSettings();
    ShowLaserSettings();
    ShowStoppers();
    ShowUsers();
}

void HxSettingsWindow::ShowProtectSetting()
{
    ui->txtPassword->setText( HxSettings::password );
}

void HxSettingsWindow::ShowActuatorSettings()
{
    ui->txtDataDir->setText( QCoreApplication::applicationDirPath() + "/data" );
    ui->txtPLCIP->setText( HxSettings::plcIp );
    ui->spxPLCPort->setValue( HxSettings::plcPort );

    ui->txtRegCvWidth->setText( HxSettings::conveyerReg );
    ui->txtRegStopper->setText( HxSettings::stopperReg );
    ui->txtRegPassMode->setText( HxSettings::passModeReg );
    ui->txtRegSWReady->setText( HxSettings::softwareReady );
    ui->txtRegTrigger->setText( HxSettings::laserTriggerReg );
    ui->txtRegTriggerConfirm->setText( HxSettings::laserTriggerConfirmReg );
    ui->txtRegStop->setText( HxSettings::laserStopReg );
    ui->txtRegStopConfirm->setText( HxSettings::laserStopConfirmReg );
    ui->txtRegBarCode->setText( HxSettings::barcodeReg );
    ui->txtRegBarcodeConfirm->setText( HxSettings::barcodeConfirmReg );
    ui->txtRegBarcodeData->setText( HxSettings::barcodeDataReg );
    ui->txtRegBarcodeOK->setText( HxSettings::barcodeOKReg );
    ui->txtRegBarcodeNG->setText( HxSettings::barcodeNGReg );
    ui->txtRegPrintRes->setText( HxSettings::markResultReg );
}

void HxSettingsWindow::ShowLaserSettings()
{
    on_btnEnum_clicked();
}

void HxSettingsWindow::ShowStoppers()
{
    if ( ui->tbvStoppers->headers.empty() )
    {
        ui->tbvStoppers->SetHeaders( { "X","Y" } );
    }

    auto keys = HxStopper::items.keys();
    ui->tbvStoppers->SetRowCount( keys.size() );
    for ( int row = 0; row < keys.size(); row++ )
    {
        ui->tbvStoppers->SetText( row, "X", QString::number( HxStopper::items[ keys[ row ] ]->x, 'f', 2 ) );
        ui->tbvStoppers->SetText( row, "Y", QString::number( HxStopper::items[ keys[ row ] ]->y, 'f', 2 ) );
    }
}

void HxSettingsWindow::ShowUsers()
{
    if ( ui->tbvUsers->headers.empty() )
    {
        ui->tbvUsers->SetHeaders( { "ID" } );
    }
    int rows = HxUserProfile::items.size();
    ui->tbvUsers->SetRowCount( rows );
    for ( int row = 0; row < rows; row++ )
    {
        ui->tbvUsers->SetText( row, 0, HxUserProfile::items[ row ]->name );
        ui->tbvUsers->SetText( row, 1, "" );
        ui->tbvUsers->SetText( row, 2, "" );
    }
}

void HxSettingsWindow::on_btnBrowseDataDir_clicked()
{
    QString dir = QFileDialog::getExistingDirectory();
    if ( dir.length() > 1 )
    {
        ui->txtDataDir->setText( dir );
    }
}

void HxSettingsWindow::on_actionSave_triggered()
{
    HxSettings::password = ui->txtPassword->text();


    HxSettings::plcIp = ui->txtPLCIP->text();
    HxSettings::plcPort = ui->spxPLCPort->value();
    HxSettings::laserPort = ui->cbxLaserPort->lineEdit()->text();

    HxSettings::conveyerReg = ui->txtRegCvWidth->text();
    HxSettings::stopperReg = ui->txtRegStopper->text();
    HxSettings::passModeReg = ui->txtRegPassMode->text();
    HxSettings::softwareReady = ui->txtRegSWReady->text();
    HxSettings::laserTriggerReg = ui->txtRegTrigger->text();
    HxSettings::laserTriggerConfirmReg = ui->txtRegTriggerConfirm->text();
    HxSettings::laserStopReg = ui->txtRegStop->text();
    HxSettings::laserStopConfirmReg = ui->txtRegStopConfirm->text();
    HxSettings::barcodeReg = ui->txtRegBarCode->text();
    HxSettings::barcodeConfirmReg = ui->txtRegBarcodeConfirm->text();
    HxSettings::barcodeDataReg = ui->txtRegBarcodeData->text();
    HxSettings::barcodeOKReg = ui->txtRegBarcodeOK->text();
    HxSettings::barcodeNGReg = ui->txtRegBarcodeNG->text();
    HxSettings::markResultReg = ui->txtRegPrintRes->text();
    HxSettings::Save();

    int rows = ui->tbvStoppers->DataTable()->rowCount();
    for ( int row = 0; row < rows; row++ )
    {
        int index = row + 1;
        double x = ui->tbvStoppers->Item( row, 0 )->text().toDouble();
        double y = ui->tbvStoppers->Item( row, 1 )->text().toDouble();
        auto stp = HxStopper::Find( index );
        if ( stp != nullptr )
        {
            stp->x = x;
            stp->y = y;
        }
    }
    HxStopper::Save();

    HxUserProfile::Save();

    HxMessage::show( "Đã lưu cài đặt !", "Thông báo" );
}

void HxSettingsWindow::on_actionLoad_triggered()
{
    HxSettings::Load();
    HxStopper::Load();
    HxUserProfile::Load();
    ShowActuatorSettings();
    ShowLaserSettings();
    ShowStoppers();
}

void HxSettingsWindow::on_btnAddUser_clicked()
{
    if ( HxAddUserDialog( this ).exec() )
    {
        ShowUsers();
    }
}


void HxSettingsWindow::on_tbnRemoveUser_clicked()
{
    int row = ui->tbvUsers->currentIndex().row();
    if ( row < 0 ) return;
    HxUserProfile::items.erase( HxUserProfile::items.begin() + row );
    ShowUsers();
}


void HxSettingsWindow::on_btnResetUser_clicked()
{
    int row = ui->tbvUsers->currentIndex().row();
    if ( row < 0 ) return;
    HxUserProfile::items[ row ]->pass = "";
}


void HxSettingsWindow::on_btnEnum_clicked()
{
    QList<QSerialPortInfo> portInfos = QSerialPortInfo::availablePorts();
    QStringList portnames;
    for ( auto& portinfo : portInfos )
    {
        portnames.push_back( portinfo.portName() );
    }
    if ( !portnames.contains( HxSettings::laserPort ) )
    {
        portnames.push_back( HxSettings::laserPort );
    }

    std::sort( portnames.begin(), portnames.end() );
    ui->cbxLaserPort->clear();
    ui->cbxLaserPort->addItem( "" );
    ui->cbxLaserPort->addItems( portnames );

    ui->cbxLaserPort->setCurrentText( HxSettings::laserPort );
}

