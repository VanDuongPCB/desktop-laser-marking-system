#include "HxTransferWindow.h"
#include "ui_hxtransferwindow.h"
#include "HxModel.h"
#include "HxMarker.h"
#include "HxPLC.h"
#include "HxMessage.h"
#include "HxException.h"
#include "HxSystemReport.h"
#include "HxEvent.h"

HxTransferWindow::HxTransferWindow( QWidget* parent ) : QMainWindow( parent ), ui( new Ui::TransferWindow )
{
    ui->setupUi( this );
    qApp->installEventFilter( this );

    OnShowModels();
    UpdateUI();

    connect( ui->cbxModel, &QComboBox::currentTextChanged, this, &HxTransferWindow::OnSelect );
    connect( ui->spxCvWidth, &QDoubleSpinBox::valueChanged, this, &HxTransferWindow::OnCvWidthChanged );
    connect( ui->btnPass, &QToolButton::clicked, this, &HxTransferWindow::OnTransfer );
    connect( ui->btnStop, &QToolButton::clicked, this, &HxTransferWindow::OnStop );
}

HxTransferWindow::~HxTransferWindow()
{
    delete ui;
}
//
//void HxTransferWindow::on_btnReload_clicked()
//{
//    //QString old = ui->cbxModel->currentText();
//    //ui->cbxModel->setEnabled( false );
//    //ui->cbxModel->clear();
//    //auto Models = ModelManager()->GetModels();
//    //for ( auto& [name, md] : Models )
//    //{
//    //    ui->cbxModel->addItem( md->Name() );
//    //}
//    //ui->cbxModel->setCurrentText( old );
//    //ui->cbxModel->setEnabled( true );
//}

//
//void HxTransferWindow::on_spxCvWidth_valueChanged( double arg1 )
//{
//    //try
//    //{
//    //    PLC()->SetCvWidth( ui->spxCvWidth->value() );
//    //}
//    //catch ( HxException ex )
//    //{
//    //    GetSystemReport()->Report( ex );
//    //}
//}

//
//void HxTransferWindow::on_cbxModel_currentTextChanged( const QString& arg1 )
//{
//    //if ( ui->cbxModel->isEnabled() == false ) return;
//    //auto model = ModelManager()->GetModel( arg1 );
//    //if ( model != nullptr )
//    //{
//    //    ui->spxCvWidth->setValue( model->CvWidth() );
//    //}
//}
//
//void HxTransferWindow::on_btnPass_clicked()
//{
//    //try
//    //{
//    //    GetPLC()->SetCvWidth( ui->spxCvWidth->value() );
//    //    //        GetHardware()->setEnable(true);
//    //    GetPLC()->SetTransferMode( true );
//    //    ui->btnPass->setEnabled( false );
//    //    ui->btnStop->setEnabled( true );
//    //    ui->cbxModel->setEnabled( false );
//    //    ui->spxCvWidth->setEnabled( false );
//    //    ui->btnReload->setEnabled( false );
//    //    if ( mainWindow != nullptr )
//    //    {
//    //        //mainWindow->setNavEnable( false );
//    //    }
//    //}
//    //catch ( HxException ex )
//    //{
//    //    GetSystemReport()->Report( ex );
//    //}
//}

//
//void HxTransferWindow::on_btnStop_clicked()
//{
//    //try
//    //{
//    //    //        GetHardware()->setEnable(false);
//    //    PLC()->SetTransferMode( false );
//    //    ui->btnPass->setEnabled( true );
//    //    ui->btnStop->setEnabled( false );
//    //    ui->cbxModel->setEnabled( true );
//    //    ui->spxCvWidth->setEnabled( true );
//    //    ui->btnReload->setEnabled( true );
//    //    //if ( mainWindow != nullptr )
//    //    //{
//    //    //    mainWindow->setNavEnable( true );
//    //    //}
//    //}
//    //catch ( HxException ex )
//    //{
//    //    GetSystemReport()->Report( ex );
//    //}
//}

bool HxTransferWindow::eventFilter( QObject* watched, QEvent* event )
{
    HxEvent* hxEvent( nullptr );
    HxEvent::Type type;
    if ( !HxEvent::IsCustomEvent( event, hxEvent, type ) )
        return QMainWindow::eventFilter( watched, event );

    switch ( type )
    {
    case HxEvent::eDesignAdded:
    case HxEvent::eDesignDeleted:
    case HxEvent::eDesignChanged:
    case HxEvent::eModelAdded:
    case HxEvent::eModelDeleted:
    case HxEvent::eModelChanged:
    case HxEvent::eLOTAdded:
    case HxEvent::eLOTDeleted:
    case HxEvent::eLOTChanged:
    case HxEvent::eSettingChanged:
        OnShowModels();
        break;
    case HxEvent::eMarkerSetupChanged:
    case HxEvent::eMarkerGoFree:
    case HxEvent::eMarkerGoTransfer:
    case HxEvent::eMarkerGoMark:
        UpdateUI();
        break;
    case HxEvent::eMarkerStopped:
        break;
    case HxEvent::eMarkerGoError:
        break;
    default:
        break;
    }

    return QMainWindow::eventFilter( watched, event );
}

void HxTransferWindow::OnShowModels()
{
    QString old = ui->cbxModel->currentText();
    ui->cbxModel->setEnabled( false );
    ui->cbxModel->clear();
    auto Models = ModelManager()->GetModels();
    for ( auto& [name, md] : Models )
    {
        ui->cbxModel->addItem( md->Name() );
    }
    ui->cbxModel->setCurrentText( old );
    ui->cbxModel->setEnabled( true );
}

void HxTransferWindow::OnSelect( const QString& modelName )
{
    if ( Marker()->Setup( HxMarker::eModeTransfer, modelName ) )
    {
        HxModelPtr pModel = Marker()->Model();
        if ( pModel )
        {
            QSignalBlocker blocker( ui->spxCvWidth );
            ui->spxCvWidth->setValue( pModel->CvWidth() );
        }
    }
}

void HxTransferWindow::OnCvWidthChanged( double width )
{
    try
    {
        PLC()->SetCvWidth( ui->spxCvWidth->value() );
    }
    catch ( HxException ex )
    {
        HxMsgError( ex.Message(), ex.Where() );
    }
}

void HxTransferWindow::OnTransfer()
{
    Marker()->Transfer();
}

void HxTransferWindow::OnStop()
{
    Marker()->Pause();
}

void HxTransferWindow::UpdateUI()
{
    auto pModel = Marker()->Model();
    auto state = Marker()->GetState();

    ui->cbxModel->setEnabled( state == HxMarker::eOnFree );
    ui->spxCvWidth->setEnabled( state == HxMarker::eOnFree );
    ui->btnPass->setEnabled( state == HxMarker::eOnFree && pModel != nullptr );
    ui->btnStop->setEnabled( state == HxMarker::eOnTransfering );
}