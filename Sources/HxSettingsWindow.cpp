#include "HxSettingsWindow.h"
#include "ui_hxsettingswindow.h"
#include "QLineEdit"

#include "HxSettings.h"
#include "HxStopper.h"
#include "HxMarker.h"
#include "HxProfile.h"

#include "HxMessage.h"

#include "QSerialPort"
#include "QSerialPortInfo"
#include "QFileDialog"

#include "HxFileManager.h"
#include "HxEvent.h"


HxSettingsWindow::HxSettingsWindow( QWidget* parent ) : QMainWindow( parent ), ui( new Ui::SettingsWindow )
{
    ui->setupUi( this );

    ui->tbvStoppers->setHeaders( { "X","Y" } );

    connect( ui->actionLoad, &QAction::triggered, this, &HxSettingsWindow::OnLoad );
    connect( ui->actionSave, &QAction::triggered, this, &HxSettingsWindow::OnSave );
    connect( ui->btnBrowseData, &QPushButton::clicked, this, &HxSettingsWindow::OnBrowseDataDir );
    connect( ui->btnBrowseIV, &QPushButton::clicked, this, &HxSettingsWindow::OnBrowseIVDir );
    connect( ui->btnEnumCOM, &QPushButton::clicked, this, &HxSettingsWindow::OnEnumerateLaserPorts );
}

HxSettingsWindow::~HxSettingsWindow()
{
    delete ui;
}

void HxSettingsWindow::showEvent( QShowEvent* )
{
    OnLoad();
}

bool HxSettingsWindow::eventFilter( QObject* watched, QEvent* event )
{
    return QMainWindow::eventFilter( watched, event );
}

void HxSettingsWindow::Show()
{
    ui->txtAdminPassword->setText( m_regSettings.String( AdminPassword ) );
    ui->txtSuperPassword->setText( m_regSettings.String( SuperPassword ) );
    ui->txtLeaderPassword->setText( m_regSettings.String( LeaderPassword ) );
    ui->txtDatabasePath->setText( m_regSettings.String( DatabaseFilePath ) );
    ui->txtIVDir->setText( m_regSettings.String( IVProgramDir ) );

    ui->cbxLaserPort->setCurrentText( m_regSettings.String( LaserConnPort ) );

    ui->txtPLCIP->setText( m_regSettings.String( PLCConnIPAddress ) );
    ui->spxPLCPort->setValue( m_regSettings.Int( PLCConnPort ) );

    ui->txtRegCvWidth->setText( m_regSettings.String( RegCvWidth) );
    ui->txtRegStopper->setText( m_regSettings.String( RegStopper ) );
    ui->txtRegPassMode->setText( m_regSettings.String( RegPassMode ) );
    ui->txtRegSWReady->setText( m_regSettings.String( RegSoftwareReady ) );
    ui->txtRegTrigger->setText( m_regSettings.String( RegLaserTrigger ) );
    ui->txtRegTriggerConfirm->setText( m_regSettings.String( RegLaserTriggerConfirm ) );
    ui->txtRegStop->setText( m_regSettings.String( RegLaserStop ) );
    ui->txtRegStopConfirm->setText( m_regSettings.String( RegLaserStopConfirm ) );
    ui->txtRegBarCode->setText( m_regSettings.String( RegBarcodeHasData ) );
    ui->txtRegBarcodeConfirm->setText( m_regSettings.String( RegBarcodeConfirm ) );
    ui->txtRegBarcodeData->setText( m_regSettings.String( RegBarcodeData ) );
    ui->txtRegBarcodeOK->setText( m_regSettings.String( RegBarcodeOK ) );
    ui->txtRegBarcodeNG->setText( m_regSettings.String( RegBarcodeNG ) );
    ui->txtRegPrintRes->setText( m_regSettings.String( RegMarkingResult ) );

    ui->tbvStoppers->setRowCount( m_pStoppers.size() );
    int row = 0;
    for ( auto& [index, pStopper] : m_pStoppers )
    {
        ui->tbvStoppers->setText( row, "X", QString::number( pStopper->x, 'f', 2 ) );
        ui->tbvStoppers->setText( row, "Y", QString::number( pStopper->y, 'f', 2 ) );
        row++;
    }
}
//
//void HxSettingsWindow::ShowProtectSetting()
//{
//    //if ( !m_pSettings )
//    //    m_pSettings = Settings()->Get();
//    ////ui->txtPassword->setText( FileManager()->GetSettings(HxFileManager::eSettingProtect)->value("Password").toString() );
//}
//
//void HxSettingsWindow::ShowActuatorSettings()
//{
//    //if ( !m_pSettings )
//    //    m_pSettings = Settings()->Get();
//
//    //ui->txtIVDir->setText( m_pSettings->IVProgramDir );
//    //ui->txtPLCIP->setText( m_pSettings->PLCConnIPAddress );
//    //ui->spxPLCPort->setValue( m_pSettings->PLCConnPort );
//
//    //ui->txtRegCvWidth->setText( m_pSettings->RegCvWidth );
//    //ui->txtRegStopper->setText( m_pSettings->RegStopper );
//    //ui->txtRegPassMode->setText( m_pSettings->RegPassMode );
//    //ui->txtRegSWReady->setText( m_pSettings->RegSoftwareReady );
//    //ui->txtRegTrigger->setText( m_pSettings->RegLaserTrigger );
//    //ui->txtRegTriggerConfirm->setText( m_pSettings->RegLaserTriggerConfirm );
//    //ui->txtRegStop->setText( m_pSettings->RegLaserStop );
//    //ui->txtRegStopConfirm->setText( m_pSettings->RegLaserStopConfirm );
//    //ui->txtRegBarCode->setText( m_pSettings->RegBarcodeHasData );
//    //ui->txtRegBarcodeConfirm->setText( m_pSettings->RegBarcodeConfirm );
//    //ui->txtRegBarcodeData->setText( m_pSettings->RegBarcodeData );
//    //ui->txtRegBarcodeOK->setText( m_pSettings->RegBarcodeOK );
//    //ui->txtRegBarcodeNG->setText( m_pSettings->RegBarcodeNG );
//    //ui->txtRegPrintRes->setText( m_pSettings->RegMarkingResult );
//}

//void HxSettingsWindow::ShowLaserSettings()
//{
//    
//}
//
//void HxSettingsWindow::ShowStoppers()
//{
//    ui->tbvStoppers->setRowCount( m_pStoppers.size() );
//    int row = 0;
//    for ( auto &[index, pStopper] : m_pStoppers )
//    {
//        ui->tbvStoppers->setText( row, "X", QString::number( pStopper->x, 'f', 2 ) );
//        ui->tbvStoppers->setText( row, "Y", QString::number( pStopper->y, 'f', 2 ) );
//        row++;
//    }
//}

void HxSettingsWindow::OnLoad()
{
    m_regSettings.Load();
    m_pStoppers = StopperManager()->GetStoppers();

    Show();
}

void HxSettingsWindow::OnBrowseDataDir()
{
    QFileInfo fi( m_regSettings.String( DatabaseFilePath ) );
    QString dir = QFileDialog::getExistingDirectory( this, "Chọn thư mục dữ liệu", fi.absoluteDir().absolutePath() );
    if ( !dir.isEmpty() )
    {
        ui->txtDatabasePath->setText( dir + "/LMS.db");
    }
}

void HxSettingsWindow::OnBrowseIVDir()
{
    QString dir = QFileDialog::getExistingDirectory( this, "Chọn thư mục chương trình IV", ui->txtIVDir->text() );
    if ( !dir.isEmpty() )
    {
        ui->txtIVDir->setText( dir );

    }
}

void HxSettingsWindow::OnEnumerateLaserPorts()
{
    //QList<QSerialPortInfo> portInfos = QSerialPortInfo::availablePorts();
    //QStringList portnames;
    //for ( auto& portinfo : portInfos )
    //{
    //    portnames.push_back( portinfo.portName() );
    //}
    //if ( !portnames.contains( HxSettings::laserPort ) )
    //{
    //    portnames.push_back( HxSettings::laserPort );
    //}

    //std::sort( portnames.begin(), portnames.end() );
    //ui->cbxLaserPort->clear();
    //ui->cbxLaserPort->addItem( "" );
    //ui->cbxLaserPort->addItems( portnames );

    //ui->cbxLaserPort->setCurrentText( HxSettings::laserPort );
}

void HxSettingsWindow::OnSave()
{
    m_regSettings.Clear();
    m_regSettings.Set( AdminPassword, ui->txtAdminPassword->text() );
    m_regSettings.Set( SuperPassword, ui->txtSuperPassword->text() );
    m_regSettings.Set( LeaderPassword, ui->txtLeaderPassword->text() );
    m_regSettings.Set( DatabaseFilePath, ui->txtDatabasePath->text() );
    m_regSettings.Set( IVProgramDir, ui->txtIVDir->text() );
    m_regSettings.Set( LaserConnPort, ui->cbxLaserPort->currentText() );

    m_regSettings.Set( PLCConnIPAddress, ui->txtPLCIP->text() );
    m_regSettings.Set( PLCConnPort, ui->spxPLCPort->value() );

    m_regSettings.Set( RegCvWidth, ui->txtRegCvWidth->text() );
    m_regSettings.Set( RegStopper, ui->txtRegStopper->text() );
    m_regSettings.Set( RegPassMode, ui->txtRegPassMode->text() );
    m_regSettings.Set( RegSoftwareReady, ui->txtRegSWReady->text() );

    m_regSettings.Set( RegLaserTrigger, ui->txtRegTrigger->text() );
    m_regSettings.Set( RegLaserTriggerConfirm, ui->txtRegTriggerConfirm->text() );
    m_regSettings.Set( RegLaserStop, ui->txtRegStop->text() );
    m_regSettings.Set( RegLaserStopConfirm, ui->txtRegStopConfirm->text() );
    m_regSettings.Set( RegBarcodeHasData, ui->txtRegBarCode->text() );

    m_regSettings.Set( RegBarcodeConfirm, ui->txtRegBarcodeConfirm->text() );
    m_regSettings.Set( RegBarcodeData, ui->txtRegBarcodeData->text() );
    m_regSettings.Set( RegBarcodeOK, ui->txtRegBarcodeOK->text() );
    m_regSettings.Set( RegBarcodeNG, ui->txtRegBarcodeNG->text() );
    m_regSettings.Set( RegMarkingResult, ui->txtRegPrintRes->text() );
    m_regSettings.Save();
    m_regSettings.Load();

    m_pStoppers.clear();
    for ( int i = 0; i < ui->tbvStoppers->RowCount(); i++ )
    {
        HxStopperPtr pStopper = StopperManager()->Create();
        pStopper->x = ui->tbvStoppers->item( i, 0 )->text().toDouble();
        pStopper->y = ui->tbvStoppers->item( i, 1 )->text().toDouble();
        m_pStoppers[ i + 1 ] = pStopper;
        StopperManager()->Save( i + 1, pStopper );
    }

    qApp->postEvent( qApp, new HxEvent( HxEvent::eSettingChanged ) );
    HxMsgInfo( "Đã lưu cài đặt !", "Thông báo" );
}
//
//void HxSettingsWindow::on_btnBrowseDataDir_clicked()
//{
//    //QString dir = QFileDialog::getExistingDirectory( this, "Chọn thư mục dữ liệu", ui->txtDataDir->text() );
//    //if ( !dir.isEmpty() )
//    //{
//    //    ui->txtDataDir->setText( dir );
//
//    //}
//}
//
//void HxSettingsWindow::on_btnEnum_clicked()
//{
//    //QList<QSerialPortInfo> portInfos = QSerialPortInfo::availablePorts();
//    //QStringList portnames;
//    //for ( auto& portinfo : portInfos )
//    //{
//    //    portnames.push_back( portinfo.portName() );
//    //}
//    //if ( !portnames.contains( HxSettings::laserPort ) )
//    //{
//    //    portnames.push_back( HxSettings::laserPort );
//    //}
//
//    //std::sort( portnames.begin(), portnames.end() );
//    //ui->cbxLaserPort->clear();
//    //ui->cbxLaserPort->addItem( "" );
//    //ui->cbxLaserPort->addItems( portnames );
//
//    //ui->cbxLaserPort->setCurrentText( HxSettings::laserPort );
//}
//
