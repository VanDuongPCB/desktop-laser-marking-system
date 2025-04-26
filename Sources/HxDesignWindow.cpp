#include "HxDesignWindow.h"
#include "ui_hxdesignwindow.h"
#include "HxModel.h"
#include "HxLOT.h"

HxDesignWindow::HxDesignWindow( QWidget* parent ) : QMainWindow( parent ), ui( new Ui::DesignWindow )
{
    ui->setupUi( this );

}

HxDesignWindow::~HxDesignWindow()
{
    delete ui;
}

void HxDesignWindow::showEvent( QShowEvent* )
{
    showDesigns( "" );
    showBlocks();
    showParams();
}

void HxDesignWindow::showDesigns( QString filter )
{
    designs.clear();
    filter = filter.trimmed().toUpper();
    if ( ui->tbvDesign->headers.empty() )
    {
        ui->tbvDesign->setHeaders( { "Chương trình" } );
    }

    for ( auto& item : HxDesign::items )
    {
        if ( filter == "" || item->name.contains( filter ) )
        {
            designs.push_back( item );
        }
    }

    int rows = designs.size();
    ui->tbvDesign->setRowCount( rows );
    for ( int row = 0; row < rows; row++ )
    {
        ui->tbvDesign->setText( row, 0, designs[ row ]->name );
    }
}

void HxDesignWindow::showSize()
{
    if ( design != nullptr )
    {
        ui->spxDesignWidth->setValue( design->width );
        ui->spxDesignHeight->setValue( design->height );
    }
    else
    {
        ui->spxDesignWidth->setValue( 2.5 );
        ui->spxDesignHeight->setValue( 2.5 );
    }
}

void HxDesignWindow::showBlocks()
{
    if ( ui->tbvBlocks->headers.empty() )
    {
        ui->tbvBlocks->setHeaders( { "Block","Là mã ?","Độ dài","Nội dung" } );
        ui->tbvBlocks->setColumnWidth( 0, 80 );
        ui->tbvBlocks->setColumnWidth( 1, 80 );
        ui->tbvBlocks->setColumnWidth( 2, 80 );
    }
    QStandardItemModel* model = ( QStandardItemModel* )ui->tbvBlocks->model();
    disconnect( model, &QStandardItemModel::itemChanged, this, &HxDesignWindow::blockChanged );
    int rows = 32;
    ui->tbvBlocks->setRowCount( rows );
    for ( int row = 0; row < rows; row++ )
    {
        ui->tbvBlocks->setText( row, "Block", QString::number( row ).rightJustified( 3, '0' ) );
        ui->tbvBlocks->item( row, "Block" )->setFlags( ui->tbvBlocks->item( row, 0 )->flags() & ~Qt::ItemIsEditable );
        ui->tbvBlocks->setText( row, "Là mã ?", "" );
        ui->tbvBlocks->item( row, "Là mã ?" )->setCheckable( true );
        ui->tbvBlocks->item( row, "Là mã ?" )->setCheckState( Qt::Unchecked );
        ui->tbvBlocks->item( row, "Là mã ?" )->setFlags( ui->tbvBlocks->item( row, 1 )->flags() & ~Qt::ItemIsEditable );
        ui->tbvBlocks->setText( row, "Độ dài", "0" );
        ui->tbvBlocks->setText( row, "Nội dung", "" );
    }
    if ( design != nullptr )
    {
        QList<int> nums = design->blocks.keys();
        for ( auto num : nums )
        {
            HxBlock block = design->blocks[ num ];
            ui->tbvBlocks->item( num, "Là mã ?" )->setCheckState( block.isCode ? Qt::Checked : Qt::Unchecked );
            ui->tbvBlocks->setText( num, "Độ dài", QString::number( block.textLen ) );
            ui->tbvBlocks->setText( num, "Nội dung", block.data );
        }
    }
    connect( model, &QStandardItemModel::itemChanged, this, &HxDesignWindow::blockChanged );
}

void HxDesignWindow::showParams()
{
    // model
    if ( ui->tbvModelParams->headers.empty() )
    {
        ui->tbvModelParams->setHeaders( { "Thông số model" } );
    }
    QStringList modelParams = HxModel::paramNames();
    int rows = modelParams.size();
    ui->tbvModelParams->setRowCount( rows );
    for ( int row = 0; row < rows; row++ )
    {
        ui->tbvModelParams->setText( row, 0, modelParams[ row ] );
    }


    // lot
    if ( ui->tbvLotParams->headers.empty() )
    {
        ui->tbvLotParams->setHeaders( { "Thông số lot" } );
    }
    QStringList lotParams = HxLOT::paramNames();
    rows = lotParams.size();
    ui->tbvLotParams->setRowCount( rows );
    for ( int row = 0; row < rows; row++ )
    {
        ui->tbvLotParams->setText( row, 0, lotParams[ row ] );
    }
}

void HxDesignWindow::blockChanged( QStandardItem* item )
{
    if ( design == nullptr ) return;
    int row = item->row();
    bool code = ui->tbvBlocks->item( row, "Là mã ?" )->checkState() == Qt::Checked;
    int len = ui->tbvBlocks->item( row, "Độ dài" )->text().trimmed().toInt();
    QString data = ui->tbvBlocks->item( row, "Nội dung" )->text();
    if ( !design->blocks.contains( row ) )
    {
        design->blocks.insert( row, HxBlock() );
    }
    design->blocks[ row ].isCode = code;
    design->blocks[ row ].textLen = len;
    design->blocks[ row ].data = data;
    HxDesign::save( design );
}

void HxDesignWindow::on_actionSave_triggered()
{
    HxDesign::save();
}

void HxDesignWindow::on_actionLoad_triggered()
{
    design = nullptr;
    HxDesign::load();
    showDesigns( "" );
    showBlocks();
}

void HxDesignWindow::on_tbvDesign_pressed( const QModelIndex& index )
{
    int row = index.row();
    if ( row < 0 ) return;
    design = designs[ row ];
    showSize();
    showBlocks();
}

void HxDesignWindow::on_spxDesignWidth_valueChanged( double arg1 )
{
    if ( design == nullptr ) return;
    design->width = arg1;
    HxDesign::save( design );
}

void HxDesignWindow::on_spxDesignHeight_valueChanged( double arg1 )
{
    if ( design == nullptr ) return;
    design->height = arg1;
    HxDesign::save( design );
}

void HxDesignWindow::on_tbvModelParams_doubleClicked( const QModelIndex& index )
{
    int row = ui->tbvBlocks->currentIndex().row();
    if ( row < 0 ) return;
    QString param = "MODEL." + ui->tbvModelParams->item( index.row(), 0 )->text();
    QString oldText = ui->tbvBlocks->item( row, "Nội dung" )->text();
    if ( oldText.length() > 0 ) oldText += ",";
    oldText += param;
    ui->tbvBlocks->setText( row, "Nội dung", oldText );
    HxDesign::save( design );
}


void HxDesignWindow::on_tbvLotParams_doubleClicked( const QModelIndex& index )
{
    int row = ui->tbvBlocks->currentIndex().row();
    if ( row < 0 ) return;
    QString param = "LOT." + ui->tbvLotParams->item( index.row(), 0 )->text();
    QString oldText = ui->tbvBlocks->item( row, "Nội dung" )->text();
    if ( oldText.length() > 0 ) oldText += ",";
    oldText += param;
    ui->tbvBlocks->setText( row, "Nội dung", oldText );
    HxDesign::save( design );
}

