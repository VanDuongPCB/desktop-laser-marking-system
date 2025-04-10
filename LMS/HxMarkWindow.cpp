#include "HxMarkWindow.h"
#include "ui_HxMarkWindow.h"


#include "HxMarker.h"
#include "HxMessage.h"
#include "HxException.h"
#include "HxSystemError.h"
#include <HxLaser.h>
#include <HxPLC.h>

#include "HxConvert.h"
#include <HxDebugger.h>



HxMarkWindow::HxMarkWindow( QWidget* parent ) : QMainWindow( parent ), ui( new Ui::MarkWindow )
{
    mainWindow = ( HxMainWindow* )parent;
    ui->setupUi( this );

    ui->tbvSelector->setHeaders( { "Tên","Model","MAC đầu","MAC cuối","Sản lượng","Tiến độ","Trạng thái" } );

    connect( HxMarker::instance(), &HxMarker::printed, this, &HxMarkWindow::controllerPrinted );
    connect( HxMarker::instance(), &HxMarker::started, this, &HxMarkWindow::markStarted );
    connect( HxMarker::instance(), &HxMarker::stopped, this, &HxMarkWindow::markStopped );
    connect( HxSystemError::instance(), &HxSystemError::reported, this, &HxMarkWindow::handleException );

    // showLots();
    // showLotInfo( nullptr );
    // showLotStatus( nullptr );
    // showLotBlocks();
    // showExceptions();
    // updateUI();
}

HxMarkWindow::~HxMarkWindow()
{
    delete ui;
}

void HxMarkWindow::showEvent( QShowEvent* )
{
    showLots();
    // showLotInfo( HxMarker::instance()->lot );
    // showLotStatus( HxMarker::instance()->lot );
    // updateUI();
}

void HxMarkWindow::showLots()
{
    auto LOTs = GetLOTManager()->GetLOTs();
    std::vector<HxLOTPtr> needShowLots;
    for(auto [name, pLOT]: LOTs)
    {
        if(name.startsWith("2_"))
            break;
        needShowLots.push_back(pLOT);
    }

    int rows = needShowLots.size();
    ui->tbvSelector->setRowCount(rows);
    int row = 0;
    for(auto &pLOT : needShowLots)
    {
        auto status = pLOT->Status();
        QString statusName = ProductStatusToString(status);
        QColor statusColor = ProductStatusToColor(status);

        ui->tbvSelector->setText( row, "Tên", pLOT->m_name );
        ui->tbvSelector->setText( row, "Model", pLOT->m_modelName );
        ui->tbvSelector->setText( row, "MAC đầu", pLOT->m_macStart );
        ui->tbvSelector->setText( row, "MAC cuối", pLOT->m_macEnd );
        ui->tbvSelector->setText( row, "Sản lượng", QString::number( pLOT->m_quantity ) );
        ui->tbvSelector->setText( row, "Tiến độ", QString::number( pLOT->m_progress ) + "/" + QString::number( pLOT->m_quantity ) );
        ui->tbvSelector->setText( row, "Trạng thái", statusName );


        for ( int col = 0; col < ui->tbvSelector->dataTable()->columnCount(); col++ )
        {
            ui->tbvSelector->item( row, col )->setBackground( statusColor );
        }

        row++;
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
        ui->lblLotName->setText( lot->m_name );
        ui->lblSerialStart->setText( lot->m_macStart );
        ui->lblSerialEnd->setText( lot->m_macEnd );
        ui->lblQuantity->setText( QString::number( lot->m_quantity ) );
        ui->lblStatus->setText( ProductStatusToString( lot->Status() ).toUpper() );
        ui->lblModelName->setText( lot->m_modelName );
        auto model = GetModelManager()->GetModel( lot->m_modelName );
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
        ui->lblStatus->setText( ProductStatusToString( lot->Status() ).toUpper() );
        ui->lblCount->setText( QString::number( lot->m_progress ) );
        ui->pgbProgress->setMaximum( lot->m_quantity );
        ui->pgbProgress->setValue( lot->m_progress );

        switch (lot->Status()) {
        case HxLOT::ePending:
            ui->lblStatus->setStyleSheet( "background:#888" );
            break;
        case HxLOT::eProduct:
            ui->lblStatus->setStyleSheet( "background:#ff8" );
            break;
        case HxLOT::eCompleted:
            ui->lblStatus->setStyleSheet( "background:#8f8" );
            break;
        default:
            break;
        }

        // if ( lot->status() == "Chưa sản xuất" )
        // {
        //     ui->lblStatus->setStyleSheet( "background:#888" );
        // }
        // else if ( lot->status() == "Đang sản xuất" )
        // {
        //     ui->lblStatus->setStyleSheet( "background:#ff8" );
        // }
        // else
        // {
        //     ui->lblStatus->setStyleSheet( "background:#8f8" );
        // }
    }
}

void HxMarkWindow::showLotBlocks()
{
    if ( ui->tbvBlocks->headers.empty() )
    {
        ui->tbvBlocks->setHeaders( { "Block","Dữ liệu" } );
    }
    ui->tbvBlocks->setRowCount( 0 );
    auto lot = HxMarker::instance()->m_pLOT;
    if ( lot == nullptr ) return;
    auto model = HxMarker::instance()->m_pModel;
    auto design = HxMarker::instance()->design;
    int codeIndex = -1;
    if ( design != nullptr )
    {
        codeIndex = design->indexOfBlockCode();
    }
    auto tempLot = std::make_shared<HxLOT>( HxLOT() );
    tempLot.get()[ 0 ] = lot.get()[ 0 ];
    tempLot->m_progress--;
    if ( tempLot->m_progress < 0 )
    {
        tempLot->m_progress = 0;
    }

    QMap<int, QString> blockdatas = HxBlockInfo::gen( design, tempLot, model );
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
    auto lot = HxMarker::instance()->m_pLOT;
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
        if ( GetPLC()->setEnable( true ) )
        {
            HxMarker::instance()->start();
            ui->actionSelect->setEnabled( false );
            ui->actionLoad->setEnabled( false );
            ui->actionMark->setEnabled( false );
            ui->actionRun->setEnabled( false );
            ui->actionStop->setEnabled( true );
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
        if ( GetPLC()->setEnable( false ) )
        {
            HxMarker::instance()->stop();
            ui->actionSelect->setEnabled( true );
            ui->actionLoad->setEnabled( true );
            ui->actionMark->setEnabled( true );
            ui->actionRun->setEnabled( true );
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
        HxMsgInfo( "Đã khắc xong !", "In test" );
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
    auto pLOT = GetLOTManager()->GetLOT(lotName);
    if(HxMarker::instance()->Select(pLOT))
    {
        ui->stackedWidget->setCurrentWidget( ui->pageMark );
        showLotInfo( HxMarker::instance()->m_pLOT );
        showLotStatus( HxMarker::instance()->m_pLOT );
        showLotBlocks();
        updateUI();
    }
}

void HxMarkWindow::on_actionLoad_triggered()
{
    showLots();
    showLotInfo( HxMarker::instance()->m_pLOT );
    showLotStatus( HxMarker::instance()->m_pLOT );
    showLotBlocks();
}

