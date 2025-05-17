#include "HxLOTSelector.h"
#include "ui_hxlotselector.h"
#include "HxLOT.h"
#include "HxConvert.h"
#include "HxMarker.h"

HxLOTSelector::HxLOTSelector( QWidget* parent ) : QDialog( parent ), ui( new Ui::LOTSelector )
{
    ui->setupUi( this );

    ui->tbvLOTs->setHeaders( { "Tên","Model","MAC đầu","MAC cuối","Sản lượng","Tiến độ","Trạng thái" } );
    ui->tbvLOTs->setColumnWidth( 0, 200 );
    ui->tbvLOTs->setColumnWidth( 1, 250 );

    connect( ui->tbvLOTs, &HxTableView::doubleClicked, this, &HxLOTSelector::OnSelected );
    OnShow();
}

HxLOTSelector::~HxLOTSelector()
{
}

void HxLOTSelector::OnShow()
{
    HxLOTPtrMap items = LOTManager()->GetLOTs( QDate::currentDate() );
    ui->tbvLOTs->setRowCount( items.size() );
    int row = 0;
    for ( auto& [name, pLOT] : items )
    {
        auto status = pLOT->Status();
        QString statusName = ProductStatusToString( status );
        QColor statusColor = ProductStatusToColor( status );

        ui->tbvLOTs->setText( row, "Tên", pLOT->Name() );
        ui->tbvLOTs->setText( row, "Kiểu in", pLOT->IsRePrint() ? "In lại" : "");
        ui->tbvLOTs->setText( row, "Model", pLOT->Model() );
        ui->tbvLOTs->setText( row, "Sản lượng", QString::number( pLOT->Quantity() ) );
        ui->tbvLOTs->setText( row, "Seri đầu", pLOT->CounterStart() );
        ui->tbvLOTs->setText( row, "Seri cuối", pLOT->CounterEnd() );
        ui->tbvLOTs->setText( row, "Tiến độ", QString::number( pLOT->Progress() ) + "/" + QString::number( pLOT->Quantity() ) );
        ui->tbvLOTs->setText( row, "MAC đầu", pLOT->MACStart() );
        ui->tbvLOTs->setText( row, "MAC cuối", pLOT->MACEnd() );
        ui->tbvLOTs->setText( row, "Trạng thái", statusName );

        for ( int col = 0; col < ui->tbvLOTs->dataTable()->columnCount(); col++ )
        {
            ui->tbvLOTs->item( row, col )->setBackground( statusColor );
        }

        row++;
    }
}

void HxLOTSelector::OnSelected( const QModelIndex& index )
{
    int row = index.row();
    QString lotName = ui->tbvLOTs->item( row, 0 )->text();
    if ( Marker()->Setup( HxMarker::eModeMarking, lotName ) )
    {
        close();
        setResult( 1 );
    }
}