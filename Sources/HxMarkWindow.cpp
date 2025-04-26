#include "HxMarkWindow.h"
#include "ui_hxmarkwindow.h"


#include "HxMarker.h"
#include "HxMessage.h"
#include "HxException.h"
#include "HxSystemError.h"
#include "HxLaserDevice.h"
#include "HxActuator.h"






HxMarkWindow::HxMarkWindow( QWidget* parent ) : QMainWindow( parent ), ui( new Ui::MarkWindow )
{
    mainWindow = ( HxMainWindow* )parent;
    ui->setupUi( this );
    connect( HxMarker::instance(), &HxMarker::printed, this, &HxMarkWindow::controllerPrinted );
    connect( HxMarker::instance(), &HxMarker::started, this, &HxMarkWindow::markStarted );
    connect( HxMarker::instance(), &HxMarker::stopped, this, &HxMarkWindow::markStopped );
    connect( HxSystemError::instance(), &HxSystemError::reported, this, &HxMarkWindow::handleException );

    showLots();
    showLotInfo( nullptr );
    showLotStatus( nullptr );
    showLotBlocks();
    showExceptions();
    updateUI();
}

HxMarkWindow::~HxMarkWindow()
{
    delete ui;
}

void HxMarkWindow::showEvent( QShowEvent* )
{
    showLots();
    showLotInfo( HxMarker::instance()->lot );
    showLotStatus( HxMarker::instance()->lot );
    updateUI();
}

void HxMarkWindow::showLots()
{
    if ( ui->tbvSelector->headers.empty() )
    {
        ui->tbvSelector->setHeaders( { "Tên","Model","MAC đầu","MAC cuối","Sản lượng","Tiến độ","Trạng thái" } );
    }

    HxLOT::sort();

    int rows = ( int )HxLOT::items.size();
    ui->tbvSelector->setRowCount( rows );
    for ( int row = 0; row < rows; row++ )
    {
        QString status = HxLOT::items[ row ]->status();

        ui->tbvSelector->setText( row, "Tên", HxLOT::items[ row ]->name );
        ui->tbvSelector->setText( row, "Model", HxLOT::items[ row ]->modelName );
        ui->tbvSelector->setText( row, "MAC đầu", HxLOT::items[ row ]->macStart );
        ui->tbvSelector->setText( row, "MAC cuối", HxLOT::items[ row ]->macEnd );
        ui->tbvSelector->setText( row, "Sản lượng", QString::number( HxLOT::items[ row ]->quantity ) );
        ui->tbvSelector->setText( row, "Tiến độ", QString::number( HxLOT::items[ row ]->progress ) + "/" + QString::number( HxLOT::items[ row ]->quantity ) );
        ui->tbvSelector->setText( row, "Trạng thái", status );

        QColor background = QColor( 255, 255, 255 );
        if ( status == "Đang sản xuất" )
        {
            background = QColor( 255, 255, 128 );
        }
        else if ( status == "Đã hoàn thành" )
        {
            background = QColor( 128, 255, 128 );
        }

        for ( int col = 0; col < ui->tbvSelector->dataTable()->columnCount(); col++ )
        {
            ui->tbvSelector->item( row, col )->setBackground( background );
        }
    }
}

void HxMarkWindow::showLotInfo( std::shared_ptr<HxLOT> lot )
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
        ui->lblStatus->setText( lot->status() );
        ui->lblModelName->setText( lot->modelName );
        auto model = HxModel::find( lot->modelName );
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

void HxMarkWindow::showLotStatus( std::shared_ptr<HxLOT> lot )
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
        ui->lblStatus->setText( lot->status() );
        ui->lblCount->setText( QString::number( lot->progress ) );
        ui->pgbProgress->setMaximum( lot->quantity );
        ui->pgbProgress->setValue( lot->progress );

        if ( lot->status() == "Chưa sản xuất" )
        {
            ui->lblStatus->setStyleSheet( "background:#888" );
        }
        else if ( lot->status() == "Đang sản xuất" )
        {
            ui->lblStatus->setStyleSheet( "background:#ff8" );
        }
        else
        {
            ui->lblStatus->setStyleSheet( "background:#8f8" );
        }
    }
}

void HxMarkWindow::showLotBlocks()
{
    if ( ui->tbvBlocks->headers.empty() )
    {
        ui->tbvBlocks->setHeaders( { "Block","Dữ liệu" } );
    }
    ui->tbvBlocks->setRowCount( 0 );
    auto lot = HxMarker::instance()->lot;
    if ( lot == nullptr ) return;
    auto model = HxMarker::instance()->model;
    auto design = HxMarker::instance()->design;
    int codeIndex = -1;
    if ( design != nullptr )
    {
        codeIndex = design->indexOfBlockCode();
    }
    auto tempLot = std::make_shared<HxLOT>( HxLOT() );
    tempLot.get()[ 0 ] = lot.get()[ 0 ];
    tempLot->progress--;
    if ( tempLot->progress < 0 )
    {
        tempLot->progress = 0;
    }

    QMap<int, QString> blockdatas = HxBlock::gen( design, tempLot, model );
    QList<int> blockNums = blockdatas.keys();
    std::sort( blockNums.begin(), blockNums.end() );
    ui->tbvBlocks->setRowCount( blockNums.size() );
    for ( int i = 0; i < blockNums.size(); i++ )
    {
        ui->tbvBlocks->setText( i, "Block", QString::number( blockNums[ i ] ).rightJustified( 3, '0' ) );
        ui->tbvBlocks->setText( i, "Dữ liệu", blockdatas[ blockNums[ i ] ] );
        if ( blockNums[ i ] == codeIndex )
        {
            ui->lblBarcode->setText( blockdatas[ blockNums[ i ] ] );
        }
    }

    ui->lblFormat->setText( "" );
    if ( codeIndex > 0 )
    {
        ui->lblFormat->setText( design->blocks[ codeIndex ].data );
    }
}

void HxMarkWindow::showExceptions()
{
    if ( ui->tbvErrors->headers.empty() )
    {
        ui->tbvErrors->setHeaders( { "Thời gian","Từ","Lỗi" } );
    }
    int rows = ( int )exceptions.size();
    ui->tbvErrors->setRowCount( rows );
    for ( int row = 0; row < rows; row++ )
    {
        ui->tbvErrors->setText( row, 0, exceptions[ row ].time );
        ui->tbvErrors->setText( row, 1, exceptions[ row ].where );
        ui->tbvErrors->setText( row, 2, exceptions[ row ].message.replace( "\n", ", " ) );
    }
    ui->tbvErrors->scrollToBottom();
}

void HxMarkWindow::updateUI()
{
    auto lot = HxMarker::instance()->lot;
    bool hasData = lot != nullptr;
    bool canMark = hasData && lot->isCompleted() == false;
    bool busy = HxMarker::instance()->isBusy();

    ui->actionSelect->setEnabled( !busy );
    ui->actionLoad->setEnabled( !busy );
    ui->actionMark->setEnabled( hasData && !busy );
    ui->actionRun->setEnabled( canMark && !busy );
    ui->actionStop->setEnabled( busy );
}

void HxMarkWindow::handleException( HxException ex )
{
    exceptions.push_back( ex );
    showExceptions();
}

void HxMarkWindow::markStarted()
{
    updateUI();
}

void HxMarkWindow::markStopped()
{
    updateUI();
}

void HxMarkWindow::controllerPrinted( std::shared_ptr<HxLOT> lot )
{
    showLotStatus( lot );
    showLotBlocks();
}

void HxMarkWindow::on_actionSelect_triggered()
{
    ui->stackedWidget->setCurrentWidget( ui->pageSelector );
    updateUI();
}

void HxMarkWindow::on_actionRun_triggered()
{
    try
    {
        if ( HxActuator::setEnable( true ) )
        {
            HxMarker::instance()->start();
            ui->actionSelect->setEnabled( false );
            ui->actionLoad->setEnabled( false );
            ui->actionMark->setEnabled( false );
            ui->actionRun->setEnabled( false );
            ui->actionStop->setEnabled( false );
            if ( mainWindow != nullptr )
            {
                mainWindow->setNavEnable( false );
            }
        }
    }
    catch ( HxException ex )
    {
        HxSystemError::instance()->errorReport( ex );
        ex.where = "Khắc";
        //        Message::error(ex.message);
    }
}

void HxMarkWindow::on_actionStop_triggered()
{
    try
    {
        if ( HxActuator::setEnable( false ) )
        {
            HxMarker::instance()->stop();
            ui->actionSelect->setEnabled( true );
            ui->actionLoad->setEnabled( false );
            ui->actionMark->setEnabled( false );
            ui->actionRun->setEnabled( false );
            ui->actionStop->setEnabled( false );
            if ( mainWindow != nullptr )
            {
                mainWindow->setNavEnable( true );
            }
        }
    }
    catch ( HxException ex )
    {
        //        Message::error(ex.message);
        ex.where = "Dừng khắc";
        HxSystemError::instance()->errorReport( ex );
    }

}

void HxMarkWindow::on_actionMark_triggered()
{
    try
    {
        HxMarker::instance()->mark( true );
        HxMessage::show( "Đã khắc xong !", "In test" );
    }
    catch ( HxException ex )
    {
        //        Message::error(ex.message);
        ex.where = "In test";
        HxSystemError::instance()->errorReport( ex );
    }
}

void HxMarkWindow::on_tbvSelector_doubleClicked( const QModelIndex& index )
{
    int row = index.row();
    QString lotName = ui->tbvSelector->item( row, 0 )->text();
    auto lot = HxLOT::find( lotName );
    if ( HxMarker::instance()->select( lot ) )
    {
        ui->stackedWidget->setCurrentWidget( ui->pageMark );
        showLotInfo( HxMarker::instance()->lot );
        showLotStatus( HxMarker::instance()->lot );
        showLotBlocks();
        updateUI();
    }
}

void HxMarkWindow::on_actionLoad_triggered()
{
    showLots();
    showLotInfo( HxMarker::instance()->lot );
    showLotStatus( HxMarker::instance()->lot );
    showLotBlocks();
}

