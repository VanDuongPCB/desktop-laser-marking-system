#include "HxTransferWindow.h"
#include "ui_hxtransferwindow.h"
#include "HxModel.h"
#include "HxMarker.h"
#include "HxActuator.h"
#include "HxMessage.h"
#include "HxException.h"
#include "HxSystemError.h"

HxTransferWindow::HxTransferWindow( QWidget* parent ) : QMainWindow( parent ), ui( new Ui::TransferWindow )
{
    ui->setupUi( this );
    mainWindow = ( HxMainWindow* )parent;
    connect( HxMarker::instance(), &HxMarker::started, this, &HxTransferWindow::passStarted );
    connect( HxMarker::instance(), &HxMarker::stopped, this, &HxTransferWindow::passStopped );
    on_btnReload_clicked();
}

HxTransferWindow::~HxTransferWindow()
{
    delete ui;
}

void HxTransferWindow::passStarted()
{
    ui->cbxModel->setEnabled( false );
    ui->spxCvWidth->setEnabled( false );
    ui->btnPass->setEnabled( false );
    ui->btnStop->setEnabled( true );
}

void HxTransferWindow::passStopped()
{
    ui->cbxModel->setEnabled( true );
    ui->spxCvWidth->setEnabled( true );
    ui->btnPass->setEnabled( true );
    ui->btnStop->setEnabled( false );
}


void HxTransferWindow::on_btnReload_clicked()
{
    QString old = ui->cbxModel->currentText();
    ui->cbxModel->setEnabled( false );
    ui->cbxModel->clear();
    for ( auto& md : HxModel::items )
    {
        ui->cbxModel->addItem( md->name );
    }
    ui->cbxModel->setCurrentText( old );
    ui->cbxModel->setEnabled( true );
}


void HxTransferWindow::on_spxCvWidth_valueChanged( double arg1 )
{
    try
    {
        HxActuator::setCvWidth( ui->spxCvWidth->value() );
    }
    catch ( HxException ex )
    {
        HxSystemError::instance()->errorReport( ex );
    }
}


void HxTransferWindow::on_cbxModel_currentTextChanged( const QString& arg1 )
{
    if ( ui->cbxModel->isEnabled() == false ) return;
    auto model = HxModel::find( arg1 );
    if ( model != nullptr )
    {
        ui->spxCvWidth->setValue( model->cvWidth );
    }
}

void HxTransferWindow::on_btnPass_clicked()
{
    try
    {
        HxActuator::setCvWidth( ui->spxCvWidth->value() );
        //        Actuator::setEnable(true);
        HxActuator::setTransferMode( true );
        ui->btnPass->setEnabled( false );
        ui->btnStop->setEnabled( true );
        ui->cbxModel->setEnabled( false );
        ui->spxCvWidth->setEnabled( false );
        ui->btnReload->setEnabled( false );
        if ( mainWindow != nullptr )
        {
            mainWindow->setNavEnable( false );
        }
    }
    catch ( HxException ex )
    {
        HxSystemError::instance()->errorReport( ex );
    }
}


void HxTransferWindow::on_btnStop_clicked()
{
    try
    {
        //        Actuator::setEnable(false);
        HxActuator::setTransferMode( false );
        ui->btnPass->setEnabled( true );
        ui->btnStop->setEnabled( false );
        ui->cbxModel->setEnabled( true );
        ui->spxCvWidth->setEnabled( true );
        ui->btnReload->setEnabled( true );
        if ( mainWindow != nullptr )
        {
            mainWindow->setNavEnable( true );
        }
    }
    catch ( HxException ex )
    {
        HxSystemError::instance()->errorReport( ex );
    }
}

