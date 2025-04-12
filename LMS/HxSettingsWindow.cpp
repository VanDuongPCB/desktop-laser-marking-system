#include "HxSettingsWindow.h"
#include "ui_HxSettingsWindow.h"
#include "HxAddProfileDialog.h"

#include "HxSettings.h"
#include "HxStopper.h"
#include "HxMarker.h"
#include "HxProfile.h"

#include "HxMessage.h"

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QFileDialog>

#include "HxFileManager.h"


HxSettingsWindow::HxSettingsWindow( QWidget* parent ) : QMainWindow( parent ), ui( new Ui::SettingsWindow )
{
    ui->setupUi( this );
    ui->tbvUsers->setHeaders( { "ID" } );
}

HxSettingsWindow::~HxSettingsWindow()
{
    delete ui;
}

void HxSettingsWindow::showEvent( QShowEvent* )
{
    showProtectSetting();
    showActuatorSettings();
    showLaserSettings();
    showStoppers();
    showUsers();
}

void HxSettingsWindow::showProtectSetting()
{
    ui->txtPassword->setText( GetFileManager()->GetSettings(HxFileManager::eSettingProtect)->value("Password").toString() );
}

void HxSettingsWindow::showActuatorSettings()
{
    ui->txtDataDir->setText( GetFileManager()->GetPath( HxFileManager::eDBRootDir ) );
    ui->txtPLCIP->setText( NxSettings::plcIp );
    ui->spxPLCPort->setValue( NxSettings::plcPort );

    ui->txtRegCvWidth->setText( NxSettings::conveyerReg );
    ui->txtRegStopper->setText( NxSettings::stopperReg );
    ui->txtRegPassMode->setText( NxSettings::passModeReg );
    ui->txtRegSWReady->setText( NxSettings::softwareReady );
    ui->txtRegTrigger->setText( NxSettings::laserTriggerReg );
    ui->txtRegTriggerConfirm->setText( NxSettings::laserTriggerConfirmReg );
    ui->txtRegStop->setText( NxSettings::laserStopReg );
    ui->txtRegStopConfirm->setText( NxSettings::laserStopConfirmReg );
    ui->txtRegBarCode->setText( NxSettings::barcodeReg );
    ui->txtRegBarcodeConfirm->setText( NxSettings::barcodeConfirmReg );
    ui->txtRegBarcodeData->setText( NxSettings::barcodeDataReg );
    ui->txtRegBarcodeOK->setText( NxSettings::barcodeOKReg );
    ui->txtRegBarcodeNG->setText( NxSettings::barcodeNGReg );
    ui->txtRegPrintRes->setText( NxSettings::markResultReg );
}

void HxSettingsWindow::showLaserSettings()
{
    on_btnEnum_clicked();
}

void HxSettingsWindow::showStoppers()
{
    if ( ui->tbvStoppers->headers.empty() )
    {
        ui->tbvStoppers->setHeaders( { "X","Y" } );
    }

    auto keys = HxStopper::items.keys();
    ui->tbvStoppers->setRowCount( keys.size() );
    for ( int row = 0; row < keys.size(); row++ )
    {
        ui->tbvStoppers->setText( row, "X", QString::number( HxStopper::items[ keys[ row ] ]->x, 'f', 2 ) );
        ui->tbvStoppers->setText( row, "Y", QString::number( HxStopper::items[ keys[ row ] ]->y, 'f', 2 ) );
    }
}

void HxSettingsWindow::showUsers()
{
    HxProfilePtrArray profiles = GetProfileManager()->GetProfiles();
    int rows = profiles.size();
    ui->tbvUsers->setRowCount( rows );
    for ( int row = 0; row < rows; row++ )
    {
        ui->tbvUsers->setText( row, 0, profiles[ row ]->name );
        ui->tbvUsers->setText( row, 1, "" );
        ui->tbvUsers->setText( row, 2, "" );
    }
}

void HxSettingsWindow::on_btnBrowseDataDir_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Chọn thư mục dữ liệu", ui->txtDataDir->text());
    if ( !dir.isEmpty() )
    {
        ui->txtDataDir->setText( dir );

    }
}

void HxSettingsWindow::on_actionSave_triggered()
{
    GetFileManager()->GetSettings(HxFileManager::eSettingData)->setValue("RootDataDir", ui->txtDataDir->text());
    GetFileManager()->GetSettings(HxFileManager::eSettingProtect)->setValue("Password", ui->txtPassword->text());

    NxSettings::plcIp = ui->txtPLCIP->text();
    NxSettings::plcPort = ui->spxPLCPort->value();
    NxSettings::laserPort = ui->cbxLaserPort->lineEdit()->text();

    NxSettings::conveyerReg = ui->txtRegCvWidth->text();
    NxSettings::stopperReg = ui->txtRegStopper->text();
    NxSettings::passModeReg = ui->txtRegPassMode->text();
    NxSettings::softwareReady = ui->txtRegSWReady->text();
    NxSettings::laserTriggerReg = ui->txtRegTrigger->text();
    NxSettings::laserTriggerConfirmReg = ui->txtRegTriggerConfirm->text();
    NxSettings::laserStopReg = ui->txtRegStop->text();
    NxSettings::laserStopConfirmReg = ui->txtRegStopConfirm->text();
    NxSettings::barcodeReg = ui->txtRegBarCode->text();
    NxSettings::barcodeConfirmReg = ui->txtRegBarcodeConfirm->text();
    NxSettings::barcodeDataReg = ui->txtRegBarcodeData->text();
    NxSettings::barcodeOKReg = ui->txtRegBarcodeOK->text();
    NxSettings::barcodeNGReg = ui->txtRegBarcodeNG->text();
    NxSettings::markResultReg = ui->txtRegPrintRes->text();
    NxSettings::save();

    int rows = ui->tbvStoppers->dataTable()->rowCount();
    for ( int row = 0; row < rows; row++ )
    {
        int index = row + 1;
        double x = ui->tbvStoppers->item( row, 0 )->text().toDouble();
        double y = ui->tbvStoppers->item( row, 1 )->text().toDouble();
        auto stp = HxStopper::find( index );
        if ( stp != nullptr )
        {
            stp->x = x;
            stp->y = y;
        }
    }
    HxStopper::save();

    HxMsgInfo( "Đã lưu cài đặt !", "Thông báo" );
}

void HxSettingsWindow::on_actionLoad_triggered()
{
    NxSettings::load();
    HxStopper::load();
    showActuatorSettings();
    showLaserSettings();
    showStoppers();
}

void HxSettingsWindow::on_btnAddUser_clicked()
{
    if ( HxAddProfileDialog( this ).exec() )
    {
        showUsers();
    }
}


void HxSettingsWindow::on_tbnRemoveUser_clicked()
{
    int row = ui->tbvUsers->currentIndex().row();
    if ( row < 0 ) return;
    QString userName = ui->tbvUsers->item(row,0)->text();
    GetProfileManager()->Remove(userName);
    showUsers();
}


void HxSettingsWindow::on_btnResetUser_clicked()
{
    int row = ui->tbvUsers->currentIndex().row();
    if ( row < 0 ) return;
    QString userName = ui->tbvUsers->item(row,0)->text();
    HxProfilePtr profile = GetProfileManager()->GetProfile(userName);
    if(!profile)
        return;
    profile->pass = "";
    GetProfileManager()->Save(profile);
}


void HxSettingsWindow::on_btnEnum_clicked()
{
    QList<QSerialPortInfo> portInfos = QSerialPortInfo::availablePorts();
    QStringList portnames;
    for ( auto& portinfo : portInfos )
    {
        portnames.push_back( portinfo.portName() );
    }
    if ( !portnames.contains( NxSettings::laserPort ) )
    {
        portnames.push_back( NxSettings::laserPort );
    }

    std::sort( portnames.begin(), portnames.end() );
    ui->cbxLaserPort->clear();
    ui->cbxLaserPort->addItem( "" );
    ui->cbxLaserPort->addItems( portnames );

    ui->cbxLaserPort->setCurrentText( NxSettings::laserPort );
}

