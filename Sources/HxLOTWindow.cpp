#include "HxLOTWindow.h"
#include "HxMessage.h"
#include "ui_hxlotwindow.h"
#include "HxMarker.h"

HxLOTWindow::HxLOTWindow( QWidget* parent ) :QMainWindow( parent ), ui( new Ui::LOTWindow )
{
    ui->setupUi( this );
    connect( HxMarker::Instance(), &HxMarker::printed, this, &HxLOTWindow::ControllerPrinted );
    ShowLot( "" );
}

HxLOTWindow::~HxLOTWindow()
{
    delete ui;
}

void HxLOTWindow::showEvent( QShowEvent* )
{
    ShowLot( "" );
}

void HxLOTWindow::ShowLot( QString filter )
{
    filter = filter.trimmed();

    if ( ui->tbvLots->headers.empty() )
    {
        ui->tbvLots->SetHeaders( { "Tên","Kiểu in","Model","Sản lượng","Seri đầu","Seri cuối","Tiến độ","MAC đầu","MAC cuối","Trạng thái" } );
        ui->tbvLots->setColumnWidth( 0, 150 );
        ui->tbvLots->setColumnWidth( 2, 150 );
        ui->tbvLots->setColumnWidth( 7, 150 );
        ui->tbvLots->setColumnWidth( 8, 150 );
        ui->tbvLots->setColumnWidth( 9, 150 );
    }

    HxLOT::Sort();

    int rows = HxLOT::items.size();
    ui->tbvLots->SetRowCount( rows );
    for ( int row = 0; row < rows; row++ )
    {
        int start = HxLOT::items[ row ]->counterStart.toInt();
        int end = start + HxLOT::items[ row ]->quantity - 1;
        QString status = HxLOT::items[ row ]->Status();
        ui->tbvLots->SetText( row, "Tên", HxLOT::items[ row ]->name );
        ui->tbvLots->SetText( row, "Kiểu in", HxLOT::items[ row ]->isRePrint ? "In lại" : "" );
        ui->tbvLots->SetText( row, "Model", HxLOT::items[ row ]->modelName );
        ui->tbvLots->SetText( row, "Sản lượng", QString::number( HxLOT::items[ row ]->quantity ) );
        ui->tbvLots->SetText( row, "Seri đầu", HxLOT::items[ row ]->counterStart );
        ui->tbvLots->SetText( row, "Seri cuối", QString::number( end ).rightJustified( HxLOT::items[ row ]->counterStart.length(), '0' ) );
        ui->tbvLots->SetText( row, "Tiến độ", QString::number( HxLOT::items[ row ]->progress ) + "/" + QString::number( HxLOT::items[ row ]->quantity ) );
        ui->tbvLots->SetText( row, "MAC đầu", HxLOT::items[ row ]->macStart );
        ui->tbvLots->SetText( row, "MAC cuối", HxLOT::items[ row ]->macEnd );
        ui->tbvLots->SetText( row, "Trạng thái", status );

        QColor background = QColor( 255, 255, 255 );
        if ( status == "Đang sản xuất" )
        {
            background = QColor( 255, 255, 128 );
        }
        else if ( status == "Đã hoàn thành" )
        {
            background = QColor( 128, 255, 128 );
        }

        for ( int col = 0; col < ui->tbvLots->DataTable()->columnCount(); col++ )
        {
            ui->tbvLots->Item( row, col )->setBackground( background );
        }
    }
}


void HxLOTWindow::ControllerPrinted( std::shared_ptr<HxLOT> lot )
{
    ShowLot( "" );
}

void HxLOTWindow::DataChanged()
{
    ShowLot( "" );
}


void HxLOTWindow::on_actionNew_triggered()
{
    HxLOTPropertyDialog dialog( this );
    dialog.SetData( nullptr );
    if ( dialog.exec() )
    {
        ShowLot( "" );
    }
}

void HxLOTWindow::on_actionRemove_triggered()
{
    std::vector<int> indexs;
    for ( auto it : ui->tbvLots->selectionModel()->selectedRows() )
    {
        indexs.push_back( it.row() );
    }
    std::sort( indexs.begin(), indexs.end(), []( int i1, int i2 ) { return i1 > i2; } );

    if ( indexs.size() <= 0 ) return;
    int ret = HxMessage::warning( "Xóa các lot đang chọn ?", "Khoan đã" );
    if ( ret == QMessageBox::Yes )
    {
        for ( auto index : indexs )
        {
            if ( HxLOT::items[ index ]->Status() == "Chưa sản xuất" )
            {
                HxLOT::Remove( index );
            }
        }
    }
    HxLOT::SaveAll();
    ShowLot( "" );
}

void HxLOTWindow::on_actionSave_triggered()
{
    HxLOT::SaveAll();
}

void HxLOTWindow::on_actionLoad_triggered()
{
    ShowLot( "" );
}

void HxLOTWindow::on_tbvLots_doubleClicked( const QModelIndex& index )
{
    int row = index.row();
    if ( row < 0 ) return;
    QString lotName = ui->tbvLots->Item( row, 0 )->text();
    auto lot = HxLOT::Find( lotName );
    HxLOTPropertyDialog dialog( this );
    dialog.SetData( lot );
    if ( dialog.exec() )
    {
        ShowLot( "" );
    }
}
