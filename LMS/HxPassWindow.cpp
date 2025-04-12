#include "HxPassWindow.h"
#include "ui_HxPassWindow.h"
#include "HxModel.h"
#include "HxMarker.h"
#include "HxPLC.h"
#include "HxMessage.h"
#include "HxException.h"
#include "HxSystemError.h"

HxPassWindow::HxPassWindow( QWidget* parent ) : QMainWindow( parent ), ui( new Ui::PassWindow )
{
    ui->setupUi( this );
    mainWindow = ( HxMainWindow* )parent;
    connect( HxMarker::instance(), &HxMarker::started, this, &HxPassWindow::passStarted );
    connect( HxMarker::instance(), &HxMarker::stopped, this, &HxPassWindow::passStopped );
    on_btnReload_clicked();
}

HxPassWindow::~HxPassWindow()
{
    delete ui;
}

void HxPassWindow::passStarted()
{
    ui->cbxModel->setEnabled( false );
    ui->spxCvWidth->setEnabled( false );
    ui->btnPass->setEnabled( false );
    ui->btnStop->setEnabled( true );
}

void HxPassWindow::passStopped()
{
    ui->cbxModel->setEnabled( true );
    ui->spxCvWidth->setEnabled( true );
    ui->btnPass->setEnabled( true );
    ui->btnStop->setEnabled( false );
}


void HxPassWindow::on_btnReload_clicked()
{
    QString old = ui->cbxModel->currentText();
    ui->cbxModel->setEnabled( false );
    ui->cbxModel->clear();
    auto Models = GetModelManager()->GetModels();
    for ( auto& [name, md] : Models )
    {
        ui->cbxModel->addItem( md->Name() );
    }
    ui->cbxModel->setCurrentText( old );
    ui->cbxModel->setEnabled( true );
}


void HxPassWindow::on_spxCvWidth_valueChanged( double arg1 )
{
    try
    {
        GetPLC()->setCvWidth( ui->spxCvWidth->value() );
    }
    catch ( HxException ex )
    {
        HxSystemError::instance()->errorReport( ex );
    }
}


void HxPassWindow::on_cbxModel_currentTextChanged( const QString& arg1 )
{
    if ( ui->cbxModel->isEnabled() == false ) return;
    auto model = GetModelManager()->GetModel( arg1 );
    if ( model != nullptr )
    {
        ui->spxCvWidth->setValue( model->CvWidth() );
    }
}

void HxPassWindow::on_btnPass_clicked()
{
    try
    {
        GetPLC()->setCvWidth( ui->spxCvWidth->value() );
        //        GetHardware()->setEnable(true);
        GetPLC()->setTransferMode( true );
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


void HxPassWindow::on_btnStop_clicked()
{
    try
    {
        //        GetHardware()->setEnable(false);
        GetPLC()->setTransferMode( false );
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

