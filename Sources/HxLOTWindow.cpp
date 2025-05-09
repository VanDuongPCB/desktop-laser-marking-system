#include "HxLOTWindow.h"
#include "ui_hxlotwindow.h"
#include "HxLOTPropertyDialog.h"

#include "QLabel"

#include "HxMessage.h"
#include "HxMarker.h"
#include "HxConvert.h"

HxLOTWindow::HxLOTWindow( QWidget* parent ) :QMainWindow( parent ), ui( new Ui::LotWindow )
{
    ui->setupUi( this );

    ui->tbvLots->setHeaders( { "Tên","Kiểu in","Model","Sản lượng","Seri đầu","Seri cuối","Tiến độ","MAC đầu","MAC cuối","Trạng thái" } );
    ui->tbvLots->setColumnWidth( 0, 200 );
    ui->tbvLots->setColumnWidth( 2, 250 );
    ui->tbvLots->setColumnWidth( 7, 120 );
    ui->tbvLots->setColumnWidth( 8, 120 );
    ui->tbvLots->setColumnWidth( 9, 120 );

    QLabel* pLabel = new QLabel( "Tìm kiếm nhanh" );
    ui->toolBar->addWidget( pLabel );
    m_pSearchTextBox = new QLineEdit();
    m_pSearchTextBox->setSizePolicy( QSizePolicy::Preferred, QSizePolicy::Preferred );
    m_pSearchTextBox->setMinimumWidth( 300 );
    m_pSearchTextBox->setClearButtonEnabled( true );
    ui->toolBar->addWidget( m_pSearchTextBox );

    connect( m_pSearchTextBox, &QLineEdit::textChanged, this, &HxLOTWindow::OnFilter );

    //connect( HxMarker::instance(), &HxMarker::printed, this, &HxLotWindow::controllerPrinted );
    //showLot( "" );
}

HxLOTWindow::~HxLOTWindow()
{
    delete m_pSearchTextBox;
    delete ui;
}

void HxLOTWindow::showEvent( QShowEvent* )
{
    OnRefresh();
}

void HxLOTWindow::OnRefresh()
{
    m_LOTs = LOTManager()->GetLOTs();
    OnFilter( m_pSearchTextBox->text() );
}

void HxLOTWindow::OnFilter( const QString& filter )
{
    QString input = filter.trimmed().toUpper();
    HxLOTPtrArray items;
    for ( auto& [name, pLOT] : m_LOTs )
    {
        bool bIsMatch = pLOT->Name().contains( input );

        if ( bIsMatch )
            items.push_back( pLOT );
    }
    ui->tbvLots->setRowCount( items.size() );
    int row = 0;
    for ( auto&  pLOT : items )
    {
        auto status = pLOT->Status();
        QString statusName = ProductStatusToString( status );
        QColor statusColor = ProductStatusToColor( status );

        ui->tbvLots->setText( row, "Tên", pLOT->Name() );
        ui->tbvLots->setText( row, "Kiểu in", pLOT->IsRePrint() ? "In lại" : "" );
        ui->tbvLots->setText( row, "Model", pLOT->Model() );
        ui->tbvLots->setText( row, "Sản lượng", QString::number( pLOT->Quantity() ) );
        ui->tbvLots->setText( row, "Seri đầu", pLOT->CounterStart() );
        ui->tbvLots->setText( row, "Seri cuối", pLOT->CounterEnd() );
        ui->tbvLots->setText( row, "Tiến độ", QString::number( pLOT->Progress() ) + "/" + QString::number( pLOT->Quantity() ) );
        ui->tbvLots->setText( row, "MAC đầu", pLOT->MACStart() );
        ui->tbvLots->setText( row, "MAC cuối", pLOT->MACEnd() );
        ui->tbvLots->setText( row, "Trạng thái", statusName );

        for ( int col = 0; col < ui->tbvLots->dataTable()->columnCount(); col++ )
        {
            ui->tbvLots->item( row, col )->setBackground( statusColor );
        }

        row++;
    }
}

void HxLOTWindow::OnNew()
{

}
void HxLOTWindow::OnDelete()
{

}
void HxLOTWindow::OnUpdate()
{

}
void HxLOTWindow::OnEdit()
{

}





void HxLOTWindow::showLot( QString filter )
{
     //filter = filter.trimmed();

     //if ( ui->tbvLots->headers.empty() )
     //{
     //    ui->tbvLots->setHeaders( { "Tên","Kiểu in","Model","Sản lượng","Seri đầu","Seri cuối","Tiến độ","MAC đầu","MAC cuối","Trạng thái" } );
     //    ui->tbvLots->setColumnWidth( 0, 150 );
     //    ui->tbvLots->setColumnWidth( 2, 150 );
     //    ui->tbvLots->setColumnWidth( 7, 150 );
     //    ui->tbvLots->setColumnWidth( 8, 150 );
     //    ui->tbvLots->setColumnWidth( 9, 150 );
     //}

     //HxLOT::sort();

     //int rows = HxLOT::items.size();
     //ui->tbvLots->setRowCount( rows );
     //for ( int row = 0; row < rows; row++ )
     //{
     //    int start = HxLOT::items[ row ]->counterStart.toInt();
     //    int end = start + HxLOT::items[ row ]->quantity - 1;
     //    QString status = HxLOT::items[ row ]->status();
     //    ui->tbvLots->setText( row, "Tên", HxLOT::items[ row ]->name );
     //    ui->tbvLots->setText( row, "Kiểu in", HxLOT::items[ row ]->isRePrint ? "In lại" : "" );
     //    ui->tbvLots->setText( row, "Model", HxLOT::items[ row ]->modelName );
     //    ui->tbvLots->setText( row, "Sản lượng", QString::number( HxLOT::items[ row ]->quantity ) );
     //    ui->tbvLots->setText( row, "Seri đầu", HxLOT::items[ row ]->counterStart );
     //    ui->tbvLots->setText( row, "Seri cuối", QString::number( end ).rightJustified( HxLOT::items[ row ]->counterStart.length(), '0' ) );
     //    ui->tbvLots->setText( row, "Tiến độ", QString::number( HxLOT::items[ row ]->progress ) + "/" + QString::number( HxLOT::items[ row ]->quantity ) );
     //    ui->tbvLots->setText( row, "MAC đầu", HxLOT::items[ row ]->macStart );
     //    ui->tbvLots->setText( row, "MAC cuối", HxLOT::items[ row ]->macEnd );
     //    ui->tbvLots->setText( row, "Trạng thái", status );

     //    QColor background = QColor( 255, 255, 255 );
     //    if ( status == "Đang sản xuất" )
     //    {
     //        background = QColor( 255, 255, 128 );
     //    }
     //    else if ( status == "Đã hoàn thành" )
     //    {
     //        background = QColor( 128, 255, 128 );
     //    }

     //    for ( int col = 0; col < ui->tbvLots->dataTable()->columnCount(); col++ )
     //    {
     //        ui->tbvLots->item( row, col )->setBackground( background );
     //    }
     //}


}


void HxLOTWindow::controllerPrinted( std::shared_ptr<HxLOT> lot )
{
    showLot( "" );
}

void HxLOTWindow::dataChanged()
{
    showLot( "" );
}


void HxLOTWindow::on_actionNew_triggered()
{
    HxLOTPropertyDialog dialog( "", m_LOTs, this );
    if ( dialog.exec() )
    {
        // showLot( "" );
        OnRefresh();
    }
}

void HxLOTWindow::on_actionRemove_triggered()
{
    QStringList names;
    for ( auto it : ui->tbvLots->selectionModel()->selectedRows() )
    {
        names.push_back( ui->tbvLots->item( it.row(), 0 )->text() );
    }

    if ( names.isEmpty() )
        return;

    if ( HxMsgQuestion( "Xóa các lot đang chọn ?", "Khoan đã" ) == QMessageBox::Yes )
    {
        LOTManager()->Removes( names );
        OnRefresh();
    }

    // std::vector<int> indexs;
    // for ( auto it : ui->tbvLots->selectionModel()->selectedRows() )
    // {
    //     indexs.push_back( it.row() );
    // }
    // std::sort( indexs.begin(), indexs.end(), []( int i1, int i2 ) { return i1 > i2; } );

    // if ( indexs.size() <= 0 ) return;
    // int ret = HxMsgQuestion( "Xóa các lot đang chọn ?", "Khoan đã" );
    // if ( ret == QMessageBox::Yes )
    // {
    //     for ( auto index : indexs )
    //     {
    //         if ( HxLOT::items[ index ]->status() == "Chưa sản xuất" )
    //         {
    //             HxLOT::remove( index );
    //         }
    //     }
    // }
    // HxLOT::saveAll();
    // showLot( "" );
}

void HxLOTWindow::on_actionSave_triggered()
{
    // HxLOT::saveAll();
}

void HxLOTWindow::on_actionLoad_triggered()
{
    // showLot( "" );
    OnRefresh();
}

void HxLOTWindow::on_tbvLots_doubleClicked( const QModelIndex& index )
{
    int row = index.row();
    if ( row < 0 )
        return;
    QString lotName = ui->tbvLots->item( row, 0 )->text();
    HxLOTPropertyDialog dialog( lotName, m_LOTs, this );
    if ( dialog.exec() )
    {
        //OnRefresh();
    }
}
