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

    connect( ui->cbxModel, &QComboBox::currentTextChanged, this, &HxTransferWindow::OnSelect );
    connect( ui->spxCvWidth, &QDoubleSpinBox::valueChanged, this, &HxTransferWindow::OnCvWidthChanged );
    connect( ui->btnPass, &QToolButton::clicked, this, &HxTransferWindow::OnTransfer );
    connect( ui->btnStop, &QToolButton::clicked, this, &HxTransferWindow::OnStop );
}

HxTransferWindow::~HxTransferWindow()
{
    delete ui;
}

void HxTransferWindow::showEvent( QShowEvent* )
{
    OnShowModels();
    UpdateUI();
}

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
    QSignalBlocker blocker( ui->cbxModel );
    QString old = ui->cbxModel->currentText();
    ui->cbxModel->clear();
    auto Models = ModelManager()->GetModels();
    for ( auto& [name, md] : Models )
    {
        ui->cbxModel->addItem( md->Name() );
    }
    ui->cbxModel->setCurrentText( old );
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
    LockUI();
    Marker()->Transfer();
}

void HxTransferWindow::OnStop()
{
    LockUI();
    Marker()->Pause();
}

void HxTransferWindow::LockUI()
{
    ui->cbxModel->setEnabled( false );
    ui->spxCvWidth->setEnabled( false );
    ui->btnPass->setEnabled( false );
    ui->btnStop->setEnabled( false );
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