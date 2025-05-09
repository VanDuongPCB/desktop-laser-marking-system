#include "HxMarkWindow.h"
#include "ui_HxMarkWindow.h"


#include "HxMarker.h"
#include "HxMessage.h"
#include "HxException.h"
#include "HxSystemReport.h"
#include "HxLaser.h"
#include "HxPLC.h"
#include "HxEvent.h"

#include "HxConvert.h"
#include "HxDataGenerator.h"

#include "HxLOTSelector.h"

HxMarkWindow::HxMarkWindow( QWidget* parent ) : QMainWindow( parent ), ui( new Ui::MarkWindow )
{
    ui->setupUi( this );
    qApp->installEventFilter( this );


    ui->tbvBlocks->setHeaders( { "Block","Dữ liệu" } );
    ui->tbvErrors->setHeaders( { "Thời gian","Từ","Lỗi" } );

    //connect( HxMarker::instance(), &HxMarker::printed, this, &HxMarkWindow::controllerPrinted );
    //connect( HxMarker::instance(), &HxMarker::started, this, &HxMarkWindow::markStarted );
    //connect( HxMarker::instance(), &HxMarker::stopped, this, &HxMarkWindow::markStopped );
    connect( GetSystemReport(), &HxSystemReport::reported, this, &HxMarkWindow::handleException );

    // showLots();
    // showLotInfo( nullptr );
    // showLotStatus( nullptr );
    // showLotBlocks();
    // showExceptions();
    // updateUI();

    connect( ui->actionSelect, &QAction::triggered, this, &HxMarkWindow::OnSelect );
    connect( ui->actionMark, &QAction::triggered, this, &HxMarkWindow::OnTest );
    connect( ui->actionRun, &QAction::triggered, this, &HxMarkWindow::OnRun );
    connect( ui->actionStop, &QAction::triggered, this, &HxMarkWindow::OnStop );
}

HxMarkWindow::~HxMarkWindow()
{
    delete ui;
}

void HxMarkWindow::showEvent( QShowEvent* )
{
    ShowLotInfo();
    ShowLotBlocks();
    ShowLotStatus();
}

bool HxMarkWindow::eventFilter( QObject* watched, QEvent* event )
{
    HxEvent* hxEvent( nullptr );
    HxEvent::Type type;
    if ( !HxEvent::IsCustomEvent( event, hxEvent, type ) )
        return QMainWindow::eventFilter( watched, event );

    switch ( type )
    {
    case HxEvent::eMarkerSetupChanged:
    {
        ShowLotInfo();
        ShowLotBlocks();
        ShowLotStatus();
        UpdateUI();
    }
        break;
    case HxEvent::eMarkerGoFree:
    case HxEvent::eMarkerGoTransfer:
    case HxEvent::eMarkerGoTest:
    case HxEvent::eMarkerGoMark:
        UpdateUI();
        break;
    case HxEvent::eMarkerMarked:
        ShowLotBlocks();
        ShowLotStatus();
        break;
    case HxEvent::eMarkerGoFinish:
        UpdateUI();
        break;
    case HxEvent::eMarkerGoError:
        break;
    case HxEvent::eMarkerStopped:
        break;
    default:
        break;
    }
    return QMainWindow::eventFilter( watched, event );
}

void HxMarkWindow::ShowLotInfo()
{
    HxLOTPtr pLOT = Marker()->LOT();
    if ( !pLOT )
    {
        ui->lblLotName->setText( "Chưa chọn lot..." );
        ui->lblCounterStart->setText( "Chưa chọn lot..." );
        ui->lblSerialStart->setText( "Chưa chọn lot..." );
        ui->lblSerialEnd->setText( "Chưa chọn lot..." );
        ui->lblQuantity->setText( "Chưa chọn lot..." );
        ui->lblStatus->setText( "Chưa chọn lot..." );
        ui->lblModelName->setText( "Chưa chọn lot..." );
        ui->lblModelCode->setText( "Chưa chọn lot..." );
        ui->lblDesign->setText( "Chưa chọn lot..." );
        ui->lblIVProgram->setText( "Chưa chọn lot..." );
    }
    else
    {
        ui->lblLotName->setText( pLOT->Name() );
        ui->lblCounterStart->setText( pLOT->CounterStart() );
        ui->lblSerialStart->setText( pLOT->MACStart() );
        ui->lblSerialEnd->setText( pLOT->MACEnd() );
        ui->lblQuantity->setText( QString::number( pLOT->Quantity() ) );
        ui->lblStatus->setText( ProductStatusToString( pLOT->Status() ).toUpper() );
        ui->lblModelName->setText( pLOT->Model() );
        auto model = ModelManager()->GetModel( pLOT->Model() );
        if ( model != nullptr )
        {
            ui->lblModelCode->setText( model->Code() );
            ui->lblDesign->setText( model->Design() );
            ui->lblIVProgram->setText( model->IVProgram() );
        }
        else
        {
            ui->lblModelCode->setText( "Không xác định..." );
            ui->lblDesign->setText( "Không xác định..." );
            ui->lblIVProgram->setText( "Không xác định..." );
        }
    }
}

void HxMarkWindow::ShowLotStatus()
{
    HxLOTPtr pLOT = Marker()->LOT();
    if ( !pLOT )
    {
        ui->pgbProgress->setMaximum( 0 );
        ui->pgbProgress->setValue( 0 );
        ui->lblStatus->setText( "KHÔNG XÁC ĐỊNH" );
        ui->lblStatus->setStyleSheet( "color:#888" );
    }
    else
    {
        ui->pgbProgress->setMaximum( pLOT->Quantity() );
        ui->pgbProgress->setValue( pLOT->Progress() );
        ui->lblStatus->setText( ProductStatusToString( pLOT->Status() ).toUpper() );

        switch ( pLOT->Status() )
        {
        case HxLOT::ePending:
            ui->lblStatus->setStyleSheet( "color:#888" );
            break;
        case HxLOT::eProduct:
            ui->lblStatus->setStyleSheet( "color:#f80" );
            break;
        case HxLOT::eCompleted:
            ui->lblStatus->setStyleSheet( "color:#080" );
            break;
        default:
            break;
        }
    }
}

void HxMarkWindow::ShowLotBlocks()
{
    ui->tbvBlocks->setRowCount( 0 );
    auto pLOT = Marker()->LOT();
    auto pModel = Marker()->Model();
    auto pDesign = Marker()->Design();
    if ( !pLOT || !pModel || !pDesign )
        return;

    int codeIndex = pDesign->IndexOfBlockCode();

    std::map<int, QString> blockdatas = GenMarkData( pDesign, pLOT, pModel, -1 );
    ui->tbvBlocks->setRowCount( blockdatas.size() );
    int row = 0;
    for ( auto& [index, data] : blockdatas )
    {
        ui->tbvBlocks->setText( row, "Block", QString::number( index ).rightJustified( 3, '0' ) );
        ui->tbvBlocks->setText( row, "Dữ liệu", data );
        if ( index == codeIndex )
            ui->lblBarcode->setText( data );
        row++;
    }

    ui->lblFormat->setText( "" );
    if ( codeIndex > 0 )
    {
        ui->lblFormat->setText( pDesign->Block( codeIndex ).data );
    }
}

void HxMarkWindow::ShowExceptions()
{
    int rows = ( int )exceptions.size();
    ui->tbvErrors->setRowCount( rows );
    for ( int row = 0; row < rows; row++ )
    {
        ui->tbvErrors->setText( row, 0, exceptions[ row ].Time() );
        ui->tbvErrors->setText( row, 1, exceptions[ row ].Where() );
        ui->tbvErrors->setText( row, 2, exceptions[ row ].Message().replace("\n", ", "));
    }
    ui->tbvErrors->scrollToBottom();
}

void HxMarkWindow::UpdateUI()
{
    auto pLOT = Marker()->LOT();
    auto state = Marker()->GetState();

    ui->actionSelect->setEnabled( state == HxMarker::eOnFree || state == HxMarker::eOnFinish );
    ui->actionMark->setEnabled( pLOT != nullptr && state == HxMarker::eOnFree );
    ui->actionRun->setEnabled( pLOT != nullptr && state == HxMarker::eOnFree );
    ui->actionStop->setEnabled( state == HxMarker::eOnMarking );

    //auto lot = HxMarker::instance()->m_pLOT;
    //bool hasData = lot != nullptr;
    //bool canMark = hasData && lot->IsCompleted() == false;
    //bool busy = HxMarker::instance()->IsBusy();

    //ui->actionSelect->setEnabled( !busy );
    //ui->actionLoad->setEnabled( !busy );
    //ui->actionMark->setEnabled( hasData && !busy );
    //ui->actionRun->setEnabled( canMark && !busy );
    //ui->actionStop->setEnabled( busy );
}

void HxMarkWindow::handleException( HxException ex )
{
    exceptions.push_back( ex );
    ShowExceptions();
}

void HxMarkWindow::markStarted()
{
    UpdateUI();
}

void HxMarkWindow::markStopped()
{
    UpdateUI();
}

void HxMarkWindow::controllerPrinted( std::shared_ptr<HxLOT> lot )
{
    //showLotStatus( lot );
    //showLotBlocks();
}

void HxMarkWindow::on_actionSelect_triggered()
{
    //ui->stackedWidget->setCurrentWidget( ui->pageSelector );
    UpdateUI();
}

void HxMarkWindow::on_actionRun_triggered()
{
    //try
    //{
    //    if ( GetPLC()->SetEnable( true ) )
    //    {
    //        HxMarker::instance()->Start();
    //        ui->actionSelect->setEnabled( false );
    //        ui->actionLoad->setEnabled( false );
    //        ui->actionMark->setEnabled( false );
    //        ui->actionRun->setEnabled( false );
    //        ui->actionStop->setEnabled( true );
    //        if ( mainWindow != nullptr )
    //        {
    //            mainWindow->setNavEnable( false );
    //        }
    //    }
    //}
    //catch ( HxException ex )
    //{
    //    GetSystemReport()->Report( ex );
    //    //ex.where = "Khắc";
    //    //        Message::error(ex.message);
    //}
}

void HxMarkWindow::on_actionStop_triggered()
{
    //try
    //{
    //    if ( GetPLC()->SetEnable( false ) )
    //    {
    //        HxMarker::instance()->Stop();
    //        ui->actionSelect->setEnabled( true );
    //        ui->actionLoad->setEnabled( true );
    //        ui->actionMark->setEnabled( true );
    //        ui->actionRun->setEnabled( true );
    //        ui->actionStop->setEnabled( false );
    //        if ( mainWindow != nullptr )
    //        {
    //            mainWindow->setNavEnable( true );
    //        }
    //    }
    //}
    //catch ( HxException ex )
    //{
    //    //        Message::error(ex.message);
    //    //ex.where = "Dừng khắc";
    //    GetSystemReport()->Report( ex );
    //}

}

void HxMarkWindow::on_actionMark_triggered()
{
    //try
    //{
    //    HxMarker::instance()->Mark( true );
    //    HxMsgInfo( "Đã khắc xong !", "In test" );
    //}
    //catch ( HxException ex )
    //{
    //    //        Message::error(ex.message);
    //    //ex.where = "In test";
    //    GetSystemReport()->Report( ex );
    //}
}

void HxMarkWindow::on_tbvSelector_doubleClicked( const QModelIndex& index )
{
    //int row = index.row();
    //QString lotName = ui->tbvSelector->item( row, 0 )->text();
    //auto pLOT = GetLOTManager()->GetLOT( lotName );
    //if ( HxMarker::instance()->Select( pLOT ) )
    //{
    //    ui->stackedWidget->setCurrentWidget( ui->pageMark );
    //    showLotInfo( HxMarker::instance()->m_pLOT );
    //    showLotStatus( HxMarker::instance()->m_pLOT );
    //    showLotBlocks();
    //    updateUI();
    //}
}

void HxMarkWindow::on_actionLoad_triggered()
{
    //showLots();
    //showLotInfo( HxMarker::instance()->m_pLOT );
    //showLotStatus( HxMarker::instance()->m_pLOT );
    //showLotBlocks();
}

void HxMarkWindow::OnSelect()
{
    if ( HxLOTSelector( this ).exec() )
    {
        ShowLotInfo();
        ShowLotBlocks();
        ShowLotStatus();
    }
}

void HxMarkWindow::OnTest()
{
    Marker()->Test();
}

void HxMarkWindow::OnRun()
{
    Marker()->Mark();
}

void HxMarkWindow::OnStop()
{
    Marker()->Pause();
}