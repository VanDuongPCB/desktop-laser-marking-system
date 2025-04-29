#include "HxMarkWindow.h"
#include "ui_hxmarkwindow.h"


#include "HxMarker.h"
#include "HxMessage.h"
#include "HxException.h"
#include "HxSystemError.h"
#include "HxLaserDevice.h"
#include "HxActuator.h"

#include "HxDataGenerator.h"




HxMarkWindow::HxMarkWindow( QWidget* parent ) : QMainWindow( parent ), ui( new Ui::MarkWindow )
{
    mainWindow = ( HxMainWindow* )parent;
    ui->setupUi( this );
    connect( HxMarker::Instance(), &HxMarker::printed, this, &HxMarkWindow::ControllerPrinted );
    connect( HxMarker::Instance(), &HxMarker::started, this, &HxMarkWindow::MarkStarted );
    connect( HxMarker::Instance(), &HxMarker::stopped, this, &HxMarkWindow::MarkStopped );
    connect( HxSystemError::Instance(), &HxSystemError::Reported, this, &HxMarkWindow::HandleException );

    ShowLots();
    ShowLotInfo( nullptr );
    ShowLotStatus( nullptr );
    ShowLotBlocks();
    ShowExceptions();
    UpdateUI();
}

HxMarkWindow::~HxMarkWindow()
{
    delete ui;
}

void HxMarkWindow::showEvent( QShowEvent* )
{
    ShowLots();
    ShowLotInfo( HxMarker::Instance()->lot );
    ShowLotStatus( HxMarker::Instance()->lot );
    UpdateUI();
}

void HxMarkWindow::ShowLots()
{
    if ( ui->tbvSelector->headers.empty() )
    {
        ui->tbvSelector->SetHeaders( { "Tên","Model","MAC đầu","MAC cuối","Sản lượng","Tiến độ","Trạng thái" } );
    }

    HxLOT::Sort();

    int rows = ( int )HxLOT::items.size();
    ui->tbvSelector->SetRowCount( rows );
    for ( int row = 0; row < rows; row++ )
    {
        QString status = HxLOT::items[ row ]->Status();

        ui->tbvSelector->SetText( row, "Tên", HxLOT::items[ row ]->name );
        ui->tbvSelector->SetText( row, "Model", HxLOT::items[ row ]->modelName );
        ui->tbvSelector->SetText( row, "MAC đầu", HxLOT::items[ row ]->macStart );
        ui->tbvSelector->SetText( row, "MAC cuối", HxLOT::items[ row ]->macEnd );
        ui->tbvSelector->SetText( row, "Sản lượng", QString::number( HxLOT::items[ row ]->quantity ) );
        ui->tbvSelector->SetText( row, "Tiến độ", QString::number( HxLOT::items[ row ]->progress ) + "/" + QString::number( HxLOT::items[ row ]->quantity ) );
        ui->tbvSelector->SetText( row, "Trạng thái", status );

        QColor background = QColor( 255, 255, 255 );
        if ( status == "Đang sản xuất" )
        {
            background = QColor( 255, 255, 128 );
        }
        else if ( status == "Đã hoàn thành" )
        {
            background = QColor( 128, 255, 128 );
        }

        for ( int col = 0; col < ui->tbvSelector->DataTable()->columnCount(); col++ )
        {
            ui->tbvSelector->Item( row, col )->setBackground( background );
        }
    }
}

void HxMarkWindow::ShowLotInfo( std::shared_ptr<HxLOT> lot )
{
    if ( lot == nullptr )
    {
        ui->lblLotName->setText( "Chưa chọn lot..." );
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
        ui->lblLotName->setText( lot->name );
        ui->lblSerialStart->setText( lot->macStart );
        ui->lblSerialEnd->setText( lot->macEnd );
        ui->lblQuantity->setText( QString::number( lot->quantity ) );
        ui->lblStatus->setText( lot->Status() );
        ui->lblModelName->setText( lot->modelName );
        auto model = HxModel::Find( lot->modelName );
        if ( model != nullptr )
        {
            ui->lblModelCode->setText( model->code );
            ui->lblDesign->setText( model->design );
            ui->lblIVProgram->setText( model->ivProgram );
        }
        else
        {
            ui->lblModelCode->setText( "Không xác định..." );
            ui->lblDesign->setText( "Không xác định..." );
            ui->lblIVProgram->setText( "Không xác định..." );
        }
    }
}

void HxMarkWindow::ShowLotStatus( std::shared_ptr<HxLOT> lot )
{
    if ( lot == nullptr )
    {
        ui->lblStatus->setText( "Không xác định..." );
        ui->lblCount->setText( "Không xác định..." );
        ui->pgbProgress->setMaximum( 100 );
        ui->pgbProgress->setValue( 0 );
    }
    else
    {
        ui->lblStatus->setText( lot->Status() );
        ui->lblCount->setText( QString::number( lot->progress ) );
        ui->pgbProgress->setMaximum( lot->quantity );
        ui->pgbProgress->setValue( lot->progress );

        if ( lot->Status() == "Chưa sản xuất" )
        {
            ui->lblStatus->setStyleSheet( "background:#888" );
        }
        else if ( lot->Status() == "Đang sản xuất" )
        {
            ui->lblStatus->setStyleSheet( "background:#ff8" );
        }
        else
        {
            ui->lblStatus->setStyleSheet( "background:#8f8" );
        }
    }
}

void HxMarkWindow::ShowLotBlocks()
{
    if ( ui->tbvBlocks->headers.empty() )
    {
        ui->tbvBlocks->SetHeaders( { "Block","Dữ liệu" } );
    }
    ui->tbvBlocks->SetRowCount( 0 );
    auto lot = HxMarker::Instance()->lot;
    if ( lot == nullptr ) return;
    auto model = HxMarker::Instance()->model;
    auto design = HxMarker::Instance()->design;
    int codeIndex = -1;
    if ( design != nullptr )
    {
        codeIndex = design->IndexOfBlockCode();
    }
    auto tempLot = std::make_shared<HxLOT>( HxLOT() );
    tempLot.get()[ 0 ] = lot.get()[ 0 ];
    tempLot->progress--;
    if ( tempLot->progress < 0 )
    {
        tempLot->progress = 0;
    }

    std::map<int, QString> blocks = BlockDataGen( design, tempLot, model );
    ui->tbvBlocks->SetRowCount( blocks.size() );
    int row = 0;
    for ( auto& [index, block] : blocks )
    {
        ui->tbvBlocks->SetText( row, "Block", QString::number( index ).rightJustified( 3, '0' ) );
        ui->tbvBlocks->SetText( row, "Dữ liệu", block );
        if ( index == codeIndex )
        {
            ui->lblBarcode->setText( block );
        }
        row++;
    }

    ui->lblFormat->setText( "" );
    if ( codeIndex > 0 )
    {
        ui->lblFormat->setText( design->blocks[ codeIndex ].data );
    }
}

void HxMarkWindow::ShowExceptions()
{
    if ( ui->tbvErrors->headers.empty() )
    {
        ui->tbvErrors->SetHeaders( { "Thời gian","Từ","Lỗi" } );
    }
    int rows = ( int )exceptions.size();
    ui->tbvErrors->SetRowCount( rows );
    for ( int row = 0; row < rows; row++ )
    {
        ui->tbvErrors->SetText( row, 0, exceptions[ row ].time );
        ui->tbvErrors->SetText( row, 1, exceptions[ row ].where );
        ui->tbvErrors->SetText( row, 2, exceptions[ row ].message.replace( "\n", ", " ) );
    }
    ui->tbvErrors->scrollToBottom();
}

void HxMarkWindow::UpdateUI()
{
    auto lot = HxMarker::Instance()->lot;
    bool hasData = lot != nullptr;
    bool canMark = hasData && lot->IsCompleted() == false;
    bool busy = HxMarker::Instance()->IsBusy();

    ui->actionSelect->setEnabled( !busy );
    ui->actionLoad->setEnabled( !busy );
    ui->actionMark->setEnabled( hasData && !busy );
    ui->actionRun->setEnabled( canMark && !busy );
    ui->actionStop->setEnabled( busy );
}

void HxMarkWindow::HandleException( HxException ex )
{
    exceptions.push_back( ex );
    ShowExceptions();
}

void HxMarkWindow::MarkStarted()
{
    UpdateUI();
}

void HxMarkWindow::MarkStopped()
{
    UpdateUI();
}

void HxMarkWindow::ControllerPrinted( std::shared_ptr<HxLOT> lot )
{
    ShowLotStatus( lot );
    ShowLotBlocks();
}

void HxMarkWindow::on_actionSelect_triggered()
{
    ui->stackedWidget->setCurrentWidget( ui->pageSelector );
    UpdateUI();
}

void HxMarkWindow::on_actionRun_triggered()
{
    try
    {
        if ( HxActuator::SetEnable( true ) )
        {
            HxMarker::Instance()->Start();
            ui->actionSelect->setEnabled( false );
            ui->actionLoad->setEnabled( false );
            ui->actionMark->setEnabled( false );
            ui->actionRun->setEnabled( false );
            ui->actionStop->setEnabled( false );
            if ( mainWindow != nullptr )
            {
                mainWindow->SetNavEnable( false );
            }
        }
    }
    catch ( HxException ex )
    {
        HxSystemError::Instance()->ErrorReport( ex );
        ex.where = "Khắc";
        //        Message::error(ex.message);
    }
}

void HxMarkWindow::on_actionStop_triggered()
{
    try
    {
        if ( HxActuator::SetEnable( false ) )
        {
            HxMarker::Instance()->Stop();
            ui->actionSelect->setEnabled( true );
            ui->actionLoad->setEnabled( false );
            ui->actionMark->setEnabled( false );
            ui->actionRun->setEnabled( false );
            ui->actionStop->setEnabled( false );
            if ( mainWindow != nullptr )
            {
                mainWindow->SetNavEnable( true );
            }
        }
    }
    catch ( HxException ex )
    {
        //        Message::error(ex.message);
        ex.where = "Dừng khắc";
        HxSystemError::Instance()->ErrorReport( ex );
    }

}

void HxMarkWindow::on_actionMark_triggered()
{
    try
    {
        HxMarker::Instance()->Mark( true );
        HxMessage::show( "Đã khắc xong !", "In test" );
    }
    catch ( HxException ex )
    {
        //        Message::error(ex.message);
        ex.where = "In test";
        HxSystemError::Instance()->ErrorReport( ex );
    }
}

void HxMarkWindow::on_tbvSelector_doubleClicked( const QModelIndex& index )
{
    int row = index.row();
    QString lotName = ui->tbvSelector->Item( row, 0 )->text();
    auto lot = HxLOT::Find( lotName );
    if ( HxMarker::Instance()->Select( lot ) )
    {
        ui->stackedWidget->setCurrentWidget( ui->pageMark );
        ShowLotInfo( HxMarker::Instance()->lot );
        ShowLotStatus( HxMarker::Instance()->lot );
        ShowLotBlocks();
        UpdateUI();
    }
}

void HxMarkWindow::on_actionLoad_triggered()
{
    ShowLots();
    ShowLotInfo( HxMarker::Instance()->lot );
    ShowLotStatus( HxMarker::Instance()->lot );
    ShowLotBlocks();
}

